// ESP32 Server with static HTML page

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "my_data.h"

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;

    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;

    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;

	case IP_EVENT_STA_GOT_IP:
		printf("WiFi got IP ... \n\n");
		break;

    default:
        break;
    }
}

void wifi_connection()
{
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init(); // TCP/IP initiation 					s1.1
	esp_event_loop_create_default(); // event loop 			s1.2
	esp_netif_create_default_wifi_sta(); // WiFi station 	s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					s1.4
	
	// 2 - Wi-Fi Configuration Phase
	esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);

	// 3 - Wi-Fi Start Phase
    esp_wifi_start();

	// 4- Wi-Fi Connect Phase
	esp_wifi_connect();
}

static esp_err_t get_handler(httpd_req_t *req)
{
    char *response_message = "<!DOCTYPE HTML><html><head>\
                                <title>Static HTML page</title>\
                                <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
                                </head><body>\
                                <h1>This is static HTML page</h1>\
                                </form><br>\
                                </body></html>";
    httpd_resp_send(req, response_message, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

void server_initiation()
{
    httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server_handle = NULL;
    httpd_start(&server_handle, &server_config);

    httpd_uri_t uri_get = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = get_handler,
        .user_ctx = NULL};
    httpd_register_uri_handler(server_handle, &uri_get);
}

void app_main(void)
{
    nvs_flash_init();   
	wifi_connection();  
	server_initiation();
}
