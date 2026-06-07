#include "network.h"
#include "config.h"
#include "dashboard.h"
#include "actuator.h"
#include "storage.h"
#include "cloud.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <string.h>
#include <stdlib.h>

static const char* TAG = "network";
static httpd_handle_t server = NULL;
static bool sta_connected = false;
static bool ap_active = false;

extern float g_temp_c;
extern uint8_t g_fan_pwm;
extern int g_mode;
extern float g_setpoint;

static esp_err_t root_handler(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char*)dashboard_html, dashboard_html_len);
    return ESP_OK;
}

static esp_err_t api_status_handler(httpd_req_t* req) {
    char buf[384];
    int64_t uptime_us = esp_timer_get_time();
    int64_t cloud_age_s = (cloud_last_upload_ms() == 0) ? -1
        : (int64_t)((uptime_us / 1000 - cloud_last_upload_ms()) / 1000);
    snprintf(buf, sizeof(buf),
        "{\"temp\":%.1f,\"fan\":%u,\"mode\":%d,\"setpoint\":%.1f,"
        "\"wifi\":\"%s\",\"uptime\":%lld,"
        "\"cloud\":{\"ok\":%d,\"fail\":%d,\"age_s\":%lld}}",
        g_temp_c, g_fan_pwm * 100 / PWM_MAX, g_mode, g_setpoint,
        network_get_mode_str(), (long long)(uptime_us / 1000000),
        cloud_get_ok_count(), cloud_get_fail_count(),
        (long long)cloud_age_s);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, buf, strlen(buf));
    return ESP_OK;
}

static esp_err_t api_control_handler(httpd_req_t* req) {
    char buf[128];
    int len = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (len <= 0) return ESP_FAIL;
    buf[len] = 0;

    // Handle Mode explicitly
    if (strstr(buf, "\"mode\":0")) { 
        g_mode = 0; 
        actuator_set_mode(MODE_AUTO); 
    }
    if (strstr(buf, "\"mode\":1")) { 
        g_mode = 1; 
        actuator_set_mode(MODE_MANUAL); 
    }

    // Handle Setpoint
    char* sp = strstr(buf, "\"setpoint\":");
    if (sp) { 
        g_setpoint = atof(sp + 11); 
        actuator_set_setpoint(g_setpoint); 
    }

    // Handle Fan Speed
    char* fp = strstr(buf, "\"fan\":");
    if (fp) {
        int pct = atoi(fp + 6);
        if (pct > 100) pct = 100;
        if (pct < 0) pct = 0;
        g_fan_pwm = (uint8_t)(pct * PWM_MAX / 100);
        actuator_set_pwm(g_fan_pwm);
        
        // FIX: If the user changes the fan speed, force MANUAL mode!
        g_mode = 1; 
        actuator_set_mode(MODE_MANUAL);
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, "{\"ok\":true}", 11);
    return ESP_OK;
}

static esp_err_t api_log_csv_handler(httpd_req_t* req) {
    char* csv = malloc(8192); // Increased to prevent truncation
    if (!csv) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    int len = storage_get_csv(csv, 8192); // Update size here too
    httpd_resp_set_type(req, "text/csv");
    httpd_resp_set_hdr(req, "Content-Disposition", "attachment; filename=log.csv");
    httpd_resp_send(req, csv, len);
    free(csv);
    return ESP_OK;
}

static esp_err_t api_log_plt_handler(httpd_req_t* req) {
    char buf[1024];
    int len = storage_get_plt(buf, sizeof(buf));
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_hdr(req, "Content-Disposition", "attachment; filename=plot.plt");
    httpd_resp_send(req, buf, len);
    return ESP_OK;
}

static esp_err_t api_log_clear_handler(httpd_req_t* req) {
    storage_clear();
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, "{\"ok\":true}", 11);
    return ESP_OK;
}

static void wifi_event_handler(void* arg, esp_event_base_t base,
                                int32_t id, void* data) {
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        sta_connected = false;
        ESP_LOGI(TAG, "STA disconnected, will retry...");
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*)data;
        sta_connected = true;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

static void start_ap(void) {
    ap_active = true;
    wifi_config_t wifi_ap_cfg = {
        .ap = {
            .ssid = WIFI_AP_SSID,
            .password = WIFI_AP_PASS,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };
    esp_wifi_set_mode(WIFI_MODE_APSTA);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_cfg);
    ESP_LOGI(TAG, "AP started: %s", WIFI_AP_SSID);
}

static void start_http_server(void) {
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.max_open_sockets = 5;
    cfg.lru_purge_enable = true;
    httpd_start(&server, &cfg);

    httpd_uri_t root_uri = { "/", HTTP_GET, root_handler, NULL };
    httpd_uri_t status_uri = { "/api/status", HTTP_GET, api_status_handler, NULL };
    httpd_uri_t control_uri = { "/api/control", HTTP_POST, api_control_handler, NULL };
    httpd_uri_t csv_uri = { "/api/log/csv", HTTP_GET, api_log_csv_handler, NULL };
    httpd_uri_t plt_uri = { "/api/log/plt", HTTP_GET, api_log_plt_handler, NULL };
    httpd_uri_t clear_uri = { "/api/log/clear", HTTP_POST, api_log_clear_handler, NULL };

    httpd_register_uri_handler(server, &root_uri);
    httpd_register_uri_handler(server, &status_uri);
    httpd_register_uri_handler(server, &control_uri);
    httpd_register_uri_handler(server, &csv_uri);
    httpd_register_uri_handler(server, &plt_uri);
    httpd_register_uri_handler(server, &clear_uri);

    ESP_LOGI(TAG, "HTTP server started");
}

void network_init(void) {
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL);

    esp_wifi_set_mode(WIFI_MODE_APSTA);
    wifi_config_t wifi_sta_cfg = {
        .sta = {
            .ssid = WIFI_STA_SSID,
            .password = WIFI_STA_PASS,
        },
    };
    esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_cfg);
    esp_wifi_start();

    int waited = 0;
    while (waited < WIFI_TIMEOUT_MS && !sta_connected) {
        vTaskDelay(pdMS_TO_TICKS(100));
        waited += 100;
    }

    if (!sta_connected) start_ap();

    start_http_server();
}

void network_poll(void) {
    static int64_t last_check = 0;
    int64_t now = esp_timer_get_time() / 1000;
    if (!sta_connected && (now - last_check > WIFI_RECONNECT_MS)) {
        esp_wifi_connect();
        last_check = now;
    }
}

bool network_is_connected(void) { return sta_connected || ap_active; }
bool network_is_sta_connected(void) { return sta_connected; }
const char* network_get_mode_str(void) {
    if (sta_connected) return "STA";
    if (ap_active) return "AP";
    return "OFF";
}