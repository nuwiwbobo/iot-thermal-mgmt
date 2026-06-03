#include "actuator.h"
#include "config.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include <math.h>

static uint8_t current_pwm = 0;
static uint8_t target_pwm = 0;
static fan_mode_t mode = MODE_AUTO;
static float setpoint = DEFAULT_SETPOINT;

void actuator_init(void) {
    gpio_set_direction(L298N_IN1_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(L298N_IN2_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(L298N_IN1_GPIO, 1);
    gpio_set_level(L298N_IN2_GPIO, 0);

    ledc_timer_config_t timer_cfg = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = PWM_RESOLUTION,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = PWM_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&timer_cfg);

    ledc_channel_config_t chan_cfg = {
        .gpio_num = L298N_ENA_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
    };
    ledc_channel_config(&chan_cfg);
}

void actuator_set_pwm(uint8_t duty) {
    target_pwm = (duty > PWM_MAX) ? PWM_MAX : duty;
}

void actuator_ramp_to(uint8_t target) {
    target_pwm = (target > PWM_MAX) ? PWM_MAX : target;
}

uint8_t actuator_get_pwm(void) { return current_pwm; }
void actuator_set_mode(fan_mode_t m) { mode = m; }
fan_mode_t actuator_get_mode(void) { return mode; }
void actuator_set_setpoint(float sp) { setpoint = sp; }
float actuator_get_setpoint(void) { return setpoint; }

void actuator_compute(float temp_c) {
    if (temp_c >= TEMP_CRITICAL) {
        actuator_set_pwm(0);
        return;
    }
    if (temp_c >= TEMP_SAFETY_MAX) {
        actuator_set_pwm(PWM_MAX);
        return;
    }

    if (mode == MODE_AUTO) {
        float error = temp_c - setpoint;
        if (fabsf(error) < TEMP_HYSTERESIS) {
            // hold current
        } else if (error <= 0) {
            target_pwm = 0;
        } else {
            int pwm = (int)(error * PID_P_GAIN);
            if (pwm < PWM_MIN) pwm = PWM_MIN;
            if (pwm > PWM_MAX) pwm = PWM_MAX;
            target_pwm = pwm;
        }
    }

    if (current_pwm < target_pwm) {
        current_pwm += PWM_RAMP_STEP;
        if (current_pwm > target_pwm) current_pwm = target_pwm;
    } else if (current_pwm > target_pwm) {
        if (current_pwm > PWM_RAMP_STEP) current_pwm -= PWM_RAMP_STEP;
        else current_pwm = 0;
    }

    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, current_pwm);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}