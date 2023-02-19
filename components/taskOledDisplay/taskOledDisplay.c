#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"

#include "taskOledDisplay.h"
#include "taskStmp.h"
#include "ssd1306.h"
#include "main.h"

//#include "font8x8_basic.h"

static const char *TAG_OLED = "oled_ssd1315";
static char *emptyLine = "                ";
SSD1306_t oled;

static batt_mon_t battMon = {
    .volt = 0,
    .temp = 0.0f,
    .humid = 0.0f,
    .msg = ""
};

bool createOnce = false;

void oled_task(void *pvParameter){
   /*Init OLED display*/
#if CONFIG_I2C_INTERFACE
	ESP_LOGI(TAG_OLED, "INTERFACE is i2c");
	ESP_LOGI(TAG_OLED, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(TAG_OLED, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(TAG_OLED, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&oled, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE
#if CONFIG_FLIP
    oled._flip = true;
	ESP_LOGW(TAG_OLED, "Flip upside down");
#endif

#if CONFIG_SSD1306_128x64
	ESP_LOGI(TAG_OLED, "Panel is 128x64");
	ssd1306_init(&oled, 128, 64);
#endif // CONFIG_SSD1306_128x64
#if CONFIG_SSD1306_128x32
	ESP_LOGI(TAG_OLED, "Panel is 128x32");
	ssd1306_init(&oled, 128, 32);
#endif // CONFIG_SSD1306_128x32

    ssd1306_clear_screen(&oled, false);
	ssd1306_contrast(&oled, 0xff);

    state_t state = STATE_INIT;

    while (1) {
        /* Wait for a message to arrive. */
        xQueueReceive(xQueueAdcBattVolt, &battMon.volt, portMAX_DELAY);
        xQueueReceive(xQueueDht11Humid, &battMon.humid, portMAX_DELAY);
        xQueueReceive(xQueueDht11Temp, &battMon.temp, portMAX_DELAY);
       
        /* Take the mutex to ensure exclusive access to the queue. */
        xSemaphoreTake(xMutexAdcBattVolt, portMAX_DELAY);
        xSemaphoreTake(xMutexDht11Humid, portMAX_DELAY);
        xSemaphoreTake(xMutexDht11Temp, portMAX_DELAY);

        if ( battMon.volt > 12000.0f ){
            state = STATE_WARNING__BATT_TOO_HIGH;
        }
        else {
             state = STATE_DISPLAY__ADC;
        }

        switch (state){
            case STATE_INIT:
                printSreenTextCenter(&oled, 0, emptyLine);
                printSreenTextCenter(&oled, 1, emptyLine);
                printSreenTextCenter(&oled, 2, emptyLine);
                printSreenTextCenter(&oled, 3, "Initializing...");
                printSreenTextCenter(&oled, 4, "Please wait!");
                printSreenTextCenter(&oled, 5, emptyLine);
                printSreenTextCenter(&oled, 6, emptyLine);
                printSreenTextCenter(&oled, 7, emptyLine);
                vTaskDelay(10 / portTICK_PERIOD_MS);
                break;
            
            case STATE_DISPLAY__ADC:
                printSreenTextCenter(&oled, 0, "BATTERY MONITOR");
                printSreenTextCenter(&oled, 1, emptyLine);
                printSreenTextCenter(&oled, 2, emptyLine);
                printSreenFloat(&oled, 0, 3, "Batt. = ", THREE_DIGITS, battMon.volt, "mV");
                printSreenFloat(&oled, 0, 4, "RH    = ", THREE_DIGITS, battMon.humid, "%");
                printSreenFloat(&oled, 0, 5, "Temp. = ", THREE_DIGITS, battMon.temp, "C");
                printSreenTextCenter(&oled, 6, emptyLine);
                printSreenTextCenter(&oled, 7, emptyLine);
                vTaskDelay(10 / portTICK_PERIOD_MS);
                break;

            case STATE_WARNING__BATT_TOO_HIGH:
                printSreenTextCenter(&oled, 0, "WARNING !!!");
                printSreenTextCenter(&oled, 1, emptyLine);
                printSreenTextCenter(&oled, 2, "BATTERY VOLTAGE");
                printSreenTextCenter(&oled, 3, "TOO HIGH!");
                printSreenTextCenter(&oled, 4, emptyLine);
                printSreenFloat(&oled, 0, 5, "Batt. = ", THREE_DIGITS, battMon.volt, "(mV)");
                printSreenTextCenter(&oled, 6, emptyLine);
                printSreenTextCenter(&oled, 7, emptyLine);
                strcpy( battMon.msg, "BATTERY VOLTAGE TOO HIGH");
                if (!createOnce){
                    xTaskCreate(&smtp_client_task, "smtp_client_task__batt_too_high",
                             TASK_STACK_SIZE , (void*) &battMon, 6, NULL);
                    createOnce = true;
                }
                vTaskDelay(10 / portTICK_PERIOD_MS);
                break;

            default:
                clearSreen(&oled);
                break;
        }

        /* Give back the mutex. */
        xSemaphoreGive(xMutexAdcBattVolt);
        xSemaphoreGive(xMutexDht11Humid);
        xSemaphoreGive(xMutexDht11Temp);

    }
    



}