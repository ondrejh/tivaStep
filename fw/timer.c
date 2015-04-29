#ifndef __TIMER_C__
#define __TIMER_C__


/******* defines ********/

#define TICKS_PER_MICROSEC 16

#define get_fast_ticks() TimerValueGet(TIMER0_BASE,TIMER_A)
#define get_slow_ticks() TimerValueGet(TIMER0_BASE,TIMER_B)


/****** prototypes ******/

void init_timer(void);
void wait_ticks(uint32_t t);
void wait_micros(uint32_t us);


/**** implementation ****/

void init_timer(void)
{
    // peripheral
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // timer0

    // free running timer
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC_UP);
    TimerEnable(TIMER0_BASE, TIMER_BOTH);
}

/*uint32_t get_fast_ticks(void)
{
    return TimerValueGet(TIMER0_BASE,TIMER_A);
}*/

/*uint32_t get_slow_ticks(void)
{
    return TimerValueGet(TIMER0_BASE,TIMER_B);
}*/

void wait_ticks(uint32_t t)
{
    uint32_t tstart = get_fast_ticks();
    while ((get_fast_ticks()-tstart)<t);
}

#define TICKS_PER_MICROSEC 16

void wait_micros(uint32_t us)
{
    uint32_t tstart = get_fast_ticks();
    uint32_t t = us*TICKS_PER_MICROSEC;
    while ((get_fast_ticks()-tstart)<t);
}

#endif // __TIMER_C__
