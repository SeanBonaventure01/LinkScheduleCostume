#include "time_manager.h"
#include "config.h"

TimeManager::TimeManager()
    : lastSyncTime_(0),
      lastSyncMillis_(0),
      isSynced_(false),
      timezoneOffset_(TIMEZONE_OFFSET_SECONDS) {
}

void TimeManager::init() {
    // Set initial time to Monday, October 20, 2025 at 8:00 AM PST
    struct tm timeinfo;
    timeinfo.tm_year = 2025 - 1900;  // Years since 1900
    timeinfo.tm_mon = 10 - 1;        // Month (0-11, so October = 9)
    timeinfo.tm_mday = 20;           // Day of month
    timeinfo.tm_hour = 8;            // 8 AM
    timeinfo.tm_min = 0;
    timeinfo.tm_sec = 0;
    timeinfo.tm_isdst = 0;           // No DST for Pacific Standard Time in October

    // Convert to time_t
    lastSyncTime_ = mktime(&timeinfo);
    lastSyncMillis_ = millis();
    isSynced_ = true;

    Serial.println("[TimeManager] Initialized with time: Monday, October 20, 2025 08:00:00 PST");
    Serial.print("[TimeManager] Unix timestamp: ");
    Serial.println(lastSyncTime_);
}

bool TimeManager::syncNTP() {
    // Not implemented - using internal clock only
    Serial.println("[TimeManager] syncNTP() - not implemented (using internal clock)");
    return false;
}

time_t TimeManager::getCurrentTime() {
    if (!isSynced_) {
        return 0;
    }

    // Calculate elapsed time since last sync using millis()
    unsigned long currentMillis = millis();
    unsigned long elapsedMillis;

    // Handle millis() rollover (occurs after ~49.7 days)
    if (currentMillis >= lastSyncMillis_) {
        elapsedMillis = currentMillis - lastSyncMillis_;
    } else {
        // Rollover occurred
        elapsedMillis = (0xFFFFFFFF - lastSyncMillis_) + currentMillis + 1;
    }

    // Convert milliseconds to seconds
    uint32_t elapsedSeconds = elapsedMillis / 1000;

    // Return current time
    return lastSyncTime_ + elapsedSeconds;
}

bool TimeManager::isTimeSynced() {
    return isSynced_;
}

uint32_t TimeManager::getSecondsSinceSync() {
    if (!isSynced_) {
        return 0;
    }

    unsigned long currentMillis = millis();
    unsigned long elapsedMillis;

    // Handle millis() rollover
    if (currentMillis >= lastSyncMillis_) {
        elapsedMillis = currentMillis - lastSyncMillis_;
    } else {
        // Rollover occurred
        elapsedMillis = (0xFFFFFFFF - lastSyncMillis_) + currentMillis + 1;
    }

    return elapsedMillis / 1000;
}

void TimeManager::update() {
    // For now, no periodic updates needed
    // In a real implementation, this would check for NTP sync interval
}
