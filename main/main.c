#include "esp_log.h"
#include "ethernet.h"

void app_main(void)
{
    ESP_ERROR_CHECK(ethernet_start());
}
