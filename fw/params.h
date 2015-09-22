#ifndef __PARAMS_H__
#define __PARAMS_H__

#define TEST_SPEED_DEFAULT 2000
#define TEST_ACCEL_DEFAULT 5000

// 16 bit table
#define M1_TEST_SPEED_OFFSET 4
#define M1_TEST_ACCEL_OFFSET 5
#define M2_TEST_SPEED_OFFSET 6
#define M2_TEST_ACCEL_OFFSET 7

// 32 bit table
#define RTC_SET_OFFSET 4 // registers to set rtc time - 2 registers
#define RTC_TIME_OFFSET 5 // registers contains rtc time

#define M1_POSITION_OFFSET 6 // 32 bit position register
#define M2_POSITION_OFFSET 7

void init_table(void);
void tab_write(int pos, uint32_t value);
uint32_t tab_read(int pos);
void tab_write_16b(int pos16, uint16_t val16);
uint16_t tab_read_16b(int pos16);

#endif // __PARAMS_H__
