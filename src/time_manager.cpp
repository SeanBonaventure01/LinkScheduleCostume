#include "time_manager.h"
#include "config.h"
#include <WiFi.h>

TimeManager::TimeManager()
    : lastSyncTime_(0),
      lastSyncMillis_(0),
      isSynced_(false),
      timezoneOffset_(TIMEZONE_OFFSET_SECONDS) {
}

void TimeManager::init() {
    Serial.println("[TimeManager] Initializing...");

    // Scan for available networks
    Serial.println("[TimeManager] Scanning for WiFi networks...");
    WiFi.mode(WIFI_STA);
    int n = WiFi.scanNetworks();

    if (n == 0) {
        Serial.println("[TimeManager] No networks found");
    } else {
        Serial.print("[TimeManager] Found ");
        Serial.print(n);
        Serial.println(" networks:");
        for (int i = 0; i < n; i++) {
            Serial.print("  ");
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(" dBm) ");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Encrypted");
        }
    }

    // Attempt WiFi connection
    Serial.print("[TimeManager] Connecting to WiFi SSID: '");
    Serial.print(WIFI_SSID);
    Serial.println("'");

    // Disconnect any previous connection
    WiFi.disconnect(true);
    delay(100);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long startAttempt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < WIFI_CONNECT_TIMEOUT) {
        delay(500);
        Serial.print(".");
        wl_status_t status = WiFi.status();
        if (status == WL_CONNECT_FAILED || status == WL_NO_SSID_AVAIL) {
            Serial.println();
            Serial.print("[TimeManager] WiFi connection failed early. Status: ");
            Serial.println(status);
            break;
        }
    }
    Serial.println();

    // Try NTP sync if WiFi connected
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[TimeManager] WiFi connected!");
        Serial.print("[TimeManager] IP address: ");
        Serial.println(WiFi.localIP());

        if (syncNTP()) {
            Serial.println("[TimeManager] NTP sync successful");
            return;
        }
    } else {
        Serial.print("[TimeManager] WiFi connection failed. Status code: ");
        Serial.println(WiFi.status());
        Serial.println("[TimeManager] Status meanings: 0=IDLE, 1=NO_SSID, 3=CONNECTED, 4=CONNECT_FAILED, 6=DISCONNECTED");
    }

    // Fallback to 8 AM default time
    Serial.println("[TimeManager] Using fallback time: 8:00 AM");
    setDefaultTime();
}

void TimeManager::setDefaultTime() {
    // Set to 8:00 AM today
    struct tm timeinfo;
    time_t now = time(nullptr);
    localtime_r(&now, &timeinfo);

    timeinfo.tm_hour = 8;
    timeinfo.tm_min = 0;
    timeinfo.tm_sec = 0;

    lastSyncTime_ = mktime(&timeinfo);
    lastSyncMillis_ = millis();
    isSynced_ = true;

    Serial.print("[TimeManager] Fallback time set to: ");
    Serial.println(asctime(&timeinfo));
}

bool TimeManager::syncNTP() {
    Serial.println("[TimeManager] Attempting NTP sync...");
    Serial.print("[TimeManager] NTP server: ");
    Serial.println(NTP_SERVER);

    // Configure NTP with Pacific timezone offset
    configTime(TIMEZONE_OFFSET_SECONDS, 0, NTP_SERVER);

    // Wait for time to be set (up to 15 seconds)
    int retry = 0;
    const int maxRetries = 150;  // 150 * 100ms = 15 seconds

    while (retry < maxRetries) {
        time_t now = time(nullptr);

        // Print progress every 2 seconds
        if (retry % 20 == 0) {
            Serial.print("[TimeManager] Waiting for NTP... (");
            Serial.print(retry / 10);
            Serial.print("s, timestamp: ");
            Serial.print(now);
            Serial.println(")");
        }

        if (now > 1000000000) {  // Valid timestamp (after year 2001)
            lastSyncTime_ = now;
            lastSyncMillis_ = millis();
            isSynced_ = true;

            struct tm timeinfo;
            localtime_r(&now, &timeinfo);
            Serial.print("[TimeManager] NTP time synced: ");
            Serial.println(asctime(&timeinfo));

            return true;
        }
        delay(100);
        retry++;
    }

    Serial.println("[TimeManager] NTP sync timeout after 15 seconds");
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
