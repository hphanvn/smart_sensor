/* Advanced HTTPS OTA example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"


#include "wifi.h"
#include "blufi.h"


#include "taskMqtt.h"
#include "taskStmp.h"
#include "taskOta.h"
#include "taskServer.h"
#include "taskOledDisplay.h"
#include "taskAdcRead.h"
#include "taskDHT11.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include "main.h"

#define HARD_CODE_WIFI

QueueHandle_t xQueueAdcBattVolt;
SemaphoreHandle_t xMutexAdcBattVolt;

QueueHandle_t xQueueDht11Humid;
SemaphoreHandle_t xMutexDht11Humid;

QueueHandle_t xQueueDht11Temp;
SemaphoreHandle_t xMutexDht11Temp;

esp_event_handler_instance_t instance_got_ip;

void post_wifi_config(void){
    xTaskCreate(&oled_task, "oled_task", TASK_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(&adc_task, "adc_task", TASK_STACK_SIZE , NULL, 5, NULL);
    xTaskCreate(&dht11_task, "dht11_task", TASK_STACK_SIZE , NULL, 5, NULL);
    xTaskCreate(&mqtt_task, "mqtt_task", TASK_STACK_SIZE, NULL, 5, NULL);
    // GPIO initialization
    // gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    // setup_server();
    //OTA need to be trigger by web or app or button!
    //xTaskCreate(&ota_task, "ota_task", TASK_STACK_SIZE , NULL, 4, NULL);
}

#ifndef HARD_CODE_WIFI
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    // Process events
    post_wifi_config();
}
#endif

void app_main(void)
{
    // Initialize NVS.
    //ESP_ERROR_CHECK(nvs_flash_erase());
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // 1.OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // 2.NVS partition contains data in new format and cannot be recognized by this version of code.
        // If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Create the message queue. */
    xQueueAdcBattVolt = xQueueCreate(QUEUE_ADC_LEN__BATT_VOLT, QUEUE_ADC_SIZE__BATT_VOLT);
    xQueueDht11Humid= xQueueCreate(QUEUE_DHT11_LEN__HUMI, QUEUE_DHT11_SIZE__HUMI);
    xQueueDht11Temp = xQueueCreate(QUEUE_DHT11_LEN__TEMP, QUEUE_DHT11_SIZE__TEMP);
    /* Create the mutex. */
    xMutexAdcBattVolt = xSemaphoreCreateMutex();
    xMutexDht11Humid = xSemaphoreCreateMutex();
    xMutexDht11Temp = xSemaphoreCreateMutex();


#ifdef HARD_CODE_WIFI
    setup_wifi(); 
    post_wifi_config();
#else  
    setup_wifi_via_blufi();
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                            &event_handler,NULL, &instance_got_ip));
#endif
}

