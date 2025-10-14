#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

/**
 * WiFi Manager
 * Handles WiFi connectivity and maintains connection
 */
class WiFiManager {
public:
    WiFiManager();

    /**
     * Initialize WiFi connection
     * @param ssid WiFi network name
     * @param password WiFi password
     */
    void init(const char* ssid, const char* password);

    /**
     * Check if WiFi is connected
     * @return true if connected, false otherwise
     */
    bool isConnected();

    /**
     * Maintain WiFi connection (call in loop)
     * Attempts reconnection if disconnected
     */
    void maintain();

private:
    const char* ssid_;
    const char* password_;
    unsigned long lastReconnectAttempt_;
};

#endif // WIFI_MANAGER_H
