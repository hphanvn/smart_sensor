#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "main.h"
#include "dht11.h"
#include "driver/gpio.h"

#define DI_DHT11__DATA              GPIO_NUM_19
#define SENSOR_TYPE                 DHT_TYPE_DHT11

const static char *TAG = "dht11_task";

void dht11_task(void *pvParameter){
    float temperature, humidity;
    
    ESP_LOGI(TAG, "Starting DHT Task\n\n");

    while (1)
    {
        if (dht_read_float_data(SENSOR_TYPE, DI_DHT11__DATA, &humidity, &temperature) == ESP_OK){
             ESP_LOGD(TAG,"Humidity: %.1f%% Temp: %.1fC\n", humidity, temperature);
            /* Take the mutex to ensure exclusive access to the queue. */
            xSemaphoreTake(xMutexDht11Humid, portMAX_DELAY);
            xSemaphoreTake(xMutexDht11Temp, portMAX_DELAY);
            /* Send the next value to the queue. */
            xQueueSend(xQueueDht11Humid, &humidity, 0);
            xQueueSend(xQueueDht11Temp, &temperature, 0);
             /* Give back the mutex. */
            xSemaphoreGive(xMutexDht11Humid);
            xSemaphoreGive(xMutexDht11Temp);
        }
        else
             ESP_LOGE(TAG,"Could not read data from sensor\n");

        // If you read the sensor data too often, it will heat up
        // http://www.kandrsmith.org/RJS/Misc/Hygrometers/dht_sht_how_fast.html
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
