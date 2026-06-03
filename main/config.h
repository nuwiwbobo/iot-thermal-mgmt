#ifndef CONFIG_H
#define CONFIG_H

#include "soc/adc_channel.h"

// ===== Pin Definitions =====
#define LM35_ADC_CHANNEL   ADC_CHANNEL_6     // GPIO34 on ADC1
#define L298N_ENA_GPIO     16
#define L298N_IN1_GPIO     17
#define L298N_IN2_GPIO     18
#define BTN_MODE_GPIO      4
#define BTN_VAL_GPIO       5
#define VBAT_ADC_CHANNEL   ADC_CHANNEL_7     // GPIO35 on ADC1

// ===== Timing (ms) =====
#define TEMP_SAMPLE_MS     50
#define BUTTON_SCAN_MS     100
#define FAN_CTRL_MS        200
#define LOG_FLUSH_MS       10000

// ===== ADC =====
#define ADC_BIT_WIDTH      ADC_BITWIDTH_12
#define ADC_SAMPLES        10
#define LM35_MV_PER_C      10
#define LM35_ADC_ATTEN     ADC_ATTEN_DB_0    // 0-1.1V range

// ===== Temperature Thresholds =====
#define TEMP_SAFETY_MAX    60.0f
#define TEMP_CRITICAL      75.0f
#define TEMP_HYSTERESIS    0.5f
#define DEFAULT_SETPOINT   30.0f

// ===== PWM =====
#define PWM_FREQ_HZ        5000
#define PWM_RESOLUTION     LEDC_TIMER_8_BIT
#define PWM_MAX            255
#define PWM_MIN            25
#define PWM_RAMP_STEP      25
#define PID_P_GAIN         12.75f   // 255 / 20°C full range

// ===== WiFi =====
#define WIFI_AP_SSID       "ThermalCtrl-AP"
#define WIFI_AP_PASS       "thermal123"
#define WIFI_STA_SSID      "myssid"
#define WIFI_STA_PASS      "mypassword"
#define WIFI_TIMEOUT_MS    10000
#define WIFI_RECONNECT_MS  30000

// ===== Logging =====
#define LOG_RING_SIZE      100
#define LOG_FLUSH_COUNT    100
#define LOG_MAX_ENTRIES    512
#define LOG_FILE           "/log.csv"

// ===== Button =====
#define DEBOUNCE_MS        50
#define LONG_PRESS_MS      500
#define BTN_ADJUST_FINE    1
#define BTN_ADJUST_COARSE  10

#endif