#ifndef __mbrtu_serv_c__
#define __mbrtu_serv_c__
#include <stdint.h>
#include "driverlib/eeprom.h"
#include "crc16.c"

/* data prototypes */
#define MBDATA_TABLE_LENGTH 96
uint16_t mbData[MBDATA_TABLE_LENGTH];

#define MBRTU_TX_BUFF_SIZE 64
uint8_t mb_tx_buffer[MBRTU_TX_BUFF_SIZE];

/* function prototypes */
int mb_tx_char_avail(void);
char mb_tx_char_get(void);
void mbrtu_recv_char(char c, uint32_t t);
void mbrtu_init_table(uint8_t address);
/*int mb_tx_char_avail(void);
char mb_tx_char_get(void);*/

int mb_tx_buff_ptr_in = 0;
int mb_tx_buff_ptr_out = 0;

#define MBRTU_RX_BUFF_SIZE 64
#define MBRTU_TIMEOUT 1823 /* 19.2kBaud, 3.5 * 10 bit */

#define MB_UNIT_ADDRESS_DEFAULT 0x00
uint8_t mb_unit_address = MB_UNIT_ADDRESS_DEFAULT;

// modbus function codes
#define MB_READHOLDREG_FCODE 0x03
#define MB_WRITEHOLDREG_FCODE 0x10

//#define LED_MB_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,GPIO_PIN_3);}while(0)
//#define LED_MB_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);}while(0)
#define LED_MB_ON() do{}while(0)
#define LED_MB_OFF() do{}while(0)


#define MBRTU_TX_BUFF_SIZE 64

int mb_tx_char_avail(void)
{
    return (mb_tx_buff_ptr_in-mb_tx_buff_ptr_out);
}

char mb_tx_char_get(void)
{
    return (char)mb_tx_buffer[mb_tx_buff_ptr_out++];
}

/* create exception response into tx buffer */
void mbrtu_exception(uint8_t code, uint8_t* mbrtu_rec_buff)
{
    mb_tx_buff_ptr_in = 0;
    mb_tx_buff_ptr_out = 0;
    uint8_t* p_mbrtu_tx_buff = mb_tx_buffer;
    *p_mbrtu_tx_buff++=mbrtu_rec_buff[0];
    *p_mbrtu_tx_buff++=mbrtu_rec_buff[1]|0x80;
    *p_mbrtu_tx_buff++=code;
    uint16_t crc = CRC16(mb_tx_buffer,3);
    *p_mbrtu_tx_buff++=(crc>>8);
    *p_mbrtu_tx_buff  =(crc&0xFF);
    mb_tx_buff_ptr_out = 0;
    mb_tx_buff_ptr_in = 5;
}

/* mbrtu_read_registers .. read holding registers (create answer) */
void mbrtu_read_registers(uint8_t* mbrtu_rec_buff)
{
    if ((mbrtu_rec_buff[0]==mb_unit_address) &&
        (CRC16((uint8_t *)mbrtu_rec_buff,8)==0)) {
        uint16_t adr = (mbrtu_rec_buff[2]<<8)|mbrtu_rec_buff[3];
        uint16_t len = (mbrtu_rec_buff[4]<<8)|mbrtu_rec_buff[5];
        if ((adr<MBDATA_TABLE_LENGTH)||((adr+len)<MBDATA_TABLE_LENGTH)) {
            mb_tx_buff_ptr_in = 0;
            mb_tx_buff_ptr_out = 0;
            uint8_t* p_mbrtu_tx_buff = mb_tx_buffer;
            *p_mbrtu_tx_buff++=mbrtu_rec_buff[0];
            *p_mbrtu_tx_buff++=mbrtu_rec_buff[1];
            *p_mbrtu_tx_buff++=len*2;
            int i;
            for (i=0;i<len;i++) {
                uint16_t d16b = mbData[adr+i];
                *p_mbrtu_tx_buff++=(d16b>>8);
                *p_mbrtu_tx_buff++=(d16b&0xFF);
            }
            uint16_t crc = CRC16(mb_tx_buffer,3+2*len);
            *p_mbrtu_tx_buff++=(crc>>8);
            *p_mbrtu_tx_buff  =(crc&0xFF);
            mb_tx_buff_ptr_out = 0;
            mb_tx_buff_ptr_in = 5+2*len;
        } else {
            // send exception (incorrect data address)
            mbrtu_exception(2,mbrtu_rec_buff);
        }
    }
}

/* mbrtu_write_registers .. write holding registers (create answer) */
void mbrtu_write_registers(uint8_t* mbrtu_rec_buff,int dlen)
{
    if ((mbrtu_rec_buff[0]==mb_unit_address) &&
        (CRC16((uint8_t *)mbrtu_rec_buff,dlen)==0)) {
        uint16_t adr = (mbrtu_rec_buff[2]<<8)|mbrtu_rec_buff[3];
        uint16_t len = (mbrtu_rec_buff[4]<<8)|mbrtu_rec_buff[5];
        if ((dlen==9+len*2)&&(adr<MBDATA_TABLE_LENGTH)&&((adr+len)<MBDATA_TABLE_LENGTH)) {
            uint8_t* p_mb_buff = &mbrtu_rec_buff[7];
            int i;
            for (i=0;i<len;i++) {
                uint16_t d16b = (*p_mb_buff++)<<8;
                d16b |= *p_mb_buff++;
                mbData[adr++] = d16b;
            }
            mb_tx_buff_ptr_in = 0;
            mb_tx_buff_ptr_out = 0;
            p_mb_buff = mb_tx_buffer;
            for (i=0;i<6;i++) {
                *p_mb_buff++ = mbrtu_rec_buff[i];
            }
            uint16_t crc = CRC16(mb_tx_buffer,6);
            *p_mb_buff++=(crc>>8);
            *p_mb_buff  =(crc&0xFF);
            mb_tx_buff_ptr_out = 0;
            mb_tx_buff_ptr_in = 8;
        } else {
            mbrtu_exception(2,mbrtu_rec_buff);
        }
    }
}

/* mbrtu_recv .. receive one byte
args: char c: byte
      uint32_t t: timeout from last receive in "us" (0 if not used)
*/
void mbrtu_recv_char(char c, uint32_t t)
{
    // receive parameters
    static int mbrtu_rec_status = 0;
    static uint8_t mbrtu_rec_buff[MBRTU_RX_BUFF_SIZE];
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
            if ((mbrtu_rec_buff[0]==mb_unit_address) &&
                (CRC16((uint8_t *)mbrtu_rec_buff,8)==0)) {
                mbrtu_read_registers(mbrtu_rec_buff);
            }
            mbrtu_rec_status=0;
        }
        break;
    case 3: // write holding registers
        mbrtu_rec_buff[mbrtu_rec_ptr++]=c;
        if ((mbrtu_rec_ptr>=9)&&(mbrtu_rec_ptr>=mbrtu_rec_buff[6]+9)) {
            // write holding registers message buffer ready
            uint16_t dlen = mbrtu_rec_buff[6]+9;
            if ((mbrtu_rec_buff[0]==mb_unit_address) &&
                (CRC16((uint8_t *)mbrtu_rec_buff,dlen)==0)) {
                mbrtu_write_registers(mbrtu_rec_buff,dlen);
            }
            mbrtu_rec_status=0;
        }
        break;
    default:
        mbrtu_rec_status = 0;
        break;
    }

    // buffer limit
    if (mbrtu_rec_ptr>=MBRTU_RX_BUFF_SIZE)
        mbrtu_rec_ptr=MBRTU_RX_BUFF_SIZE-1;

    if (mbrtu_rec_status>1) {LED_MB_ON();}
    else {LED_MB_OFF();}
}

/* mbrtu_init .. initialize data table */
void mbrtu_init_table(uint8_t address)
{
    mb_unit_address = address;

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    int eeerr = ROM_EEPROMInit();

    if (eeerr!=0) {
        int i;
        for (i=0;i<MBDATA_TABLE_LENGTH;i++) {
            mbData[i]=0;
        }
    } else {
        ROM_EEPROMRead((uint32_t*)&mbData[0],0,MBDATA_TABLE_LENGTH*2);
        uint16_t mbDataCRC;
        ROM_EEPROMRead((uint32_t*)&mbDataCRC,MBDATA_TABLE_LENGTH*2,4);
        if (mbDataCRC!=CRC16((uint8_t*)&mbData[0],MBDATA_TABLE_LENGTH*2)) {
            int i;
            for (i=0;i<MBDATA_TABLE_LENGTH;i++) {
                mbData[i]=0;
            }
        }
    }
}

void mbrtu_save_eeprom(void)
{
    uint16_t mbDataCRC = CRC16((uint8_t*)&mbData[0],MBDATA_TABLE_LENGTH*2);
    ROM_EEPROMProgram((uint32_t*)&mbData[0],0,MBDATA_TABLE_LENGTH*2);
    ROM_EEPROMProgram((uint32_t*)&mbDataCRC,MBDATA_TABLE_LENGTH*2,4);
}

#endif
