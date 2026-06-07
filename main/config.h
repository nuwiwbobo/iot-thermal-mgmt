#ifndef CONFIG_H
#define CONFIG_H

// ===== BMP280 I2C Pins =====
#define BMP280_SDA_GPIO      21
#define BMP280_SCL_GPIO      22
#define BMP280_I2C_PORT      I2C_NUM_0
#define BMP280_I2C_CLOCK_HZ  100000
#define BMP280_ADDR          0x76      // SDO=GND → 0x76, SDO=3.3V → 0x77

// ===== L298N Actuator Pins =====
#define L298N_ENA_GPIO       16
#define L298N_IN1_GPIO       17
#define L298N_IN2_GPIO       18

// ===== Button Pins =====
#define BTN_MODE_GPIO        4
#define BTN_VAL_GPIO         5

// ===== Timing (ms) =====
#define TEMP_SAMPLE_MS       50
#define BUTTON_SCAN_MS       100
#define FAN_CTRL_MS          200
#define LOG_FLUSH_MS         10000

// ===== Temperature Thresholds =====
#define TEMP_SAFETY_MAX      60.0f
#define TEMP_CRITICAL        75.0f
#define TEMP_HYSTERESIS      0.5f
#define DEFAULT_SETPOINT     30.0f

// ===== PWM =====
#define PWM_FREQ_HZ          5000
#define PWM_RESOLUTION       LEDC_TIMER_8_BIT
#define PWM_MAX              255
#define PWM_MIN            80      // ~30% duty — needed to start most 12V fans
#define PWM_RAMP_STEP        40
#define PID_P_GAIN           25.5f   // 255 / 10°C = full speed at 10°C error

// ===== WiFi =====
#define WIFI_AP_SSID         "ThermalCtrl-AP"
#define WIFI_AP_PASS         "thermal123"
#define WIFI_STA_SSID        "TP-Link_4684"
#define WIFI_STA_PASS        "06410674"
#define WIFI_TIMEOUT_MS      10000
#define WIFI_RECONNECT_MS    30000

// ===== Logging =====
#define LOG_RING_SIZE        100
#define LOG_FLUSH_COUNT      100
#define LOG_MAX_ENTRIES      512
#define LOG_FILE             "/log.csv"

// ===== Cloud Upload (Google Sheets via Apps Script) =====
#define CLOUD_URL            "https://script.google.com/macros/s/AKfycbyHPJQBr3L04ZIFWRBOCTaIvbONiBZM8uMsAF8RfqeiZSkojhrwf_KChDXt98r0_sBKlw/exec"
#define CLOUD_UPLOAD_MS      30000     // 30s between uploads

// ===== Button =====
#define DEBOUNCE_MS          50
#define LONG_PRESS_MS        500
#define BTN_ADJUST_FINE      1
#define BTN_ADJUST_COARSE    10

#endif
