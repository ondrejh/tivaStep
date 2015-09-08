// motor A ios
#define MA_ENA_H() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,GPIO_PIN_4);}while(0)
#define MA_ENA_L() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,0);}while(0)
#define MA_PUL_H() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_PIN_2);}while(0)
#define MA_PUL_L() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,0);}while(0)
#define MA_DIR_H() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,GPIO_PIN_3);}while(0)
#define MA_DIR_L() do{GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,0);}while(0)
#define MA_FLT (GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_6)!=0)

// motor B ios
#define MB_ENA_H() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,GPIO_PIN_5);}while(0)
#define MB_ENA_L() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,0);}while(0)
#define MB_PUL_H() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,GPIO_PIN_7);}while(0)
#define MB_PUL_L() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);}while(0)
#define MB_DIR_H() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_PIN_6);}while(0)
#define MB_DIR_L() do{GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0);}while(0)
#define MB_FLT (GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_4)!=0)

#define MOTORS 2

#define MOTOR_STATUS_ENABLE 0x1
#define MOTOR_STATUS_DIRECTION 0x2
#define MOTOR_STATUS_RUN 0x4

#define MOTOR_IOS_ENA MOTOR_STATUS_ENABLE
#define MOTOR_IOS_DIR MOTOR_STATUS_DIRECTION
#define MOTOR_IOS_PUL 0x4

typedef struct {
    unsigned int step_period;
    unsigned int status;

    unsigned int ios;
    unsigned int step_period_counter;
} motor_lowlev_t;

motor_lowlev_t motors_lowlev[MOTORS];

void motor_steps(unsigned int t)
{
    int i;
    for (i=0;i<MOTORS;i++) {
        if (motors_lowlev[i].status&MOTOR_STATUS_RUN) {
            motors_lowlev[i].step_period_counter+=t;
        } else {
            motors_lowlev[i].step_period_counter=0;
        }
    }

    if (motors_lowlev[0].ios&MOTOR_IOS_PUL) {
        MA_PUL_L();
        motors_lowlev[0].ios&=~MOTOR_IOS_PUL;
    }
}
