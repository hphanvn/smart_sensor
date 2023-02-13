#ifndef _TASK_OLED_DISPLAY_H
#define _TASK_OLED_DISPLAY_H
#include "ssd1306.h"

extern SSD1306_t oled;

void oled_task(void *pvParameter);
#endif