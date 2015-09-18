#ifndef __MBRTU_SERV_H__
#define __MBRTU_SERV_H__

#define MBRTU_TX_BUFF_SIZE 64
#define MBRTU_RX_BUFF_SIZE 64

#define MBRTU_TIMEOUT 1823 /* 19.2kBaud, 3.5 * 10 bit */
#define MBRTU_ADDRESS_DEFAULT 0x01

int mb_tx_char_avail(void);
char mb_tx_char_get(void);

void mbrtu_rec_reset(void);
void mbrtu_recv_char(char c);

void mbrtu_set_unit_address(uint8_t address);

#endif

