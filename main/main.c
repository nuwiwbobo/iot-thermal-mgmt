#include <stdio.h>
#include "sensor.h"
#include "actuator.h"
#include "input.h"
#include "network.h"
#include "storage.h"
#include "config.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

float g_temp_c = 25.0f;
uint8_t g_fan_pwm = 0;
int g_mode = 0;
float g_setpoint = DEFAULT_SETPOINT;

static const char* TAG = "thermal";
static int64_t last_log_flush = 0;

static void on_button(int delta, bool long_press) {
    if (delta == 0) {
        g_mode = g_mode ? 0 : 1;
        actuator_set_mode((fan_mode_t)g_mode);
        return;
    }

    if (g_mode == 0) { // AUTO MODE
        // Short press = +0.5°C, Long press = +10.0°C
        float adj = long_press ? delta : delta * 0.5f;
        g_setpoint += adj;
        
        // Wrap around logic
        if (g_setpoint > 60.0f) {
            g_setpoint = 15.0f; // Reset to minimum
        }
        actuator_set_setpoint(g_setpoint);
    } 
    else { // MANUAL MODE
        int pwm = actuator_get_pwm();
        pwm += delta; // delta is already 1 or 10 from input.c
        
        // Wrap around logic
        if (pwm > PWM_MAX) {
            pwm = 0; // Reset to minimum (fan off)
        }
        actuator_set_pwm(pwm);
    }
}

static void timer_read_temp(void* arg) {
    g_temp_c = sensor_read_temp();
}

static void timer_scan_buttons(void* arg) {
    input_scan();
}

static int debug_count = 0;

static void timer_control_fan(void* arg) {
    actuator_compute(g_temp_c);
    g_fan_pwm = actuator_get_pwm();

    log_entry_t entry = {
        .temp_c = g_temp_c,
        .fan_pwm = g_fan_pwm,
        .mode = (g_mode == 0) ? "AUTO" : "MANUAL",
        .setpoint = g_setpoint,
    };
    storage_push(&entry);

    if (++debug_count >= 10) {
        ESP_LOGI(TAG, "Temp: %.1f°C  Fan: %u%%  Mode: %s  Setpoint: %.1f°C",
                 g_temp_c, g_fan_pwm * 100 / PWM_MAX,
                 g_mode ? "MANUAL" : "AUTO", g_setpoint);
        debug_count = 0;
    }
}

void app_main(void) {
    sensor_init();
    actuator_init();
    input_init(on_button);
    storage_init();

    esp_timer_handle_t timer_temp, timer_btn, timer_fan;

    esp_timer_create_args_t temp_args = {
        .callback = timer_read_temp,
        .name = "read_temp"
    };
    esp_timer_create(&temp_args, &timer_temp);
    esp_timer_start_periodic(timer_temp, TEMP_SAMPLE_MS * 1000);

    esp_timer_create_args_t btn_args = {
        .callback = timer_scan_buttons,
        .name = "scan_buttons"
    };
    esp_timer_create(&btn_args, &timer_btn);
    esp_timer_start_periodic(timer_btn, BUTTON_SCAN_MS * 1000);

    esp_timer_create_args_t fan_args = {
        .callback = timer_control_fan,
        .name = "control_fan"
    };
    esp_timer_create(&fan_args, &timer_fan);
    esp_timer_start_periodic(timer_fan, FAN_CTRL_MS * 1000);

    network_init();

    while (1) {
        network_poll();

        int64_t now = esp_timer_get_time() / 1000;
        if (now - last_log_flush > LOG_FLUSH_MS) {
            storage_flush();
            last_log_flush = now;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}