#include "includes.h"
#include "rtc.h"

/**
 * initialize rtc module
 **/
void rtc_init(void)
{
    HibernateEnableExpClk(SysCtlClockGet());
    HibernateRTCSet(0);
    HibernateRTCEnable();
}

/**
 * read rtc time
 *
 * output: s .. seconds
 *         subs .. subseconds
 **/
void rtc_get_time(uint32_t *s,uint32_t *subs)
{
    uint32_t ls1,ls2,lss;

    IntMasterDisable();

    do {
        ls1 = HWREG(HIB_RTCC);
        lss = HWREG(HIB_RTCSS);
        ls2 = HWREG(HIB_RTCC);
    } while (ls1!=ls2);

    IntMasterEnable();

    *s = ls1;
    *subs = lss;
}

/**
 * set rtc time
 *
 * input: s .. seconds
 **/
void rtc_set_time(uint32_t s)
{
    HWREG(HIB_RTCLD) = s;
}
