#ifndef _TASK_SERVER_H_
#define _TASK_SERVER_H_

#include "esp_http_server.h"
#define LED_PIN GPIO_NUM_17

esp_err_t send_web_page(httpd_req_t *req);
esp_err_t get_req_handler(httpd_req_t *req);
esp_err_t led_on_handler(httpd_req_t *req);
esp_err_t led_off_handler(httpd_req_t *req);
httpd_handle_t setup_server(void);

#endif