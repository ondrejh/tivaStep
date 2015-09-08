#ifndef __RS485_H__

#define RS485_TXEN_PORT GPIO_PORTB_BASE
#define RS485_TXEN_PIN GPIO_PIN_5

#define RS485CharsAvail() UARTCharsAvail(UART1_BASE)
#define RS485GetChar() UARTCharGet(UART1_BASE)

void rs485_init(void);
void rs485_putch(unsigned char c);
void rs485_eot_interrupt_handler(void);

#endif // __RS485_H__
