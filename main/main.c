#include "button_method.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    button_method_init();

    while (true) {
        button_method_run();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
