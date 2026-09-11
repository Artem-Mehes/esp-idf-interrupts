#include <stdint.h>

#include "button_config.h"
#include "button_method.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#define POLLING_INTERVAL_MS 10
#define DEBOUNCE_TIME_MS 50

static const char *TAG = "polling";
static int s_last_raw_state;
static int s_stable_state;
static int s_click_count;
static int64_t s_last_poll_time_ms;
static int64_t s_last_raw_change_time_ms;

void button_method_init(void)
{
    const gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&button_config));

    // At startup, the current physical level is considered stable.
    s_last_raw_state = gpio_get_level(BUTTON_GPIO);
    s_stable_state = s_last_raw_state;

    ESP_LOGI(TAG, "Ready. GPIO %d, polling: %d ms, debounce: %d ms",
             BUTTON_GPIO, POLLING_INTERVAL_MS, DEBOUNCE_TIME_MS);
}

void button_method_run(void)
{
    const int64_t current_time_ms = esp_timer_get_time() / 1000;

    // Poll the button only once every 10 ms.
    if (current_time_ms - s_last_poll_time_ms < POLLING_INTERVAL_MS) {
        return;
    }

    s_last_poll_time_ms = current_time_ms;

    const int raw_state = gpio_get_level(BUTTON_GPIO);

    // A physical level change restarts the debounce timer.
    if (raw_state != s_last_raw_state) {
        s_last_raw_state = raw_state;
        s_last_raw_change_time_ms = current_time_ms;
    }

    // The new level was unchanged for 50 ms, so it is now stable.
    if (raw_state != s_stable_state &&
        current_time_ms - s_last_raw_change_time_ms >= DEBOUNCE_TIME_MS) {
        s_stable_state = raw_state;

        // Count only a stable press (INPUT_PULLUP means LOW is pressed).
        if (s_stable_state == 0) {
            s_click_count++;
            ESP_LOGI(TAG, "Click count: %d", s_click_count);
        }
    }
}
