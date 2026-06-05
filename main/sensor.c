#include "sensor.h"
#include "config.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

static const char* TAG = "sensor";

static i2c_master_dev_handle_t bmp280_dev = NULL;
static float last_temp = 25.0f;
static bool valid = false;

// Calibration data
static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;
static uint16_t dig_P1;
static int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
static int32_t t_fine;

#define BMP280_REG_ID          0xD0
#define BMP280_REG_RESET       0xE0
#define BMP280_REG_CTRL_MEAS   0xF4
#define BMP280_REG_CONFIG      0xF5
#define BMP280_REG_CALIB       0x88
#define BMP280_REG_TEMP_MSB    0xFA
#define BMP280_REG_PRESS_MSB   0xF7

#define BMP280_ID_BMP280       0x58
#define BMP280_ID_BME280       0x60
#define BMP280_SOFT_RESET_CMD  0xB6

static esp_err_t bmp280_read_regs(uint8_t reg, uint8_t* buf, size_t len) {
    return i2c_master_transmit_receive(bmp280_dev, &reg, 1, buf, len, 100);
}

static esp_err_t bmp280_write_reg(uint8_t reg, uint8_t val) {
    uint8_t buf[2] = { reg, val };
    return i2c_master_transmit(bmp280_dev, buf, 2, 100);
}

static bool read_calibration(void) {
    uint8_t cal[24];
    if (bmp280_read_regs(0x88, cal, 24) != ESP_OK) return false;

    dig_T1 = (uint16_t)cal[1] << 8 | cal[0];
    dig_T2 = (int16_t)((uint16_t)cal[3] << 8 | cal[2]);
    dig_T3 = (int16_t)((uint16_t)cal[5] << 8 | cal[4]);

    dig_P1 = (uint16_t)cal[7] << 8 | cal[6];
    dig_P2 = (int16_t)((uint16_t)cal[9] << 8 | cal[8]);
    dig_P3 = (int16_t)((uint16_t)cal[11] << 8 | cal[10]);
    dig_P4 = (int16_t)((uint16_t)cal[13] << 8 | cal[12]);
    dig_P5 = (int16_t)((uint16_t)cal[15] << 8 | cal[14]);
    dig_P6 = (int16_t)((uint16_t)cal[17] << 8 | cal[16]);
    dig_P7 = (int16_t)((uint16_t)cal[19] << 8 | cal[18]);
    dig_P8 = (int16_t)((uint16_t)cal[21] << 8 | cal[20]);
    dig_P9 = (int16_t)((uint16_t)cal[23] << 8 | cal[22]);

    ESP_LOGI(TAG, "Calibration: T1=%u T2=%d T3=%d", dig_T1, dig_T2, dig_T3);
    return true;
}

void sensor_init(void) {
    i2c_master_bus_config_t bus_config = {
        .i2c_port = BMP280_I2C_PORT,
        .sda_io_num = BMP280_SDA_GPIO,
        .scl_io_num = BMP280_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    if (i2c_new_master_bus(&bus_config, &bus_handle) != ESP_OK) {
        ESP_LOGE(TAG, "I2C bus init failed");
        return;
    }

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BMP280_ADDR,
        .scl_speed_hz = BMP280_I2C_CLOCK_HZ,
    };

    if (i2c_master_bus_add_device(bus_handle, &dev_config, &bmp280_dev) != ESP_OK) {
        ESP_LOGE(TAG, "BMP280 device add failed");
        return;
    }

    // Verify chip ID (accept BMP280=0x58 or BME280=0x60)
    uint8_t id;
    if (bmp280_read_regs(BMP280_REG_ID, &id, 1) != ESP_OK
        || (id != BMP280_ID_BMP280 && id != BMP280_ID_BME280)) {
        ESP_LOGE(TAG, "BMP280/BME280 not found (ID=0x%02X)", id);
        return;
    }
    const char* name = (id == BMP280_ID_BME280) ? "BME280" : "BMP280";
    ESP_LOGI(TAG, "%s found (ID=0x%02X)", name, id);

    // Read calibration data
    if (!read_calibration()) {
        ESP_LOGE(TAG, "Failed to read calibration");
        return;
    }

    // Soft reset (optional)
    bmp280_write_reg(BMP280_REG_RESET, BMP280_SOFT_RESET_CMD);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Configure: temp oversampling x2, pressure off, normal mode
    bmp280_write_reg(BMP280_REG_CTRL_MEAS, 0x2 << 5 | 0x0 << 2 | 0x3);
    // Config: standby 0.5ms, filter off
    bmp280_write_reg(BMP280_REG_CONFIG, 0x0 << 5 | 0x0 << 2);

    vTaskDelay(pdMS_TO_TICKS(10));
    valid = true;
    ESP_LOGI(TAG, "Sensor ready");
}

static int32_t compensate_temp(int32_t adc_T) {
    int32_t var1, var2;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * (int32_t)dig_T2) >> 11;
    var2 = (((((adc_T >> 4) - (int32_t)dig_T1) * ((adc_T >> 4) - (int32_t)dig_T1)) >> 12) * (int32_t)dig_T3) >> 14;
    t_fine = var1 + var2;
    return (t_fine * 5 + 128) >> 8;
}

static uint32_t compensate_pressure(int32_t adc_P) {
    int64_t var1, var2, p;
    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * (int64_t)dig_P1 >> 33;
    if (var1 == 0) return 0;
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = ((int64_t)dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((int64_t)dig_P8 * p) >> 19;
    p = ((p + var1 + var2) >> 8) + ((int64_t)dig_P7 << 4);
    return (uint32_t)p;
}

float sensor_read_temp(void) {
    if (!valid) return last_temp;

    uint8_t buf[6];
    if (bmp280_read_regs(BMP280_REG_PRESS_MSB, buf, 6) != ESP_OK) {
        return last_temp;
    }

    int32_t adc_P = (int32_t)buf[0] << 12 | (int32_t)buf[1] << 4 | (int32_t)buf[2] >> 4;
    int32_t adc_T = (int32_t)buf[3] << 12 | (int32_t)buf[4] << 4 | (int32_t)buf[5] >> 4;

    int32_t temp_raw = compensate_temp(adc_T);
    uint32_t press_raw = compensate_pressure(adc_P);

    last_temp = temp_raw / 100.0f;
    (void)press_raw;

    return last_temp;
}

float sensor_get_last_temp(void) { return last_temp; }
bool sensor_is_valid(void) { return valid; }
