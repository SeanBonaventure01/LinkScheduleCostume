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
unsigned long lastStatusPrint = 0;

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000);  // Wait for serial to stabilize

    Serial.println("========================================");
    Serial.println("Seattle Link Light Rail LED Display");
    Serial.println("Version 1.0 - Full Integration");
    Serial.println("========================================");
    Serial.println();

    // Initialize time manager
    Serial.println("Initializing Time Manager...");
    timeManager.init();
    Serial.println();

    // Load schedule data
    Serial.println("Loading Schedule Module...");
    scheduleModule.loadSchedule();
    Serial.println();

    // Initialize position engine
    Serial.println("Initializing Position Engine...");
    positionEngine.init(&scheduleModule);
    Serial.println();

    // Initialize display manager
    Serial.println("Initializing Display Manager...");
    displayManager.init();
    Serial.println();

    // Do initial train update to spawn trains for 8am
    time_t currentTime = timeManager.getCurrentTime();
    Serial.print("Current time: ");
    Serial.println(currentTime);
    positionEngine.updateAllTrains(currentTime);

    // Get initial train count
    uint8_t trainCount = 0;
    const TrainPosition* trains = positionEngine.getActiveTrainPositions(&trainCount);
    Serial.print("Initial active trains: ");
    Serial.println(trainCount);
    Serial.println();

    Serial.println("========================================");
    Serial.println("Initialization Complete!");
    Serial.println("LED display should now show:");
    Serial.println("- Blue stations (solid, never flash)");
    Serial.println("- Red trains (northbound, flashing)");
    Serial.println("- Green trains (southbound, flashing)");
    Serial.println("========================================");
    Serial.println();
}

void loop() {
    unsigned long currentMillis = millis();

    // Update train positions (every 1 second)
    if (currentMillis - lastTrainUpdate >= TRAIN_UPDATE_INTERVAL) {
        time_t now = timeManager.getCurrentTime();
        positionEngine.updateAllTrains(now);
        lastTrainUpdate = currentMillis;
    }

    // Update display rendering (every ~33ms for 30fps)
    if (currentMillis - lastDisplayUpdate >= (1000 / FRAME_RATE)) {
        // Clear LED buffer
        displayManager.clearAllLEDs();

        // Render stations (solid blue, never flash)
        displayManager.setStationLEDs();

        // Get train positions and render them (flashing red/green)
        uint8_t trainCount = 0;
        const TrainPosition* trains = positionEngine.getActiveTrainPositions(&trainCount);
        displayManager.setTrainLEDs(trains, trainCount);

        // Update physical display (handles flash timing and strip.show())
        displayManager.updateDisplay();

        lastDisplayUpdate = currentMillis;
    }

    // Print status every 10 seconds
    if (currentMillis - lastStatusPrint >= 10000) {
        time_t now = timeManager.getCurrentTime();
        struct tm* timeinfo = localtime(&now);

        uint8_t trainCount = 0;
        positionEngine.getActiveTrainPositions(&trainCount);

        Serial.print("[Status] Time: ");
        Serial.print(timeinfo->tm_hour);
        Serial.print(":");
        if (timeinfo->tm_min < 10) Serial.print("0");
        Serial.print(timeinfo->tm_min);
        Serial.print(":");
        if (timeinfo->tm_sec < 10) Serial.print("0");
        Serial.print(timeinfo->tm_sec);
        Serial.print(" | Active Trains: ");
        Serial.print(trainCount);
        Serial.print(" | Uptime: ");
        Serial.print(currentMillis / 1000);
        Serial.println(" sec");

        lastStatusPrint = currentMillis;
    }

    // Small delay to prevent watchdog issues
    delay(1);
}
