#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdint.h>

typedef enum { MODE_AUTO, MODE_MANUAL } fan_mode_t;

void actuator_init(void);
void actuator_set_pwm(uint8_t duty);
void actuator_ramp_to(uint8_t target);
uint8_t actuator_get_pwm(void);
void actuator_set_mode(fan_mode_t mode);
fan_mode_t actuator_get_mode(void);
void actuator_set_setpoint(float sp);
float actuator_get_setpoint(void);
void actuator_compute(float temp_c);

#endif