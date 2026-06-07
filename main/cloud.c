#include "cloud.h"
#include "config.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>
#include <stdio.h>

static const char* TAG = "cloud";
static int64_t last_upload_ms = 0;
static int upload_ok = 0;
static int upload_fail = 0;

void cloud_init(void) {
    last_upload_ms = 0;
    upload_ok = 0;
    upload_fail = 0;
    ESP_LOGI(TAG, "Cloud module initialized (URL %s)",
             strlen(CLOUD_URL) > 0 ? "configured" : "EMPTY - cloud disabled");
}

void cloud_upload(log_entry_t *entry) {
    if (!entry) return;

    if (strlen(CLOUD_URL) == 0) {
        // Cloud disabled, silently skip
        return;
    }

    int64_t now_ms = esp_timer_get_time() / 1000;
    if (!cloud_should_upload(now_ms, last_upload_ms, CLOUD_UPLOAD_MS)) {
        return;  // Not yet time
    }

    char body[256];
    int len = cloud_format_json(entry, body, sizeof(body));
    if (len <= 0) {
        upload_fail++;
        ESP_LOGW(TAG, "JSON format failed");
        return;
    }

    esp_http_client_config_t cfg = {
        .url = CLOUD_URL,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 8000,
        .keep_alive_enable = false,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (!client) {
        upload_fail++;
        ESP_LOGW(TAG, "HTTP client init failed");
        return;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, body, len);

    esp_err_t err = esp_http_client_perform(client);
    int status = esp_http_client_get_status_code(client);
    esp_http_client_cleanup(client);

    if (err == ESP_OK && status == 200) {
        upload_ok++;
        last_upload_ms = now_ms;
        ESP_LOGD(TAG, "Upload OK: temp=%.1f fan=%u%%",
                 entry->temp_c, (unsigned)(entry->fan_pwm * 100 / PWM_MAX));
    } else {
        upload_fail++;
        ESP_LOGW(TAG, "Upload failed: err=%s status=%d",
                 esp_err_to_name(err), status);
    }
}

bool cloud_is_online(void) {
    return upload_ok > 0 && (upload_ok + upload_fail) > 0
           && (upload_ok * 2 > upload_fail);
}

int64_t cloud_last_upload_ms(void) { return last_upload_ms; }
int cloud_get_ok_count(void) { return upload_ok; }
int cloud_get_fail_count(void) { return upload_fail; }
