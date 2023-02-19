// #include <stdio.h>
// #include <stdint.h>
// #include <stddef.h>
// #include <string.h>
// #include "esp_wifi.h"
// #include "esp_system.h"
// #include "nvs_flash.h"
// #include "esp_event.h"
// #include "esp_netif.h"
//#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "main.h"

static const char *TAG = "mqtt_task";

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

#define MQTT_PUB_TEMP_DHT11 "esp32/battMon/temperature"
#define MQTT_PUB_HUM_DHT11 "esp32/battMon/humidity"
#define MQTT_PUB_BATT_VOLT "esp32/battMon/battVolt"

uint32_t MQTT_CONNECTED = 0;

static batt_mon_t battMon = {
    .volt = 0,
    .temp = 0.0f,
    .humid = 0.0f,
    .msg = ""
};
esp_mqtt_client_handle_t client = NULL;

static void mqtt_app_start(void);
/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        MQTT_CONNECTED = 1;
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        MQTT_CONNECTED = 0;
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;        
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}


static void mqtt_app_start(void)
{
    ESP_LOGI(TAG, "Starting Mqtt");
    esp_mqtt_client_config_t mqttConfig = {
        //MUST GIVE IP OF THE COMPUTER RUN MQTT BROKER!!! DON'T USE LOCALHOST <-- WHICH IS ESP32 IP IN THIS CASE
        //If use MacOS must add:
        //allow_anonymous true
        //listener 1883
        //to: mosquitto.conf        
        .broker.address.uri = "mqtt://192.168.1.82"};

    client = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}

void mqtt_task(void *pvParameters){

    mqtt_app_start();
    char temp[12];
    char volt[12];
    char humid[12];
    while (1) {
        /* Wait for a message to arrive. */
        xQueueReceive(xQueueAdcBattVolt, &battMon.volt, portMAX_DELAY);
        xQueueReceive(xQueueDht11Humid, &battMon.humid, portMAX_DELAY);
        xQueueReceive(xQueueDht11Temp, &battMon.temp, portMAX_DELAY);
        sprintf(volt, "%.2f mV", battMon.volt);
        sprintf(humid, "%.2f %%", battMon.humid);
        sprintf(temp, "%.2f degC", battMon.temp);
        /* Take the mutex to ensure exclusive access to the queue. */
        xSemaphoreTake(xMutexAdcBattVolt, portMAX_DELAY);
        xSemaphoreTake(xMutexDht11Humid, portMAX_DELAY);
        xSemaphoreTake(xMutexDht11Temp, portMAX_DELAY);

        if (MQTT_CONNECTED)
        {
            esp_mqtt_client_publish(client, MQTT_PUB_TEMP_DHT11, temp, 0, 0, 0);
            esp_mqtt_client_publish(client, MQTT_PUB_HUM_DHT11, humid, 0, 0, 0);
            esp_mqtt_client_publish(client, MQTT_PUB_BATT_VOLT, volt, 0, 0, 0);
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
        /* Give back the mutex. */
        xSemaphoreGive(xMutexAdcBattVolt);
        xSemaphoreGive(xMutexDht11Humid);
        xSemaphoreGive(xMutexDht11Temp); 
    }
}