#ifndef __PARAMS_H__
#define __PARAMS_H__

#define TEST_SPEED_DEFAULT 2000
#define TEST_ACCEL_DEFAULT 5000

#define TEST_TURN_STEPS_DEFAULT (16*200*1000)
#define TEST_TURN_TIME_DEFAULT (60*60*24)

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
#define M1_GOTOPOS_OFFSET 8
#define M2_GOTOPOS_OFFSET 9

// tracking setup
#define M1_TIME_ZERO 10
#define M2_TIME_ZERO 11
#define M1_STEPS_ZERO 12
#define M2_STEPS_ZERO 13
#define M1_TURN_STEPS 14
#define M2_TURN_STEPS 15
#define M1_TURN_TIME 16
#define M2_TURN_TIME 17

void init_table(void);
void tab_write(int pos, uint32_t value);
uint32_t tab_read(int pos);
void tab_write_16b(int pos16, uint16_t val16);
uint16_t tab_read_16b(int pos16);

#endif // __PARAMS_H__
