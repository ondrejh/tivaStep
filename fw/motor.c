/**
 * Low level motor steering module
 *
 * Uses Timer1A as the periodic timer with interrupt to call low level io functions.
 *
 * interface functions:
 *
 *  init_motors_lowlevel(void)
 *      initializes motor GPIOs, variable context and Timer1
 *
 *  motor_set_enable(int motor_id, bool enable)
 *      set/clr motor enable flag
 *
 *  motor_set_period(int motor_id, bool run, bool direction, unsigned int step_period)
 *      set/clr run flag, motor dir flag and step period variable (systicks)
 *
 * local function:
 *
 *  motor_steps_static(void);
 *      function is periodically called to perform motor movement according to step_period and status values
 *
 *  Timer1IntHandler(void);
 *      it what the function name saids .. should be connected in startup_gcc.c interrupt vector table
 **/

#include "includes.h"
#include "motor.h"

#define MOTOR_TIMER_PERIOD 1600 // SysClockGet/10000 (100us)

// motor A ios
#define MA_ENA_H() do{ROM_GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,GPIO_PIN_4);}while(0)
#define MA_ENA_L() do{ROM_GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,0);}while(0)
#define MA_PUL_H() do{ROM_GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_PIN_2);}while(0)
#define MA_PUL_L() do{ROM_GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,0);}while(0)
#define MA_DIR_H() do{ROM_GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,GPIO_PIN_3);}while(0)
#define MA_DIR_L() do{ROM_GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,0);}while(0)
#define MA_FLT (ROM_GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_6)!=0)

// motor B ios
#define MB_ENA_H() do{ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,GPIO_PIN_5);}while(0)
#define MB_ENA_L() do{ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,0);}while(0)
#define MB_PUL_H() do{ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,GPIO_PIN_7);}while(0)
#define MB_PUL_L() do{ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);}while(0)
#define MB_DIR_H() do{ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_PIN_6);}while(0)
#define MB_DIR_L() do{ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0);}while(0)
#define MB_FLT (ROM_GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_4)!=0)

#define MOTOR_STATUS_ENABLE 0x1
#define MOTOR_STATUS_DIRECTION 0x2
#define MOTOR_STATUS_RUN 0x4
#define MOTOR_STATUS_FIRSTSTEP 0x8

#define MOTOR_IOS_ENA MOTOR_STATUS_ENABLE
#define MOTOR_IOS_DIR MOTOR_STATUS_DIRECTION
#define MOTOR_IOS_PUL 0x4

typedef struct {
    unsigned int status;
    unsigned int ios;
    unsigned int step_period;
    unsigned int step_period_counter;
    uint32_t position;
} motor_lowlev_t;

motor_lowlev_t motors_lowlev[MOTORS];

/**
 * get motor position
 **/
uint32_t motor_get_position(int id)
{
    return motors_lowlev[id].position;
}

/**
 * initializes motors ios and internal low level variables
 **/
void init_motors_lowlevel(void)
{
    int i;
    for (i=0;i<MOTORS;i++) {
        motors_lowlev[i].status=0;
        motors_lowlev[i].step_period=0;
        motors_lowlev[i].ios=0;
        motors_lowlev[i].step_period_counter=0;
        motors_lowlev[i].position=0;
    }

    // enable used peripherals
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // init motor ios
    // note: don't forget to remove R9, R10 from Tiva C launchpad board !!!
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4); // motor A PUL,DIR,ENA
    ROM_GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, 0x00); // PUL,DIR,ENA <- 'L'
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_6); // motor A FLT (input)
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7); // motor B ENA,DIR,PUL
    ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0x00); // PUL,DIR,ENA <- 'L'
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,GPIO_PIN_4); // motor B FLT (input)

    // timer 1
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, MOTOR_TIMER_PERIOD);
    ROM_IntEnable(INT_TIMER1A);
    ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerEnable(TIMER1_BASE, TIMER_A);
}

/**
 * set/clr motor enable status flag
 **/
void motor_set_enable(int motor_id, bool enable)
{
    // assert variables
    if (motor_id>=MOTORS)
        return;

    if (enable)
        motors_lowlev[motor_id].status|=MOTOR_STATUS_ENABLE;
    else
        motors_lowlev[motor_id].status&=~MOTOR_STATUS_ENABLE;
}

/**
 * set motor step period and direction
 **/
void motor_set_period(int motor_id, bool run, bool direction, unsigned int step_period)
{
    // assert variables
    if (motor_id>=MOTORS)
        return;

    if (run)
        motors_lowlev[motor_id].status|=MOTOR_STATUS_RUN;
    else
        motors_lowlev[motor_id].status&=~MOTOR_STATUS_RUN;

    if (direction)
        motors_lowlev[motor_id].status|=MOTOR_STATUS_DIRECTION;
    else
        motors_lowlev[motor_id].status&=~MOTOR_STATUS_DIRECTION;

    motors_lowlev[motor_id].step_period = step_period;
}

/**
 * low level motor function operating motor IOS
 *
 * input: time .. counter input
 * 1) Counts step period.
 * 2) If PUL output high - set low.
 * 3) Else If ENA or DIR changes in status - set output
 * 4) Else If Step count reaches step period - set PUL output high and substract valu
 **/
void motor_steps_static(void)
{
    int i;
    for (i=0;i<MOTORS;i++) {
        if ((motors_lowlev[i].status&MOTOR_STATUS_RUN)!=0) {
            motors_lowlev[i].step_period_counter+=MOTOR_TIMER_PERIOD;
        } else {
            motors_lowlev[i].step_period_counter=0;
            motors_lowlev[i].status|=MOTOR_STATUS_FIRSTSTEP;
        }
    }

    if ((motors_lowlev[0].ios&MOTOR_IOS_PUL)!=0) {
        MA_PUL_L();
        motors_lowlev[0].ios&=~MOTOR_IOS_PUL;
    } else {
        if ((motors_lowlev[0].ios&(MOTOR_IOS_ENA|MOTOR_IOS_DIR))!=(motors_lowlev[0].status&(MOTOR_STATUS_ENABLE|MOTOR_STATUS_DIRECTION))) {
            if ((motors_lowlev[0].status&MOTOR_STATUS_ENABLE)!=0) {
                motors_lowlev[0].ios|=MOTOR_IOS_ENA;
                MA_ENA_L();
            } else {
                motors_lowlev[0].ios&=~MOTOR_IOS_ENA;
                MA_ENA_H();
            }
            if ((motors_lowlev[0].status&MOTOR_STATUS_DIRECTION)!=0) {
                motors_lowlev[0].ios|=MOTOR_IOS_DIR;
                MA_DIR_L();
            } else {
                motors_lowlev[0].ios&=~MOTOR_IOS_DIR;
                MA_DIR_H();
            }
            motors_lowlev[0].step_period_counter=0;
            motors_lowlev[0].status|=MOTOR_STATUS_FIRSTSTEP;
        } else {
            if ((motors_lowlev[0].status&(MOTOR_STATUS_RUN|MOTOR_STATUS_ENABLE))==(MOTOR_STATUS_RUN|MOTOR_STATUS_ENABLE)) {
                if (motors_lowlev[0].step_period_counter>=motors_lowlev[0].step_period) {
                    motors_lowlev[0].step_period_counter-=motors_lowlev[0].step_period;
                    motors_lowlev[0].ios|=MOTOR_IOS_PUL;
                    MA_PUL_H();
                }
                else if ((motors_lowlev[0].status&MOTOR_STATUS_FIRSTSTEP)!=0) {
                    motors_lowlev[0].step_period_counter=0;
                    motors_lowlev[0].status&=~MOTOR_STATUS_FIRSTSTEP;
                    motors_lowlev[0].ios|=MOTOR_IOS_PUL;
                    MA_PUL_H();
                }
            }
        }
    }

    if ((motors_lowlev[1].ios&MOTOR_IOS_PUL)!=0) {
        MB_PUL_L();
        motors_lowlev[1].ios&=~MOTOR_IOS_PUL;
    } else {
        if ((motors_lowlev[1].ios&(MOTOR_IOS_ENA|MOTOR_IOS_DIR))!=(motors_lowlev[1].status&(MOTOR_STATUS_ENABLE|MOTOR_STATUS_DIRECTION))) {
            if ((motors_lowlev[1].status&MOTOR_STATUS_ENABLE)!=0) {
                motors_lowlev[1].ios|=MOTOR_IOS_ENA;
                MB_ENA_L();
            } else {
                motors_lowlev[1].ios&=~MOTOR_IOS_ENA;
                MB_ENA_H();
            }
            if ((motors_lowlev[1].status&MOTOR_STATUS_DIRECTION)!=0) {
                motors_lowlev[1].ios|=MOTOR_IOS_DIR;
                MB_DIR_L();
            } else {
                motors_lowlev[1].ios&=~MOTOR_IOS_DIR;
                MB_DIR_H();
            }
            motors_lowlev[1].step_period_counter=0;
            motors_lowlev[1].status|=MOTOR_STATUS_FIRSTSTEP;
        } else {
            if ((motors_lowlev[1].status&MOTOR_STATUS_RUN)!=0) {
                if (motors_lowlev[1].step_period_counter>=motors_lowlev[1].step_period) {
                    motors_lowlev[1].step_period_counter-=motors_lowlev[1].step_period;
                    motors_lowlev[1].ios|=MOTOR_IOS_PUL;
                    MB_PUL_H();
                }
                else if ((motors_lowlev[1].status&MOTOR_STATUS_FIRSTSTEP)!=0) {
                    motors_lowlev[1].step_period_counter=0;
                    motors_lowlev[1].status&=~MOTOR_STATUS_FIRSTSTEP;
                    motors_lowlev[1].ios|=MOTOR_IOS_PUL;
                    MB_PUL_H();
                }
            }
        }
    }
}

void motor_step_timer_interrupt_handler(void)
{
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    motor_steps_static();
}
