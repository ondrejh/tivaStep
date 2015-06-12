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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/can.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"

#include "timer.c"

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

// motor A ios
#define MA_ENA_H() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,GPIO_PIN_4);}while(0)
#define MA_ENA_L() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,0);}while(0)
#define MA_PUL_H() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_PIN_2);}while(0)
#define MA_PUL_L() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,0);}while(0)
#define MA_DIR_H() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,GPIO_PIN_3);}while(0)
#define MA_DIR_L() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,0);}while(0)
#define MA_FLT (GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_6)!=0)

// motor B ios
#define MB_ENA_H() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,GPIO_PIN_5);}while(0)
#define MB_ENA_L() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,0);}while(0)
#define MB_PUL_H() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,GPIO_PIN_7);}while(0)
#define MB_PUL_L() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);}while(0)
#define MB_DIR_H() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_PIN_6);}while(0)
#define MB_DIR_L() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0);}while(0)
#define MB_FLT (GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_4)!=0)

// onboard leds
#define LED_RED_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1);}while(0)
#define LED_RED_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0);}while(0)
#define LED_BLUE_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,GPIO_PIN_2);}while(0)
#define LED_BLUE_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0);}while(0)
#define LED_GREEN_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,GPIO_PIN_3);}while(0)
#define LED_GREEN_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);}while(0)

// onboard buttons
#define BTN1 (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0)
#define BTN2 (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0)

// external switches
#define SW1 (GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_0)==0)
#define SW2 (GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1)==0)
#define SW3 (GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_2)==0)
#define SW4 (GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_3)==0)

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

// debug console init
void debugConsoleInit(void)
{
  // enable GPIO port A which is used for UART0
  //SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);

  // enable UART0
  //SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC); // use internal 16MHz osc.
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioConfig(0, 115200, 16000000);

  // uart1
  GPIOPinConfigure(GPIO_PB0_U1RX);
  GPIOPinConfigure(GPIO_PB1_U1TX);
  UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
  GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTStdioConfig(0, 115200, 16000000);
}

int h2i(char c)
{
    if ((c>='0') && (c<='9'))
        return (c-'0');
    if ((c>='A') && (c<='F'))
        return (c-'A'+10);
    if ((c>='a') && (c<='f'))
        return (c-'a'+10);
    return -1;
}

// max ticks per one step (~ lowest speed possible .. 2^30)
#define MAX_TICKS 0x20000000

typedef struct {
    float speed;
    uint32_t step_ticks; // number of ticks per one step
    uint32_t step_timer; // timer counter to calculate steps
    int run; // 0 stop, 1 forward, -1 backward

    void (*do_step)();
    void (*set_enable)(_Bool);
    void (*set_dir)(_Bool);
} motor_t;

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

float ramp(float spdA, float spdB, uint32_t timA, uint32_t timB, uint32_t timN)
{
    float spdAB = spdB-spdA;
    uint32_t timAB = timB-timA;

    return spdA + spdAB*((float)timN/(float)timAB);
}

float accel(float spd, float acl, uint32_t dt)
{
    return (spd + (acl*dt/TICKS_PER_SECOND));
}

void mA_do_step(void) {MA_PUL_H();wait_micros(5);MA_PUL_L();}
void mA_set_dir(bool dir) {if (dir) {MA_DIR_H();} else {MA_DIR_L();}wait_micros(5);}
void mA_set_ena(bool ena) {if (ena) {MA_ENA_L();} else {MA_ENA_H();}}

void mB_do_step(void) {MB_PUL_H();wait_micros(5);MB_PUL_L();}
void mB_set_dir(bool dir) {if (dir) {MB_DIR_H();} else {MB_DIR_L();}wait_micros(5);}
void mB_set_ena(bool ena) {if (ena) {MB_ENA_L();} else {MB_ENA_H();}}

void motor_init(motor_t *m, void (*do_step)(), void (*set_enable)(_Bool), void (*set_direction)(_Bool))
{
    // init variables
    //m.enable = false;
    m->speed = 0; // Hz
    m->step_timer = 0;
    m->run = 0;

    // init output functions
    m->do_step = do_step;
    m->set_enable = set_enable;
    m->set_dir = set_direction;
}

void motor_step(motor_t *m, bool dir)
{
    if ((m->run==0) || ((m->run==1)&&(dir==false)) || ((m->run==-1)&&(dir==true)))
    {
        m->run=dir?1:-1;
        m->set_dir(dir);
    }

    m->do_step();
}

#define TEST_SPEED 2000.0
#define TEST_ACCEL 5000.0
#define TEST_MIN_SPEED 10.0

// main program
int main(void)
{
    // enable all used peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // uart0, ios
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // ios
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); // ios
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // ios
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // leds
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); // uart0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1); // uart1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // timer0

    // init clock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // init serial console
    debugConsoleInit();

    // init motor ios
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4); // motor A PUL,DIR,ENA
    GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, 0x00); // PUL,DIR,ENA <- 'L'
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_6); // motor A FLT (input)
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7); // motor B ENA,DIR,PUL
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00); // PUL,DIR,ENA <- 'L'
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,GPIO_PIN_4); // motor B FLT (input)

    // init exteranl switches
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    // init leds
    // Enable pin PF0/4 for GPIOInput
    //First open the lock and select the bits we want to modify in the GPIO commit register.
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x0;

    // free running timer init
    init_timer();

    // pulse ENA (to clear FLT)
    MA_ENA_H();
    MB_ENA_H();
    wait_micros(100);
    MA_ENA_L();
    MB_ENA_L();
    wait_micros(100);

    motor_t m1,m2;
    motor_init(&m1, (mA_do_step), (mA_set_ena), (mA_set_dir));
    motor_init(&m2, (mB_do_step), (mB_set_ena), (mB_set_dir));

    uint32_t tLast = 0;

    // Loop forever.
    while(1)
    {
        uint32_t tNow = get_fast_ticks();
        uint32_t tDiff = tNow-tLast;

        if (MA_FLT||MB_FLT) {LED_RED_ON();} else {LED_RED_OFF();}

        static int m1seqv = 0;
        switch (m1seqv) {
        case 0: // wait button
            if (BTN1||SW1) {
                m1.speed = -10;
                m1.run = 0;
                m1seqv++;
            }
            else if (BTN2||SW2) {
                m1.speed = 10;
                m1.run = 0;
                m1seqv=4;
            }
            break;
        case 1: // accelerate backward
            if (BTN1||SW1) {
                m1.speed = accel(m1.speed,-TEST_ACCEL,tDiff);
                if (m1.speed<=-TEST_SPEED) {
                    m1.speed =-TEST_SPEED;
                    m1seqv++;
                }
            } else m1seqv+=2;
            break;
        case 2: // go backward (full speed)
            if (!(BTN1||SW1)) m1seqv++;
            break;
        case 3:
            if (BTN1||SW1) m1seqv-=2;
            else {
                m1.speed = accel(m1.speed,TEST_ACCEL,tDiff);
                if (m1.speed>=-TEST_MIN_SPEED) {
                    m1.speed = 0.0;
                    m1seqv = 0;
                }
            }
            break;
        case 4: // accelerate forward
            if (BTN2||SW2) {
                m1.speed = accel(m1.speed,TEST_ACCEL,tDiff);
                if (m1.speed>=TEST_SPEED) {
                    m1.speed =TEST_SPEED;
                    m1seqv++;
                }
            } else m1seqv+=2;
            break;
        case 5: // go forward (full speed)
            if (!(BTN2||SW2)) m1seqv++;
            break;
        case 6: // decelerate
            if (BTN2||SW2) m1seqv-=2;
            else {
                m1.speed = accel(m1.speed,-TEST_ACCEL,tDiff);
                if (m1.speed<=TEST_MIN_SPEED) {
                    m1.speed = 0.0;
                    m1seqv = 0;
                }
            }
            break;
        default:
            m1seqv = 0;
            break;
        }

        static int m2seqv = 0;
        switch (m2seqv) {
        case 0: // wait button
            if (SW3) {
                m2.speed = -10;
                m2.run = 0;
                m2seqv++;
            }
            else if (SW4) {
                m2.speed = 10;
                m2.run = 0;
                m2seqv=4;
            }
            break;
        case 1: // accelerate backward
            if (SW3) {
                m2.speed = accel(m2.speed,-TEST_ACCEL,tDiff);
                if (m2.speed<=-TEST_SPEED) {
                    m2.speed =-TEST_SPEED;
                    m2seqv++;
                }
            } else m2seqv+=2;
            break;
        case 2: // go backward (full speed)
            if (!(SW3)) m2seqv++;
            break;
        case 3:
            if (SW3) m2seqv-=2;
            else {
                m2.speed = accel(m2.speed,TEST_ACCEL,tDiff);
                if (m2.speed>=-TEST_MIN_SPEED) {
                    m2.speed = 0.0;
                    m2seqv = 0;
                }
            }
            break;
        case 4: // accelerate forward
            if (SW4) {
                m2.speed = accel(m2.speed,TEST_ACCEL,tDiff);
                if (m2.speed>=TEST_SPEED) {
                    m2.speed =TEST_SPEED;
                    m2seqv++;
                }
            } else m2seqv+=2;
            break;
        case 5: // go forward (full speed)
            if (!(SW4)) m2seqv++;
            break;
        case 6: // decelerate
            if (SW4) m2seqv-=2;
            else {
                m2.speed = accel(m2.speed,-TEST_ACCEL,tDiff);
                if (m2.speed<=TEST_MIN_SPEED) {
                    m2.speed = 0.0;
                    m2seqv = 0;
                }
            }
            break;
        default:
            m2seqv = 0;
            break;
        }

        if (m1.speed!=0) {
            m1.step_ticks=spd2ticks(m1.speed);
            m1.step_timer+=tDiff;
            if (m1.step_timer>=m1.step_ticks) {
                motor_step(&m1,(m1.speed>0));
                m1.step_timer-=m1.step_ticks;
            }
        } else {
            m1.step_timer=0;
            m1.run=0;
        }

        if (m2.speed!=0) {
            m2.step_ticks=spd2ticks(m2.speed);
            m2.step_timer+=tDiff;
            if (m2.step_timer>=m2.step_ticks) {
                motor_step(&m2,(m2.speed>0));
                m2.step_timer-=m2.step_ticks;
            }
        } else {
            m2.step_timer=0;
            m2.run=0;
        }

        if (m1.run) {LED_BLUE_ON();} else {LED_BLUE_OFF();}
        if (m2.run) {LED_GREEN_ON();} else {LED_GREEN_OFF();}

        tLast = tNow;

        // debug uart
        if (UARTCharsAvail(UART0_BASE)) {
            char c = UARTCharGet(UART0_BASE);
            UARTCharPut(UART0_BASE,c);
        }

        if (UARTCharsAvail(UART1_BASE)) {
            char c = UARTCharGet(UART1_BASE);
            UARTCharPut(UART1_BASE,c);
        }
        /*if (UARTCharsAvail(UART0_BASE)) {
            uint32_t err = UARTRxErrorGet(UART0_BASE);
            char c = UARTCharGet(UART0_BASE);
            if (err!=0)
                UARTRxErrorClear(UART0_BASE);
            else {
                // char received without errors
                if (c=='?') {
                    char buf[64];
                    sprintf(buf,"%.1f %.1f\n",m1.speed,TEST_ACCEL);
                    //UARTwrite(buf,strlen(buf));
                }
                UARTCharPutNonBlocking(UART0_BASE,c); // echo
            }
        }*/
    }
}
