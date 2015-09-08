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
#include "mbrtu_serv.c"
#include "motor.h"

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
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinConfigure(GPIO_PA0_U0RX);
  GPIOPinConfigure(GPIO_PA1_U0TX);

  // enable UART0
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 19200, UART_CONFIG_WLEN_8|UART_CONFIG_PAR_NONE|UART_CONFIG_STOP_TWO);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  // uart1
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  GPIOPinConfigure(GPIO_PB0_U1RX);
  GPIOPinConfigure(GPIO_PB1_U1TX);

  // enable UART1
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
  UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 19200, UART_CONFIG_WLEN_8|UART_CONFIG_PAR_NONE|UART_CONFIG_STOP_TWO);
  GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

/*int h2i(char c)
{
    if ((c>='0') && (c<='9'))
        return (c-'0');
    if ((c>='A') && (c<='F'))
        return (c-'A'+10);
    if ((c>='a') && (c<='f'))
        return (c-'a'+10);
    return -1;
}*/

// max ticks per one step (~ lowest speed possible .. 2^30)
//#define MAX_TICKS 0x20000000

/*typedef struct {
    float speed;
    uint32_t step_ticks; // number of ticks per one step
    uint32_t step_timer; // timer counter to calculate steps
    uint32_t position;
    int run; // 0 stop, 1 forward, -1 backward

    // help variables - calculate position and speed
    uint32_t u32A,u32B;
    float spdA,spdB;

    // program values
    int prog;
    uint8_t ptype;
    uint16_t pval1;
    int16_t pval2;

    // function pointer placeholders
    void (*do_step)();
    void (*set_enable)(_Bool);
    void (*set_dir)(_Bool);
} motor_t;*/

/*uint32_t spd2ticks(float speed)
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
    uint32_t timAN = timN-timA;

    return (spdA + (spdAB*timAN/timAB));
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
    m->position = 0;
    m->run = 0;

    m->prog = 0;
    m->ptype = 0;
    m->pval1 = 0;
    m->pval2 = 0;

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

    m->position+=m->run;
    m->do_step();
}*/

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


// main program
int main(void)
{
    // enable all used peripherals
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // timer0
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0); // eeprom

    // init clock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // init serial console
    debugConsoleInit();
    mbrtu_init_table(1); // init data table and address

    // init exteranl switches
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // ios
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    /*// init unit leds
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) =  GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_7|GPIO_PIN_6);
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_7|GPIO_PIN_6, GPIO_PIN_7|GPIO_PIN_6);
    //HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0x0;*/

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

    ROM_IntMasterEnable();

    // free running timer init
    init_timer();

    init_motors_lowlevel();

    motor_set_enable(0,true);
    motor_set_enable(1,true);

    uint32_t tLast = get_fast_ticks();


    // Loop forever.
    while(1)
    {
        uint32_t tNow = get_fast_ticks();
        uint32_t tDiff = tNow-tLast;

        static int m2seqv = 0;
        static int m1seqv = 0;

        switch (m1seqv) {
        case 0: // wait button
            if (SW1) {
                motor_set_period(0,true,true,50000);
                m1seqv++;
            } else if (SW2) {
                motor_set_period(0,true,false,50000);
                m1seqv+=2;
            }
            break;
        case 1:
            if (!SW1) {
                motor_set_period(0,false,true,0);
                m1seqv--;
            }
            break;
        case 2:
            if (!SW2) {
                motor_set_period(0,false,false,0);
                m1seqv-=2;
            }
            break;
        }

        switch (m2seqv) {
        case 0: // wait button
            if (SW3) {
                motor_set_period(1,true,true,50000);
                m2seqv++;
            } else if (SW4) {
                motor_set_period(1,true,false,50000);
                m2seqv+=2;
            }
            break;
        case 1:
            if (!SW3) {
                motor_set_period(1,false,true,0);
                m2seqv--;
            }
            break;
        case 2:
            if (!SW4) {
                motor_set_period(1,false,false,0);
                m2seqv-=2;
            }
            break;
        }

        tLast = tNow;

        // UART (modbus)
        static int modbus_select = -1;
        static uint32_t last_trx = 0;

        //  uart 0 modbus rx
        if (UARTCharsAvail(UART0_BASE)) {
            uint32_t trx_now = get_fast_ticks();
            char c = UARTCharGet(UART0_BASE);
            if (modbus_select!=0)
                mbrtu_recv_char(c,MBRTU_TIMEOUT+1);
            else
                mbrtu_recv_char(c,(trx_now-last_trx)>>4);
            last_trx = trx_now;
            modbus_select = 0;
        }
        // uart 1 modbus rx
        if (UARTCharsAvail(UART1_BASE)) {
            uint32_t trx_now = get_fast_ticks();
            char c = UARTCharGet(UART1_BASE);
            if (modbus_select!=1)
                mbrtu_recv_char(c,MBRTU_TIMEOUT+1);
            else
                mbrtu_recv_char(c,(trx_now-last_trx)>>4);
            last_trx = trx_now;
            modbus_select = 1;
        }

        // modbus tx
        if (mb_tx_char_avail()>0) {
            switch (modbus_select) {
            case 0:
                UARTCharPut(UART0_BASE,mb_tx_char_get());
                break;
            case 1:
                UARTCharPut(UART1_BASE,mb_tx_char_get());
                break;
            default:
                break;
            }
        }

        // save data to eeprom
        if (mbData[COMMAND_OFFSET]&EESAVE_COMMAND_BIT) {
            mbrtu_save_eeprom();
            mbData[COMMAND_OFFSET]&=~EESAVE_COMMAND_BIT;
        }
    }
}
