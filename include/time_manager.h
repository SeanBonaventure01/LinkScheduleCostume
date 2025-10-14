#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include <time.h>

/**
 * Time Manager
 * Maintains accurate system time via NTP and RTC fallback
 */
class TimeManager {
public:
    TimeManager();

    /**
     * Initialize time manager
     */
    void init();

    /**
     * Synchronize time with NTP server
     * @return true if sync successful, false otherwise
     */
    bool syncNTP();

    /**
     * Get current time
     * @return current time as time_t
     */
    time_t getCurrentTime();

    /**
     * Check if time has been synced
     * @return true if synced, false otherwise
     */
    bool isTimeSynced();

    /**
     * Get seconds since last sync
     * @return seconds since last NTP sync
     */
    uint32_t getSecondsSinceSync();

    /**
     * Update internal clock tracking (call in loop)
     */
    void update();

private:
    time_t lastSyncTime_;
    uint32_t lastSyncMillis_;
    bool isSynced_;
    int timezoneOffset_;
};

#endif // TIME_MANAGER_H
