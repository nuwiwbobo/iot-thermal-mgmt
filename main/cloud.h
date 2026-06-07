#ifndef CLOUD_H
#define CLOUD_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "config.h"
#include "storage.h"

void cloud_init(void);
void cloud_upload(log_entry_t *entry);
bool cloud_is_online(void);
int64_t cloud_last_upload_ms(void);
int cloud_get_ok_count(void);
int cloud_get_fail_count(void);

// Testable helpers (inlined so host tests can call them without ESP deps).
static inline int cloud_format_json(const log_entry_t *e, char *buf, size_t max_len) {
    if (!e || !buf || max_len == 0) return 0;

    unsigned fan_pct = ((unsigned)e->fan_pwm * 100u) / PWM_MAX;
    if (fan_pct > 100) fan_pct = 100;

    return snprintf(buf, max_len,
        "{\"sample\":%lu,\"temp\":%.1f,\"fan\":%u,"
        "\"mode\":\"%s\",\"setpoint\":%.1f}",
        (unsigned long)e->sample, e->temp_c, fan_pct,
        e->mode ? e->mode : "?", e->setpoint);
}

static inline bool cloud_should_upload(int64_t now_ms, int64_t last_ms, uint32_t interval_ms) {
    if (last_ms == 0) return true;  // First upload fires immediately
    return (now_ms - last_ms) >= (int64_t)interval_ms;
}

#endif
