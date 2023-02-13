#ifndef _MAIN_H_
#define _MAIN_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

typedef enum {
    STATE_INIT =0,
    STATE_DISPLAY__ADC,
    STATE_DISPLAY__OTA,
    MAX_STATE
}state_t;

#define QUEUE_ADC_LEN__BATT_VOLT 10
#define QUEUE_ADC_SIZE__BATT_VOLT sizeof(int)

extern QueueHandle_t xQueueAdcBattVolt;
extern SemaphoreHandle_t xMutexAdcBattVolt;

#endif