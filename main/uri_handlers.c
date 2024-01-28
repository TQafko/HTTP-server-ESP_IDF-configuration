#include "pin_diagrams.c"

static esp_err_t root_handler(httpd_req_t *req)
{
    esp_err_t error;
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

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_handler,
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
<h3>LED State: OFF</h3>\
<button class=\"button button1\" onclick=\"window.location.href=\'/ledon\'\">LED ON</button>\
</body>\
</html>\
 "
};

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