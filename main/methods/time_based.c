#include <stdint.h>

#include "button_config.h"
#include "button_method.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#define DEBOUNCE_TIME_MS 50

static const char* TAG = "time_based";
static volatile bool button_pressed;
static int click_count;
static int64_t last_accepted_time_ms;

// The ISR only signals that an edge occurred.
static void button_isr_handler(void* arg) {
  const int64_t current_time_ms = esp_timer_get_time() / 1000;

  if (current_time_ms - last_accepted_time_ms >= DEBOUNCE_TIME_MS) {
    last_accepted_time_ms = current_time_ms;

    button_pressed = true;
  }
}

void button_method_init(void) {
  const gpio_config_t button_config = {
      .pin_bit_mask = 1ULL << BUTTON_GPIO,
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_NEGEDGE,
  };

  ESP_ERROR_CHECK(gpio_config(&button_config));
  ESP_ERROR_CHECK(gpio_install_isr_service(0));
  ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL));

  ESP_LOGI(TAG, "Ready. GPIO %d, debounce: %d ms", BUTTON_GPIO,
           DEBOUNCE_TIME_MS);
}

void button_method_run(void) {
  if (button_pressed) {
    click_count++;

    ESP_LOGI(TAG, "Click count: %d", click_count);
    ESP_LOGI(TAG, "Button pressed");
    button_pressed = false;
  }
}
