#include "includes.h"

#include "crc16.c"
#include "params.h"
#include "mbrtu_serv.h"

uint8_t mb_tx_buffer[MBRTU_TX_BUFF_SIZE];

volatile int mb_tx_buff_ptr_in = 0;
volatile int mb_tx_buff_ptr_out = 0;

uint8_t mbrtu_rec_buff[MBRTU_RX_BUFF_SIZE];

int mbrtu_rec_ptr = 0;

uint8_t mb_unit_address = MBRTU_ADDRESS_DEFAULT;

// modbus function codes
#define MB_READHOLDREG_FCODE 0x03
#define MB_WRITEHOLDREG_FCODE 0x10

#define LED_MB_ON() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,GPIO_PIN_3);}while(0)
#define LED_MB_OFF() do{GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);}while(0)
//#define LED_MB_ON() do{}while(0)
//#define LED_MB_OFF() do{}while(0)

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
void mbrtu_read_registers(void)
{
    uint16_t adr = (mbrtu_rec_buff[2]<<8)|mbrtu_rec_buff[3];
    uint16_t len = (mbrtu_rec_buff[4]<<8)|mbrtu_rec_buff[5];

    int i;
    int ptr = 2;
    uint16_t d16b;

    mb_tx_buff_ptr_in = 0;
    mb_tx_buff_ptr_out = 0;

    memcpy(mb_tx_buffer,mbrtu_rec_buff,ptr);

    mb_tx_buffer[ptr++] = len*2;
    ROM_IntMasterDisable();
    for (i=0;i<len;i++) {
        d16b = tab_read_16b(adr+i);
        mb_tx_buffer[ptr++] = (d16b>>8);
        mb_tx_buffer[ptr++] = (d16b&0xFF);
    }
    ROM_IntMasterEnable();
    d16b = CRC16(mb_tx_buffer,ptr);
    mb_tx_buffer[ptr++] = (d16b>>8);
    mb_tx_buffer[ptr++] = (d16b&0xFF);

    mb_tx_buff_ptr_out = 0;
    mb_tx_buff_ptr_in = ptr;
}

/* mbrtu_write_registers .. write holding registers (create answer) */
void mbrtu_write_registers(void)
{
    // write registers
    uint16_t adr = (mbrtu_rec_buff[2]<<8)|mbrtu_rec_buff[3];
    uint16_t len = (mbrtu_rec_buff[4]<<8)|mbrtu_rec_buff[5];

    int i;
    int ptr = 7;
    uint16_t d16b;

    for (i=0;i<len;i++) {
        d16b = mbrtu_rec_buff[ptr++]<<8;
        d16b |= mbrtu_rec_buff[ptr++];
        tab_write_16b(adr+i,d16b);
    }

    // create answer
    mb_tx_buff_ptr_in = 0;
    mb_tx_buff_ptr_out = 0;
    memcpy(mb_tx_buffer,mbrtu_rec_buff,6);

    uint16_t crc = CRC16(mb_tx_buffer,6);
    mb_tx_buffer[6]=(crc>>8);
    mb_tx_buffer[7]=(crc&0xFF);
    mb_tx_buff_ptr_out = 0;
    mb_tx_buff_ptr_in = 8;
}

/* mbrtu_recv .. receive one byte
args: char c: byte
*/
void mbrtu_recv_char(char c)
{
    mbrtu_rec_buff[mbrtu_rec_ptr++]=c;

    // buffer limit
    if (mbrtu_rec_ptr>=MBRTU_RX_BUFF_SIZE)
        mbrtu_rec_ptr=MBRTU_RX_BUFF_SIZE-1;

    LED_MB_ON();
}

// use rx buffer content
void mbrtu_use_rx_buffer(void)
{
    LED_MB_OFF();

    // test if enough data in buffer
    if (mbrtu_rec_ptr<3)
        return;

    // test address
    if (mbrtu_rec_buff[0]!=mb_unit_address)
        return;

    // test crc
    if (CRC16((uint8_t *)mbrtu_rec_buff,mbrtu_rec_ptr)!=0)
        return;

    // switch function code
    switch (mbrtu_rec_buff[1]) {
    case MB_READHOLDREG_FCODE:
        mbrtu_read_registers();
        break;
    case MB_WRITEHOLDREG_FCODE:
        mbrtu_write_registers();
        break;
    default:
        return;
        break;
    }
}

// reset receiver, if something received try to use it
void mbrtu_rec_reset(void)
{
    if (mbrtu_rec_ptr>0)
        mbrtu_use_rx_buffer();
    mbrtu_rec_ptr = 0;
}

// set unit address
void mbrtu_set_unit_address(uint8_t address)
{
    mb_unit_address = address;
}

/* mbrtu_init .. initialize data table */
/*void mbrtu_init_table(uint8_t address)
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
}*/

/*void mbrtu_save_eeprom(void)
{
    uint16_t mbDataCRC = CRC16((uint8_t*)&mbData[0],MBDATA_TABLE_LENGTH*2);
    ROM_EEPROMProgram((uint32_t*)&mbData[0],0,MBDATA_TABLE_LENGTH*2);
    ROM_EEPROMProgram((uint32_t*)&mbDataCRC,MBDATA_TABLE_LENGTH*2,4);
}*/
