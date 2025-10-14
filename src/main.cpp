#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"
#include "time_manager.h"
#include "schedule_module.h"
#include "position_engine.h"
#include "display_manager.h"

// Global module instances
WiFiManager wifiManager;
TimeManager timeManager;
ScheduleModule scheduleModule;
PositionEngine positionEngine;
DisplayManager displayManager;

// Timing variables
unsigned long lastTrainUpdate = 0;
unsigned long lastDisplayUpdate = 0;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000);  // Wait for serial to stabilize

    Serial.println("========================================");
    Serial.println("Seattle Link Light Rail LED Display");
    Serial.println("Version 1.0 - Stub Implementation");
    Serial.println("========================================");
    Serial.println();
    Serial.println("Hello World!");
    Serial.println();

    // Initialize display manager first
    Serial.println("Initializing Display Manager...");
    displayManager.init();

    // Set all LEDs to pink (255, 105, 180)
    Serial.println("Setting all LEDs to PINK...");
    displayManager.setAllLEDs(255, 105, 180);

    // Initialize WiFi manager (stub)
    Serial.println("Initializing WiFi Manager...");
    wifiManager.init(WIFI_SSID, WIFI_PASSWORD);

    // Initialize time manager (stub)
    Serial.println("Initializing Time Manager...");
    timeManager.init();

    // Load schedule data (stub)
    Serial.println("Loading Schedule Module...");
    scheduleModule.loadSchedule();

    // Initialize position engine (stub)
    Serial.println("Initializing Position Engine...");
    positionEngine.init(&scheduleModule);

    Serial.println();
    Serial.println("========================================");
    Serial.println("Initialization Complete!");
    Serial.println("All LEDs should now be PINK");
    Serial.println("========================================");
    Serial.println();
}

void loop() {
    // WiFi maintenance (non-blocking)
    wifiManager.maintain();

    // Time synchronization (non-blocking check)
    timeManager.update();

    // Update train positions (every 1 second)
    if (millis() - lastTrainUpdate > TRAIN_UPDATE_INTERVAL) {
        time_t now = timeManager.getCurrentTime();
        positionEngine.updateAllTrains(now);
        lastTrainUpdate = millis();
    }

    // Update display (every ~33ms for 30fps)
    if (millis() - lastDisplayUpdate > (1000 / FRAME_RATE)) {
        displayManager.updateDisplay();
        lastDisplayUpdate = millis();
    }

    // Small delay to prevent watchdog issues
    delay(1);
}
