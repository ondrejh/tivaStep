#ifndef __mbrtu_serv_c__
#define __mbrtu_serv_c__
#include <stdint.h>
#include "crc16.c"

/* data prototypes */
#define MBDATA_TABLE_LENGTH 32
uint16_t mbData[MBDATA_TABLE_LENGTH];

#define MB_TX_BUFF_LEN 128
#define MB_TX_BUFF_MASK 0x7F
uint8_t mb_tx_buffer[MB_TX_BUFF_LEN];
/* function prototypes */
int mb_tx_char_avail(void);
char mb_tx_char_get(void);
void mb_tx_char_put(char c);
//void mbrtu_read_registers((uint8_t *)mbrtu_buffer);
void mbrtu_recv(char c, uint32_t t);
void mbrtu_init(void);
/*int mb_tx_char_avail(void);
char mb_tx_char_get(void);*/

int mb_tx_buff_ptr_in = 0;
int mb_tx_buff_ptr_out = 0;

#define MBRTU_REC_BUFF_SIZE 64
#define MBRTU_TIMEOUT 1823 /* 19.2kBaud, 3.5 * 10 bit */

#define MB_UNIT_ADDRESS 0x01

// modbus function codes
#define MB_READHOLDREG_FCODE 0x03
#define MB_WRITEHOLDREG_FCODE 0x16

#define LED_MBRX_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,GPIO_PIN_3);}while(0)
#define LED_MBRX_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);}while(0)
#define LED_MBTX_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1);}while(0)
#define LED_MBTX_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0);}while(0)

#define MBRTU_TX_BUFF_SIZE 64

int mb_tx_char_avail(void)
{
    return ((mb_tx_buff_ptr_in-mb_tx_buff_ptr_out)&MB_TX_BUFF_MASK);
}

char mb_tx_char_get(void)
{
    char c = (char)mb_tx_buffer[mb_tx_buff_ptr_out++];
    mb_tx_buff_ptr_out&=MB_TX_BUFF_MASK;
    return c;
}

void mb_tx_char_put(char c)
{
    mb_tx_buffer[mb_tx_buff_ptr_in++]=(uint8_t)c;
    mb_tx_buff_ptr_in&=MB_TX_BUFF_MASK;
}

/* mbrtu_read_registers .. read holding registers (create answer) */
void mbrtu_read_registers(uint8_t* mbrtu_rec_buff)
{
    if ((mbrtu_rec_buff[0]==MB_UNIT_ADDRESS) &&
        (CRC16((uint8_t *)mbrtu_rec_buff,8)==0)) {
        LED_MBTX_ON();
        uint16_t adr = (mbrtu_rec_buff[2]<<8)|mbrtu_rec_buff[3];
        uint16_t len = (mbrtu_rec_buff[4]<<8)|mbrtu_rec_buff[5];
        if ((adr<MBRTU_REC_BUFF_SIZE)||((adr+len)<MBRTU_REC_BUFF_SIZE)) {
            uint8_t mbrtu_tx_buff[MBRTU_TX_BUFF_SIZE];
            uint8_t* p_mbrtu_tx_buff = mbrtu_tx_buff;
            *p_mbrtu_tx_buff++=mbrtu_rec_buff[0];
            *p_mbrtu_tx_buff++=mbrtu_rec_buff[1];
            *p_mbrtu_tx_buff++=len*2;
            int i;
            for (i=0;i<len;i++) {
                uint16_t d16b = mbData[adr+i];
                *p_mbrtu_tx_buff++=(d16b>>8);
                *p_mbrtu_tx_buff++=(d16b&0xFF);
            }
            uint16_t crc = CRC16(mbrtu_tx_buff,3+2*len);
            //*p_mbrtu_tx_buff++=(crc&0xFF);
            //*p_mbrtu_tx_buff  =(crc>>8);
            *p_mbrtu_tx_buff++=(crc>>8);
            *p_mbrtu_tx_buff  =(crc&0xFF);
            p_mbrtu_tx_buff = mbrtu_tx_buff;
            for (i=0;i<(5+2*len);i++)
                mb_tx_char_put(*p_mbrtu_tx_buff++);
        } else {
            // send exception (incorrect data address)
        }
    }
}

/* mbrtu_recv .. receive one byte
args: char c: byte
      uint32_t t: timeout from last receive in "us" (0 if not used)
*/
void mbrtu_recv(char c, uint32_t t)
{
    // receive parameters
    static int mbrtu_rec_status = 0;
    static uint8_t mbrtu_rec_buff[MBRTU_REC_BUFF_SIZE];
    static int mbrtu_rec_ptr = 0;

    if (t>MBRTU_TIMEOUT)
        mbrtu_rec_status = 0;

    switch (mbrtu_rec_status) {
    case 0: // start receiving (receive address)
        mbrtu_rec_ptr = 0;
        mbrtu_rec_buff[mbrtu_rec_ptr++]=c;
        mbrtu_rec_status++;
        break;
    case 1: // receive function code
        mbrtu_rec_buff[mbrtu_rec_ptr++]=c;
        if (c==MB_READHOLDREG_FCODE)
            mbrtu_rec_status=2;
        if (c==MB_WRITEHOLDREG_FCODE)
            mbrtu_rec_status=3;
        break;
    case 2: // read holding registers
        mbrtu_rec_buff[mbrtu_rec_ptr++]=c;
        if (mbrtu_rec_ptr>=8) {
            // read holding registers message buffer ready
            if ((mbrtu_rec_buff[0]==MB_UNIT_ADDRESS) &&
                (CRC16((uint8_t *)mbrtu_rec_buff,8)==0)) {
                //LED_MBTX_ON();
                mbrtu_read_registers(mbrtu_rec_buff);
            }
            //
            //   !!! PLACE SOME CODE HERE !!!
            //
            mbrtu_rec_status=0;
        }
        break;
    case 3: // write holding registers
        mbrtu_rec_buff[mbrtu_rec_ptr++]=c;
        if (mbrtu_rec_ptr>=mbrtu_rec_buff[6]+9) {
            // write holding registers message buffer ready
            if ((mbrtu_rec_buff[0]==MB_UNIT_ADDRESS) &&
                (CRC16((uint8_t *)mbrtu_rec_buff,8)==0)) {
                LED_MBTX_ON();
            }
            //
            //   !!! PLACE SOME CODE HERE !!!
            //
            mbrtu_rec_status=0;
        }
        break;
    default:
        mbrtu_rec_status = 0;
        break;
    }

    // buffer limit
    if (mbrtu_rec_ptr>=MBRTU_REC_BUFF_SIZE)
        mbrtu_rec_ptr=MBRTU_REC_BUFF_SIZE-1;

    if (mbrtu_rec_status>1) {LED_MBRX_ON();}
    else {LED_MBRX_OFF();}
}

/* mbrtu_init .. initialize data table */
void mbrtu_init(void)
{
    int i;
    for (i=0;i<MBDATA_TABLE_LENGTH;i++) {
        mbData[i]=0;
    }
}

#endif
