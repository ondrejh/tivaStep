#ifndef __RTC_H__
#define __RTC_H__

#define RTC_SUBSEC_ONESEC 0x8000

void rtc_init(void);
void rtc_get_time(uint32_t *s,uint32_t *subs);
void rtc_set_time(uint32_t s);

#endif // __RTC_H__
