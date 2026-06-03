#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t sample;
    uint64_t timestamp_ms;
    float temp_c;
    uint8_t fan_pwm;
    const char* mode;
    float setpoint;
} log_entry_t;

void storage_init(void);
void storage_push(log_entry_t* entry);
int storage_get_all(log_entry_t* buf, int max);
int storage_get_csv(char* buf, int max_len);
int storage_get_plt(char* buf, int max_len);
void storage_clear(void);
void storage_flush(void);

#endif