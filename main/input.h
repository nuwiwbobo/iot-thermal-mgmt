#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*btn_callback_t)(int delta, bool long_press);

void input_init(btn_callback_t cb);
void input_scan(void);

#endif