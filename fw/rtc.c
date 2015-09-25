#include "includes.h"
#include "rtc.h"

/**
 * initialize rtc module
 **/
void rtc_init(void)
{
    // Hibernate RTC Trim according to:
    //   http://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/p/259103/907463
    //   otherwise it skips every 65th second
    HibernateEnableExpClk(SysCtlClockGet());
    HibernateClockConfig(HIBERNATE_OSC_LOWDRIVE);
    SysCtlDelay(SysCtlClockGet()*3);
    HibernateRTCTrimSet(0x7FFF);
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
