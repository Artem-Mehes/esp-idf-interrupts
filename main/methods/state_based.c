#include <stdbool.h>
#include <stdint.h>

#include "button_config.h"
#include "button_method.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#define DEBOUNCE_TIME_MS 50

static const char *TAG = "state_based";
static volatile bool button_pressed;
static bool waiting_for_release;
static bool checking_release;
static int click_count;
static int64_t release_start_time_ms;

// The ISR only signals that a falling edge occurred.
static void button_isr_handler(void *arg)
{
    button_pressed = true;
}

void button_method_init(void)
{
    const gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };

    ESP_ERROR_CHECK(gpio_config(&button_config));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(
        gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL));

    ESP_LOGI(TAG, "Ready. GPIO %d, stable release: %d ms", BUTTON_GPIO,
             DEBOUNCE_TIME_MS);
}

void button_method_run(void)
{
    if (waiting_for_release) {
        if (gpio_get_level(BUTTON_GPIO) == 0) {
            // The button is still pressed, or HIGH was only release bounce.
            checking_release = false;
            return;
        }

        const int64_t current_time_ms = esp_timer_get_time() / 1000;

        if (!checking_release) {
            release_start_time_ms = current_time_ms;
            checking_release = true;
            return;
        }

        if (current_time_ms - release_start_time_ms >= DEBOUNCE_TIME_MS) {
            waiting_for_release = false;
            checking_release = false;
        }

        return;
    }

    if (!button_pressed) {
        return;
    }

    button_pressed = false;

    // Accept the event only while the button is physically still pressed.
    if (gpio_get_level(BUTTON_GPIO) == 0) {
        click_count++;

        ESP_LOGI(TAG, "Click count: %d", click_count);

        waiting_for_release = true;
    }
}
