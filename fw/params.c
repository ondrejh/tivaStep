#include "includes.h"
#include "params.h"

#define PARAMS_TABLE_LENGTH 64

uint32_t table[PARAMS_TABLE_LENGTH];

#define TEST_SPEED_DEFAULT 2000
#define TEST_ACCEL_DEFAULT 5000

#define M1_TEST_SPEED_OFFSET 4
#define M1_TEST_ACCEL_OFFSET 5
#define M2_TEST_SPEED_OFFSET 6
#define M2_TEST_ACCEL_OFFSET 7

void init_table(void)
{
    memset(&table,0,PARAMS_TABLE_LENGTH*sizeof(uint32_t));

    tab_write_16b(M1_TEST_SPEED_OFFSET,TEST_SPEED_DEFAULT);
    tab_write_16b(M1_TEST_ACCEL_OFFSET,TEST_ACCEL_DEFAULT);
    tab_write_16b(M2_TEST_SPEED_OFFSET,TEST_SPEED_DEFAULT);
    tab_write_16b(M2_TEST_ACCEL_OFFSET,TEST_ACCEL_DEFAULT);
}

void tab_write(int pos, uint32_t value)
{
    if (pos<PARAMS_TABLE_LENGTH) {
        ROM_IntMasterDisable();
        table[pos] = value;
        ROM_IntMasterEnable();
    }
}

uint32_t tab_read(int pos)
{
    if (pos<PARAMS_TABLE_LENGTH)
        return table[pos];
    return 0;
}

void tab_write_16b(int pos16, uint16_t val16)
{
    int pos32 = pos16>>1;
    uint32_t mask;
    uint32_t setv;
    if ((pos16&0x1)==0) {
        mask=0xFFFF0000;
        setv=(uint32_t)val16;
    } else {
        mask=0x0000FFFF;
        setv=((uint32_t)val16<<16);
    }

    ROM_IntMasterDisable();

    uint32_t val32 = table[pos32];
    val32 &= mask;
    val32 |= setv;
    table[pos32]=val32;

    ROM_IntMasterEnable();
}

uint16_t tab_read_16b(int pos16)
{
    int pos32 = pos16>>1;
    uint32_t val32 = table[pos32];

    if ((pos16&0x1)==0)
        val32 &= 0xFFFF;
    else
        val32 >>= 16;

    return val32;
}
