#include "button_config.h"
#include "button_method.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

static const char* TAG = "no_debounce";
static volatile int click_count;
static volatile bool button_pressed;

static void button_isr_handler(void* arg) {
  button_pressed = true;
  click_count++;
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
  ESP_ERROR_CHECK(
      gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL));

  ESP_LOGI(TAG, "Ready. GPIO %d, interrupt: falling edge", BUTTON_GPIO);
}

void button_method_run(void) {
  if (button_pressed) {
    ESP_LOGI(TAG, "Button pressed");
    ESP_LOGI(TAG, "Click count: %d", click_count);
    button_pressed = false;
  }
}
