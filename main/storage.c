#include "storage.h"
#include "config.h"
#include "esp_spiffs.h"
#include "esp_timer.h"
#include <stdio.h>
#include <string.h>

static log_entry_t ring[LOG_RING_SIZE];
static int ring_head = 0;
static int ring_count = 0;
static uint32_t sample_counter = 0;
static bool spiffs_mounted = false;
static uint32_t last_flushed_sample = 0;

void storage_init(void) {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    spiffs_mounted = (esp_vfs_spiffs_register(&conf) == ESP_OK);
    sample_counter = 0;
}

void storage_push(log_entry_t* entry) {
    entry->sample = sample_counter++;
    entry->timestamp_ms = esp_timer_get_time() / 1000;

    ring[ring_head] = *entry;
    ring_head = (ring_head + 1) % LOG_RING_SIZE;
    if (ring_count < LOG_RING_SIZE) ring_count++;
}

int storage_get_all(log_entry_t* buf, int max) {
    int count = (ring_count < max) ? ring_count : max;
    int start = (ring_head - ring_count + LOG_RING_SIZE) % LOG_RING_SIZE;
    for (int i = 0; i < count; i++) {
        buf[i] = ring[(start + i) % LOG_RING_SIZE];
    }
    return count;
}

int storage_get_csv(char* buf, int max_len) {
    int pos = snprintf(buf, max_len, "Sample, Timestamp_ms, Temp_C, Fan_Pct, Mode, Setpoint\n");
    int start = (ring_head - ring_count + LOG_RING_SIZE) % LOG_RING_SIZE;
    for (int i = 0; i < ring_count && pos < max_len - 80; i++) {
        log_entry_t* e = &ring[(start + i) % LOG_RING_SIZE];
        pos += snprintf(buf + pos, max_len - pos,
            "%lu, %llu, %.1f, %u, %s, %.1f\n",
            (unsigned long)e->sample, (unsigned long long)e->timestamp_ms,
            e->temp_c, (unsigned)(e->fan_pwm * 100 / PWM_MAX), e->mode, e->setpoint);
    }
    return pos;
}

int storage_get_plt(char* buf, int max_len) {
    return snprintf(buf, max_len,
        "set title \"Thermal Management - Temperature & Fan Response\"\n"
        "set xlabel \"Sample (50ms intervals)\"\n"
        "set ylabel \"Temperature (degC)\"\n"
        "set y2label \"Fan PWM (%%)\"\n"
        "set y2tics\n"
        "plot \"log.csv\" using 1:3 with lines title \"Temperature\", "
        "\"\" using 1:4 with lines axes x1y2 title \"Fan PWM (%%)\"\n"
        "pause -1\n");
}

void storage_clear(void) {
    ring_head = 0;
    ring_count = 0;
    sample_counter = 0;
}

void storage_flush(void) {
    if (!spiffs_mounted || ring_count == 0) return;
    
    // Open in "a" (append) mode!
    FILE* f = fopen("/spiffs" LOG_FILE, "a");
    if (!f) return;
    
    char buf[128];
    int start = (ring_head - ring_count + LOG_RING_SIZE) % LOG_RING_SIZE;
    
    for (int i = 0; i < ring_count; i++) {
        log_entry_t* e = &ring[(start + i) % LOG_RING_SIZE];
        
        // ONLY write samples we haven't flushed yet!
        if (e->sample >= last_flushed_sample) {
            snprintf(buf, sizeof(buf),
                "%lu, %llu, %.1f, %u, %s, %.1f\n",
                (unsigned long)e->sample, (unsigned long long)e->timestamp_ms,
                e->temp_c, (unsigned)(e->fan_pwm * 100 / PWM_MAX), e->mode, e->setpoint);
            fputs(buf, f);
            last_flushed_sample = e->sample + 1;
        }
    }
    fclose(f);
}