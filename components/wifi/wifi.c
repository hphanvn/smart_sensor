#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
//#include "esp_event.h"
#include "esp_log.h"
#include "wifi.h"

#if CONFIG_EXAMPLE_CONNECT_WIFI
#include "esp_wifi.h"
#endif

#include "protocol_examples_common.h"

void wifiInit(){
    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
    */
    ESP_ERROR_CHECK(example_connect());

    #if CONFIG_EXAMPLE_CONNECT_WIFI
        /* Ensure to disable any WiFi power save mode, this allows best throughput
        * and hence timings for overall OTA operation.
        */
        esp_wifi_set_ps(WIFI_PS_NONE);
    #endif // CONFIG_EXAMPLE_CONNECT_WIFI
}