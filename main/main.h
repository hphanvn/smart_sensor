#ifndef _MAIN_H_
#define _MAIN_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define TASK_STACK_SIZE 1024*8
typedef enum {
    STATE_INIT =0,
    STATE_DISPLAY__ADC,
    STATE_DISPLAY__OTA,
    STATE_WARNING__BATT_TOO_LOW,
    STATE_WARNING__BATT_TOO_HIGH,
    STATE_WARNING__BATT_CHARGING,
    MAX_STATE
}state_t;

#define QUEUE_ADC_LEN__BATT_VOLT 10
#define QUEUE_ADC_SIZE__BATT_VOLT sizeof(int)

#define QUEUE_DHT11_LEN__TEMP 10
#define QUEUE_DHT11_SIZE__TEMP sizeof(int)

#define QUEUE_DHT11_LEN__HUMI 10
#define QUEUE_DHT11_SIZE__HUMI sizeof(float)

extern QueueHandle_t xQueueAdcBattVolt;
extern SemaphoreHandle_t xMutexAdcBattVolt;

extern QueueHandle_t xQueueDht11Humid;
extern SemaphoreHandle_t xMutexDht11Humid;

extern QueueHandle_t xQueueDht11Temp;
extern SemaphoreHandle_t xMutexDht11Temp;

typedef struct {
    float volt;
    float temp;
    float humid;
    char msg[50];
}batt_mon_t;

#endif