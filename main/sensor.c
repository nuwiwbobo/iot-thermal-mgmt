#include "sensor.h"
#include "config.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include <math.h>

static adc_oneshot_unit_handle_t adc_handle;
static adc_cali_handle_t cali_handle;
static float last_temp = 25.0f;
static float ring_buffer[ADC_SAMPLES];
static int ring_idx = 0;
static int ring_count = 0;
static bool valid = false;

static bool init_calibration(void) {
    adc_cali_line_fitting_config_t cal_cfg = {
        .unit_id = ADC_UNIT_1,
        .atten = LM35_ADC_ATTEN,
        .bitwidth = ADC_BIT_WIDTH,
    };
    return adc_cali_create_scheme_line_fitting(&cal_cfg, &cali_handle) == ESP_OK;
}

void sensor_init(void) {
    adc_oneshot_unit_init_cfg_t init_cfg = { .unit_id = ADC_UNIT_1 };
    adc_oneshot_new_unit(&init_cfg, &adc_handle);

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = LM35_ADC_ATTEN,
        .bitwidth = ADC_BIT_WIDTH,
    };
    adc_oneshot_config_channel(adc_handle, LM35_ADC_CHANNEL, &chan_cfg);
    init_calibration();
}

float sensor_read_temp(void) {
    int raw;
    if (adc_oneshot_read(adc_handle, LM35_ADC_CHANNEL, &raw) != ESP_OK) {
        valid = false;
        return last_temp;
    }

    int mv;
    adc_cali_raw_to_voltage(cali_handle, raw, &mv);

    float temp = mv / (float)LM35_MV_PER_C;

    if (ring_count > 0 && ring_count < ADC_SAMPLES) {
        float avg = 0;
        for (int i = 0; i < ring_count; i++) avg += ring_buffer[i];
        avg /= ring_count;
        if (fabsf(temp - avg) > 15.0f) return last_temp;
    }

    ring_buffer[ring_idx] = temp;
    ring_idx = (ring_idx + 1) % ADC_SAMPLES;
    if (ring_count < ADC_SAMPLES) ring_count++;

    float sum = 0;
    for (int i = 0; i < ring_count; i++) sum += ring_buffer[i];
    last_temp = sum / ring_count;
    valid = true;
    return last_temp;
}

float sensor_get_last_temp(void) { return last_temp; }
bool sensor_is_valid(void) { return valid; }