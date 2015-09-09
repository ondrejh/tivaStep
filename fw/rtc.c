#include "includes.h"
#include "rtc.h"

void rtc_init(void)
{
    HibernateEnableExpClk(SysCtlClockGet());
    HibernateRTCSet(0);
    HibernateRTCEnable();
}

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

void rtc_set_time(uint32_t s)
{
    HWREG(HIB_RTCLD) = s;
}
