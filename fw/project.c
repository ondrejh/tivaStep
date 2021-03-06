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
#include "params.h"
#include "motor.h"
#include "rs485.h"
#include "rtc.h"

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

//#define TEST_SPEED 2000.0
//#define TEST_ACCEL 5000.0
#define TEST_MIN_SPEED 10.0

#define M_TEST_SPEED_OFFSET(x) ((x==0)?M1_TEST_SPEED_OFFSET:M2_TEST_SPEED_OFFSET)
#define M_TEST_ACCEL_OFFSET(x) ((x==0)?M1_TEST_ACCEL_OFFSET:M2_TEST_ACCEL_OFFSET)

#define M_POSITION_OFFSET(x) ((x==0)?M1_POSITION_OFFSET:M2_POSITION_OFFSET)
#define M_GOTOPOS_OFFSET(x) ((x==0)?M1_GOTOPOS_OFFSET:M2_GOTOPOS_OFFSET)

#define M_TIME_ZERO(x) ((x==0)?M1_TIME_ZERO:M2_TIME_ZERO)
#define M_STEPS_ZERO(x) ((x==0)?M1_STEPS_ZERO:M2_STEPS_ZERO)
#define M_TURN_STEPS(x) ((x==0)?M1_TURN_STEPS:M2_TURN_STEPS)
#define M_TURN_TIME(x) ((x==0)?M1_TURN_TIME:M2_TURN_TIME)
#define M_DESIRED_TIME(x) ((x==0)?M1_DESIRED_TIME:M2_DESIRED_TIME)
#define M_DESIRED_POS(x) ((x==0)?M1_DESIRED_POS:M2_DESIRED_POS)

typedef struct {
    float speed;
    float accel;
    uint32_t position;
    uint32_t gotopos;
} motor_t;

void motor_init(motor_t *m)
{
    m->speed = 0.0;
    m->accel = 0.0;
    m->position = 0;
    m->gotopos = 0;
}

float accel(float spd, float acl, uint32_t dt)
{
    return (spd + (acl*dt/TICKS_PER_SECOND));
}

float accel_limit(float spd, float spdlim, float acl, uint32_t dt)
{
    float ret = spd + (acl*dt/TICKS_PER_SECOND);
    if (ret>spdlim)
        ret = spdlim;
    if (ret<-spdlim)
        ret = -spdlim;
    return ret;
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

int32_t steps_to_stop(float speed, float accel)
{
    float st2st = speed*speed/accel/2.0;
    return (int32_t)st2st;
}

float absf(float f)
{
    if (f<0)
        return -f;
    return f;
}

int sigf(float f)
{
    if (f<0)
        return -1;
    else if (f>0)
        return 1;
    else
        return 0;
}

int32_t get_desired_pos(int32_t t_sec, int32_t t_subs, int32_t t_zero, int32_t t_turn, int32_t s_turn, int32_t s_zero)
{
    if (t_turn!=0) {
        double t = (double)((double)(t_sec-t_zero)+(double)t_subs/(double)RTC_SUBSEC_ONESEC);
        double s = t/(double)t_turn*(double)s_turn;
        return (int32_t)s+s_zero;
    }
    return s_zero;
}

// main program
int main(void)
{
    // init clock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); // 16MHz
    //SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ); // 40MHz

    ROM_IntMasterEnable();

    rtc_init();
    init_table();

    // init serial console
    rs485_init();
    init_table();
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

    uint32_t rtcs,rtcss;

    // Loop forever.
    while(1)
    {
        // rtc (update and read timer)
        if (tab_read(RTC_SET_OFFSET)!=0) {
            rtc_set_time(tab_read(RTC_SET_OFFSET));
            tab_write(RTC_SET_OFFSET,0);
        }

        rtc_get_time(&rtcs,&rtcss);
        if (tab_read(RTC_TIME_OFFSET)!=rtcs)
            tab_write(RTC_TIME_OFFSET,rtcs);

        uint32_t tNow = get_fast_ticks();
        uint32_t tDiff = tNow-tLast;

        int i;
        for (i=0;i<MOTORS;i++)
        {
            m[i].position = motor_get_position(i);
            tab_write(M_POSITION_OFFSET(i),(uint32_t)m[i].position);

            m[i].gotopos = get_desired_pos(rtcs,rtcss,
                                           tab_read(M_TIME_ZERO(i)),
                                           tab_read(M_TURN_TIME(i)),
                                           tab_read(M_TURN_STEPS(i)),
                                           tab_read(M_STEPS_ZERO(i)));

            tab_write(M_GOTOPOS_OFFSET(i),m[i].gotopos);

            int32_t epos = (int32_t)m[i].gotopos-(int32_t)m[i].position;
            if (epos>0) {
                m[i].accel = (float)tab_read_16b(M_TEST_ACCEL_OFFSET(i));
                if (steps_to_stop(m[i].speed,m[i].accel)>epos)
                    m[i].accel = -m[i].accel;
            } else if (epos<0) {
                m[i].accel = -(float)tab_read_16b(M_TEST_ACCEL_OFFSET(i));
                if (steps_to_stop(m[i].speed,m[i].accel)<epos)
                    m[i].accel = -m[i].accel;
            } else {
                if (absf(m[i].speed) < (float)TEST_MIN_SPEED) {
                    m[i].accel = 0.0;
                    m[i].speed = 0.0;
                } else
                    m[i].accel = -(float)(tab_read_16b(M_TEST_ACCEL_OFFSET(i))*sigf(m[i].speed));
            }

            m[i].speed = accel_limit(m[i].speed,(float)tab_read_16b(M_TEST_SPEED_OFFSET(i)),m[i].accel,tDiff);

            if (m[i].speed>0) motor_set_period(i,true,true,spd2ticks(m[i].speed));
            else if (m[i].speed<0) motor_set_period(i,true,false,spd2ticks(-m[i].speed));
            else motor_set_period(i,false,false,0);
        }

        tLast = tNow;

        // UART (modbus)
        static uint32_t last_trx = 0;
        if ((tNow-last_trx)>30000) mbrtu_rec_reset();

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
    }
}
