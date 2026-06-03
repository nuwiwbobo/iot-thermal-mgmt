#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include <stdbool.h>

void sensor_init(void);
float sensor_read_temp(void);
float sensor_get_last_temp(void);
bool sensor_is_valid(void);

#endif