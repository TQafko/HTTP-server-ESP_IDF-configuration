/*
By: Tedi Qafko
Date: 1/28/2024
Description: Float Server SoftAP server in ESP32 using RTOS and 
EPS-IDF applicationto recieve get and post requests via the HTTP protocol.
*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include "esp_tls_crypto.h"
#include "esp_tls.h"
#include "esp_check.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <esp_system.h>
#include "esp_eth.h"
#include "driver/gpio.h"

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN

#define LED 4

static const char *TAG = "Float Server";

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                    .required = false,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

static void configure_led (void){
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

static esp_err_t ledON_handler(httpd_req_t *req)
{
    esp_err_t error;
    gpio_set_level(LED, 1);
    ESP_LOGI(TAG, "LED Turned ON");
    const char *response = (const char *) req->user_ctx;
    error = httpd_resp_send(req, response, strlen(response));
    if (error != ESP_OK){
        ESP_LOGI(TAG, "Error %d while sending Response", error);
    } else {
        ESP_LOGI(TAG, "Response sent Successfully");
    }
    return error;
}

static const httpd_uri_t ledon = {
    .uri       = "/ledon",
    .method    = HTTP_GET,
    .handler   = ledON_handler,
    .user_ctx  = " <!DOCTYPE html>\
<html>\
<head>\
<style>\
.button {\
  border: none;\
  color: white;\
  padding: 15px 32px;\
  text-align: center;\
  text-decoration: none;\
  display: inline-block;\
  font-size: 16px;\
  margin: 4px 2px;\
  cursor: pointer;\
}\
.button1 {background-color: #000000;}\
</style>\
</head>\
<body>\
<h1>WUROV Float WebServer</h1>\
<p>Toggle the onboard LED (GPIO4)</p>\
<h3>LED State: ON</h3>\
<button class=\"button button1\" onclick=\"window.location.href=\'/ledoff\'\">LED OFF</button>\
</body>\
</html>\
 "
};

static esp_err_t ledOFF_handler(httpd_req_t *req)
{
    esp_err_t error;
    gpio_set_level(LED, 0);
    ESP_LOGI(TAG, "LED Turned OFF");
    const char *response = (const char *) req->user_ctx;
    error = httpd_resp_send(req, response, strlen(response));
    if (error != ESP_OK){
        ESP_LOGI(TAG, "Error %d while sending Response", error);
    } else {
        ESP_LOGI(TAG, "Response sent Successfully");
    }
    return error;
}

static const httpd_uri_t ledoff = {
    .uri       = "/ledoff",
    .method    = HTTP_GET,
    .handler   = ledOFF_handler,
    .user_ctx  = " <!DOCTYPE html>\
<html>\
<head>\
<style>\
.button {\
  border: none;\
  color: white;\
  padding: 15px 32px;\
  text-align: center;\
  text-decoration: none;\
  display: inline-block;\
  font-size: 16px;\
  margin: 4px 2px;\
  cursor: pointer;\
}\
.button1 {background-color: #04AA6D;} /* Green */\
</style>\
</head>\
<body>\
<h1>WUROV Float WebServer</h1>\
<p>Toggle the onboard LED (GPIO4)</p>\
<h3>LED State: OFF</h3>\
<button class=\"button button1\" onclick=\"window.location.href=\'/ledon\'\">LED ON</button>\
</body>\
</html>\
 "
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &ledoff);
        httpd_register_uri_handler(server, &ledon);
        return server;
    }
    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    return httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void app_main(void)
{
    static httpd_handle_t server = NULL;
    configure_led();
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");

    wifi_init_softap();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
    
}