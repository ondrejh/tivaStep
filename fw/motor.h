#ifndef __MOTOR_H__
#define __MOTOR_H__

#define MOTORS 2

void init_motors_lowlevel(void);

void motor_set_enable(int motor_id, bool enable);
void motor_set_period(int motor_id, bool run, bool direction, unsigned int step_period);

int32_t motor_get_position(unsigned int id);
void motor_set_position(unsigned int id, int32_t pos);

#endif // __MOTOR_H__
