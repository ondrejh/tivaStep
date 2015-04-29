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

// motor ios
#define MOTOR_ENA_H() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,GPIO_PIN_4);}while(0)
#define MOTOR_ENA_L() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,0);}while(0)
#define MOTOR_PUL_H() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_PIN_2);}while(0)
#define MOTOR_PUL_L() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,0);}while(0)
#define MOTOR_DIR_H() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,GPIO_PIN_3);}while(0)
#define MOTOR_DIR_L() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,0);}while(0)
#define MOTOR_FLT (GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_6)!=0)

// onboard leds
#define LED_RED_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1);}while(0)
#define LED_RED_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0);}while(0)
#define LED_BLUE_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,GPIO_PIN_2);}while(0)
#define LED_BLUE_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2,0);}while(0)
#define LED_GREEN_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,GPIO_PIN_3);}while(0)
#define LED_GREEN_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);}while(0)

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

// main program
int main(void)
{
    // enable all used peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // uart0, ios
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // ios
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // leds
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); // uart0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // timer0

    // init clock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // init serial console
    debugConsoleInit();

    // init motor ios
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, 0x00);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_6);

    // init leds
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);

    init_timer();

    // pulse ENA (to clear FLT)
    MOTOR_ENA_H();
    wait_micros(100);
    MOTOR_ENA_L();
    wait_micros(100);

    // Loop forever.
    while(1)
    {
        uint32_t tNow = get_fast_ticks();

        if (UARTCharsAvail(UART0_BASE)) {
            char c = UARTCharGet(UART0_BASE);
            UARTCharPut(UART0_BASE,c);
        }

        if (MOTOR_FLT) LED_RED_ON(); else LED_RED_OFF();

        if (tNow&0x1000) MOTOR_PUL_H(); else MOTOR_PUL_L();
    }
}
