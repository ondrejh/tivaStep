#include "includes.h"

#include "rs485.h"

void rs485_init(void)
{
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    ROM_GPIOPinConfigure(GPIO_PB0_U1RX);
    ROM_GPIOPinConfigure(GPIO_PB1_U1TX);
    ROM_GPIOPinTypeGPIOOutput(RS485_TXEN_PORT, RS485_TXEN_PIN);
    ROM_GPIOPinWrite(RS485_TXEN_PORT, RS485_TXEN_PIN, RS485_TXEN_PIN);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1); // uart1
    ROM_UARTTxIntModeSet(UART1_BASE, UART_TXINT_MODE_EOT);
    ROM_UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 19200, UART_CONFIG_WLEN_8|UART_CONFIG_PAR_NONE|UART_CONFIG_STOP_TWO);

    ROM_IntEnable(INT_UART1);
    ROM_UARTIntEnable(UART1_BASE, UART_INT_TX);

    ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}

void rs485_putch(unsigned char c)
{
    // set txen signal prior to put char
    ROM_GPIOPinWrite(RS485_TXEN_PORT, RS485_TXEN_PIN, 0);
    ROM_UARTCharPut(UART1_BASE,c);
}

void rs485_eot_interrupt_handler(void)
{
    // get interrupt flag
    uint32_t ui32Status;
    ui32Status = ROM_UARTIntStatus(UART1_BASE, true);

    if (ui32Status&UART_INT_TX) ROM_GPIOPinWrite(RS485_TXEN_PORT, RS485_TXEN_PIN, RS485_TXEN_PIN);

    ROM_UARTIntClear(UART1_BASE, ui32Status);
}
