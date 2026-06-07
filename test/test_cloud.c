// test/test_cloud.c
// Host-compilable unit tests for cloud module's testable helpers.
// Compile with: gcc -I main -o /tmp/test_cloud test/test_cloud.c
// Run: /tmp/test_cloud

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cloud.h"
#include "config.h"

static int tests_run = 0;
static int tests_passed = 0;

#define ASSERT(cond, msg) do { \
    tests_run++; \
    if (!(cond)) { \
        printf("FAIL: %s (line %d): %s\n", __func__, __LINE__, msg); \
        return 0; \
    } \
    tests_passed++; \
} while(0)

static int test_json_basic(void) {
    log_entry_t e = {
        .sample = 42,
        .timestamp_ms = 8400,
        .temp_c = 28.5f,
        .fan_pwm = 80,   // 80*100/255 = 31%
        .mode = "AUTO",
        .setpoint = 30.0f,
    };
    char buf[256];
    int n = cloud_format_json(&e, buf, sizeof(buf));
    ASSERT(n > 0, "format returned 0");
    ASSERT(strstr(buf, "\"sample\":42") != NULL, "sample field missing");
    ASSERT(strstr(buf, "\"temp\":28.5") != NULL, "temp field missing");
    ASSERT(strstr(buf, "\"fan\":31") != NULL, "fan percent field missing");
    ASSERT(strstr(buf, "\"mode\":\"AUTO\"") != NULL, "mode field missing");
    ASSERT(strstr(buf, "\"setpoint\":30.0") != NULL, "setpoint field missing");
    ASSERT(buf[0] == '{', "should start with {");
    ASSERT(buf[strlen(buf) - 1] == '}', "should end with }");
    return 1;
}

static int test_json_manual_mode(void) {
    log_entry_t e = {
        .sample = 100,
        .timestamp_ms = 20000,
        .temp_c = 25.0f,
        .fan_pwm = 255,  // 100%
        .mode = "MANUAL",
        .setpoint = 30.0f,
    };
    char buf[256];
    cloud_format_json(&e, buf, sizeof(buf));
    ASSERT(strstr(buf, "\"fan\":100") != NULL, "100% fan should be 100");
    ASSERT(strstr(buf, "\"mode\":\"MANUAL\"") != NULL, "manual mode missing");
    return 1;
}

static int test_json_null_safety(void) {
    char buf[256];
    int n = cloud_format_json(NULL, buf, sizeof(buf));
    ASSERT(n == 0, "null entry should return 0");
    n = cloud_format_json(&(log_entry_t){0}, NULL, 256);
    ASSERT(n == 0, "null buf should return 0");
    return 1;
}

static int test_json_small_buffer(void) {
    log_entry_t e = { .sample = 1, .temp_c = 25.0f, .fan_pwm = 0, .mode = "AUTO", .setpoint = 30.0f };
    char buf[5];
    int n = cloud_format_json(&e, buf, sizeof(buf));
    // snprintf returns the would-be length; just ensure no crash and is null-terminated
    ASSERT(n > 0, "should return would-be length");
    ASSERT(strlen(buf) <= 4, "truncated buffer should be null-terminated");
    return 1;
}

static int test_should_upload_first_time(void) {
    // last_ms=0 means "never uploaded", first interval should fire
    ASSERT(cloud_should_upload(35000, 0, 30000) == true, "first upload should fire");
    return 1;
}

static int test_should_upload_just_done(void) {
    // Just uploaded, interval not elapsed
    ASSERT(cloud_should_upload(35000, 35000, 30000) == false, "should not upload immediately");
    return 1;
}

static int test_should_upload_interval_elapsed(void) {
    // Long enough since last
    ASSERT(cloud_should_upload(65000, 35000, 30000) == true, "should upload after interval");
    return 1;
}

static int test_should_upload_not_yet(void) {
    // Not yet elapsed
    ASSERT(cloud_should_upload(50000, 35000, 30000) == false, "should not upload too soon");
    return 1;
}

static int test_should_upload_just_at_threshold(void) {
    // Exactly at interval
    ASSERT(cloud_should_upload(65000, 35000, 30000) == true, "should upload at exactly interval");
    return 1;
}

int main(void) {
    printf("Running cloud helper tests...\n");
    test_json_basic();
    test_json_manual_mode();
    test_json_null_safety();
    test_json_small_buffer();
    test_should_upload_first_time();
    test_should_upload_just_done();
    test_should_upload_interval_elapsed();
    test_should_upload_not_yet();
    test_should_upload_just_at_threshold();
    printf("\nResults: %d/%d assertions passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
