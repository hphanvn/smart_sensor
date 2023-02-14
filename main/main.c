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
#include "taskOta.h"
#include "taskServer.h"
#include "taskOledDisplay.h"
#include "taskAdcRead.h"
#include "taskDHT11.h"
//For OTA
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "driver/gpio.h"
#include "main.h"

QueueHandle_t xQueueAdcBattVolt;
SemaphoreHandle_t xMutexAdcBattVolt;

QueueHandle_t xQueueDht11Humid;
SemaphoreHandle_t xMutexDht11Humid;

QueueHandle_t xQueueDht11Temp;
SemaphoreHandle_t xMutexDht11Temp;


void app_main(void)
{
    // Initialize NVS.
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

    xTaskCreate(&oled_task, "oled_task", 1024 * 8, NULL, 5, NULL);
    xTaskCreate(&adc_task, "adc_task", 1024 * 8, NULL, 5, NULL);
    xTaskCreate(&dht11_task, "dht11_task", 1024 * 8, NULL, 5, NULL);

    wifiInit();
    // GPIO initialization
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    setup_server();

    xTaskCreate(&ota_task, "ota_task", 1024 * 8, NULL, 6, NULL);
}

