#include "ntp_time.h"
#include "../hal/prefs.h"
#include <time.h>
#include <Arduino.h>

static bool time_valid = false;
static char display_buf[20];

void ntp_init() {
    // Get saved timezone offset (-12 to +14)
    int8_t tz_offset = prefs_get_timezone();
    
    // Configure NTP servers
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    
    // Build POSIX TZ string for the timezone
    // Format: "UTC<offset>" where offset is inverted (UTC-3 becomes UTC+3 in POSIX)
    char tz_str[16];
    if (tz_offset >= 0) {
        snprintf(tz_str, sizeof(tz_str), "UTC-%d", tz_offset);
    } else {
        snprintf(tz_str, sizeof(tz_str), "UTC+%d", -tz_offset);
    }
    
    setenv("TZ", tz_str, 1);
    tzset();
    
    Serial.printf("[NTP] Time sync started (UTC%+d)\n", tz_offset);
}

bool ntp_valid() {
    if (time_valid) return true;
    struct tm t;
    if (getLocalTime(&t, 0) && t.tm_year > 100) {
        time_valid = true;
        Serial.println("[NTP] Time synchronized");
    }
    return time_valid;
}

const char* ntp_get_display_str() {
    struct tm t;
    if (!getLocalTime(&t, 0)) {
        return "";
    }
    int hr = t.tm_hour % 12;
    if (hr == 0) hr = 12;
    const char* ampm = t.tm_hour >= 12 ? "PM" : "AM";
    static const char* months[] = {
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
    };
    snprintf(display_buf, sizeof(display_buf), "%s %d %d:%02d %s",
             months[t.tm_mon], t.tm_mday, hr, t.tm_min, ampm);
    return display_buf;
}
