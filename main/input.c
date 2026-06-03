#include "input.h"
#include "config.h"
#include "driver/gpio.h"
#include "esp_timer.h"

static btn_callback_t user_cb = NULL;
static bool btn_mode_last = true;
static bool btn_val_last = true;
static int64_t btn_val_press_start = 0;
static bool btn_val_handled = false;

void input_init(btn_callback_t cb) {
    user_cb = cb;
    gpio_set_direction(BTN_MODE_GPIO, GPIO_MODE_INPUT);
    gpio_set_direction(BTN_VAL_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BTN_MODE_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(BTN_VAL_GPIO, GPIO_PULLUP_ONLY);
    btn_mode_last = gpio_get_level(BTN_MODE_GPIO);
    btn_val_last = gpio_get_level(BTN_VAL_GPIO);
}

void input_scan(void) {
    bool mode_now = gpio_get_level(BTN_MODE_GPIO);
    bool val_now = gpio_get_level(BTN_VAL_GPIO);

    if (mode_now == 0 && btn_mode_last == 1) {
        if (user_cb) user_cb(0, false);
    }
    btn_mode_last = mode_now;

    if (val_now == 0 && btn_val_last == 1) {
        btn_val_press_start = esp_timer_get_time() / 1000;
        btn_val_handled = false;
    }

    if (val_now == 0 && !btn_val_handled) {
        int64_t elapsed = (esp_timer_get_time() / 1000) - btn_val_press_start;
        if (elapsed >= LONG_PRESS_MS) {
            if (user_cb) user_cb(BTN_ADJUST_COARSE, true);
            btn_val_handled = true;
        }
    }

    if (val_now == 1 && btn_val_last == 0 && !btn_val_handled) {
        if (user_cb) user_cb(BTN_ADJUST_FINE, false);
    }
    btn_val_last = val_now;
}