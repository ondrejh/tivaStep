//*****************************************************************************
//
// project.c - Simple project to use as a starting point for more complex
//             projects.
//
// Copyright (c) 2013-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision 2.1.0.12573 of the Tiva Firmware Development Package.
//
//*****************************************************************************

#include "includes.h"

#include "timer.c"
#include "mbrtu_serv.h"
#include "motor.h"
#include "rs485.h"
#include "rtc.h"
#include "params.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Simple Project (project)</h1>
//!
//! A very simple example that can be used as a starting point for more complex
//! projects.  Most notably, this project is fully TI BSD licensed, so any and
//! all of the code (including the startup code) can be used as allowed by that
//! license.
//!
//! The provided code simply toggles a GPIO using the Tiva Peripheral Driver
//! Library.
//
//*****************************************************************************

// onboard leds
#define LED_RED_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1);}while(0)
#define LED_RED_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0);}while(0)
#define LED_BLUE_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,GPIO_PIN_2);}while(0)
#define LED_BLUE_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0);}while(0)
#define LED_GREEN_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,GPIO_PIN_3);}while(0)
#define LED_GREEN_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);}while(0)

// spindle unit leds
#define SU_LED_GREEN_ON() do{GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_6,0);}while(0)
#define SU_LED_GREEN_OFF() do{GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_6,GPIO_PIN_6);}while(0)
#define SU_LED_RED_ON() do{GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_7,0);}while(0)
#define SU_LED_RED_OFF() do{GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_7,GPIO_PIN_7);}while(0)

// onboard buttons
#define BTN1 (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0)
#define BTN2 (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0)

// external switches
#define SW1 (GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_0)==0)
#define SW2 (GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1)==0)
#define SW3 (GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_2)==0)
#define SW4 (GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_3)==0)

#define SWn1(x) ((x==0)?SW1:SW3)
#define SWn2(x) ((x==0)?SW2:SW4)

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

void error_state(void)
{
    LED_RED_ON();
}

#define STATUS_OFFSET 0
#define COMMAND_OFFSET 1
#define EESAVE_COMMAND_BIT 0x0100

#define TEST_SPEED 2000.0
#define TEST_ACCEL 5000.0
#define TEST_MIN_SPEED 10.0

#define M1_TEST_SPEED_OFFSET 4
#define M1_TEST_ACCEL_OFFSET 5
#define M2_TEST_SPEED_OFFSET 6
#define M2_TEST_ACCEL_OFFSET 7

#define RTC_SET_OFFSET 4 // 32 bit register to set rtc time
#define RTC_TIME_OFFSET 5 // 32 bit register containing rtc time

#define M_TEST_SPEED_OFFSET(x) ((x==0)?M1_TEST_SPEED_OFFSET:M2_TEST_SPEED_OFFSET)
#define M_TEST_ACCEL_OFFSET(x) ((x==0)?M1_TEST_ACCEL_OFFSET:M2_TEST_ACCEL_OFFSET)

typedef struct {
    float speed;
    float finalspeed;
    float accel;
    int seqv;
} motor_t;

void motor_init(motor_t *m)
{
    m->speed = 0.0;
    m->accel = 0.0;
    m->seqv = 0;
}

float accel(float spd, float acl, uint32_t dt)
{
    return (spd + (acl*dt/TICKS_PER_SECOND));
}

// max ticks per one step (~ lowest speed possible .. 2^30)
#define MAX_TICKS 0x20000000

uint32_t spd2ticks(float speed)
{
    float fltTicks = (float)MAX_TICKS;
    if (speed!=0)
        fltTicks = (float)TICKS_PER_SECOND/speed;
    if (fltTicks<0)
        fltTicks=-fltTicks;
    if (fltTicks>(float)MAX_TICKS)
        fltTicks=(float)MAX_TICKS;
    return (uint32_t)fltTicks;
}

// main program
int main(void)
{
    // init clock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    ROM_IntMasterEnable();

    rtc_init();
    init_table();

    // init serial console
    rs485_init();
    //mbrtu_init_table(1); // init data table and address

    // init exteranl switches
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // ios
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    // init onboard leds and btns
    // Enable pin PF0/4 for GPIOInput
    //First open the lock and select the bits we want to modify in the GPIO commit register.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // leds
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x0;

    // free running timer init
    init_timer();

    init_motors_lowlevel();

    motor_set_enable(0,true);
    motor_set_enable(1,true);

    motor_t m[MOTORS];
    motor_init(&m[0]);
    motor_init(&m[1]);

    uint32_t tLast = get_fast_ticks();

    uint32_t rtcss,rtcs;

    // Loop forever.
    while(1)
    {
        // rtc (update and read timer)
        /*rtcs = tab_read(RTC_SET_OFFSET);
        if (rtcs!=0) {
            tab_write(RTC_SET_OFFSET,0);
            rtc_set_time(rtcs);
            rtcss=0;
        } else*/
            rtc_get_time(&rtcs,&rtcss);

        if (rtcs!=tab_read(RTC_TIME_OFFSET))
            tab_write(RTC_TIME_OFFSET,rtcs);

        uint32_t tNow = get_fast_ticks();
        uint32_t tDiff = tNow-tLast;

        int i;
        for (i=0;i<MOTORS;i++)
        {
            switch (m[i].seqv) {
            case 0:
                if (SWn1(i)) {
                    m[i].finalspeed = (float)tab_read_16b(M_TEST_SPEED_OFFSET(i));
                    m[i].accel = (float)tab_read_16b(M_TEST_ACCEL_OFFSET(i));
                    m[i].speed = 10;
                    m[i].seqv++;
                }
                else if (SWn2(i)) {
                    m[i].finalspeed = -(float)tab_read_16b(M_TEST_SPEED_OFFSET(i));
                    m[i].accel = -(float)tab_read_16b(M_TEST_ACCEL_OFFSET(i));
                    m[i].speed = -10;
                    m[i].seqv=4;
                }
                break;
            case 1:
                if (SWn1(i)) {
                    m[i].speed = accel(m[i].speed,m[i].accel,tDiff);
                    if (m[i].speed>= m[i].finalspeed) {
                        m[i].speed = m[i].finalspeed;
                        m[i].seqv++;
                    }
                } else m[i].seqv+=2;
                break;
            case 2:
                if (!(SWn1(i))) m[i].seqv++;
                break;
            case 3:
                if (SWn1(i)) m[i].seqv-=2;
                else {
                    m[i].speed = accel(m[i].speed,-m[i].accel,tDiff);
                    if (m[i].speed<=10) {
                        m[i].speed = 0.0;
                        m[i].seqv=0;
                    }
                }
                break;
            case 4:
                if (SWn2(i)) {
                    m[i].speed = accel(m[i].speed,m[i].accel,tDiff);
                    if (m[i].speed<= m[i].finalspeed) {
                        m[i].speed = m[i].finalspeed;
                        m[i].seqv++;
                    }
                } else m[i].seqv+=2;
                break;
            case 5:
                if (!(SWn2(i))) m[i].seqv++;
                break;
            case 6:
                if (SWn2(i)) m[i].seqv-=2;
                else {
                    m[i].speed = accel(m[i].speed,-m[i].accel,tDiff);
                    if (m[i].speed>=-10) {
                        m[i].speed = 0.0;
                        m[i].seqv=0;
                    }
                }
                break;
            }

            if (m[i].speed>0) motor_set_period(i,true,true,spd2ticks(m[i].speed));
            else if (m[i].speed<0) motor_set_period(i,true,false,spd2ticks(-m[i].speed));
            else motor_set_period(i,false,false,0);
        }

        tLast = tNow;

        // UART (modbus)
        static uint32_t last_trx = 0;

        if ((tNow-last_trx)>30000) mbrtu_rec_reset();

        // modbus rx
        if (RS485CharsAvail()) {
            char c = RS485GetChar();
            last_trx = tNow;
            mbrtu_recv_char(c);
        }

        // modbus tx
        if (mb_tx_char_avail()>0)
            rs485_putch(mb_tx_char_get());

        // save data to eeprom
        /*if (mbData[COMMAND_OFFSET]&EESAVE_COMMAND_BIT) {
            mbrtu_save_eeprom();
            mbData[COMMAND_OFFSET]&=~EESAVE_COMMAND_BIT;
        }*/
    }
}
