#include "display_manager.h"
#include "config.h"
#include "schedule_module.h"

// External reference to schedule module (will be passed via constructor or setter in production)
// For now, we'll need station data to be provided
extern ScheduleModule scheduleModule;

DisplayManager::DisplayManager()
    : strip_(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800),
      flashState_(false),
      lastFlashToggle_(0) {
}

void DisplayManager::init() {
    strip_.begin();
    strip_.setBrightness(LED_BRIGHTNESS);
    strip_.clear();
    strip_.show();

    Serial.println("[DisplayManager] LED strip initialized");
}

void DisplayManager::clearAllLEDs() {
    strip_.clear();
}

void DisplayManager::setStationLEDs() {
    // Render all stations as solid blue
    // Stations NEVER flash - blue component is always on
    uint8_t stationCount = scheduleModule.getStationCount();

    for (uint8_t i = 0; i < stationCount; i++) {
        const Station* station = scheduleModule.getStation(i);
        if (station != nullptr) {
            uint8_t ledIndex = station->ledIndex;
            if (ledIndex < NUM_LEDS) {
                // Get current pixel color
                uint32_t currentColor = strip_.getPixelColor(ledIndex);
                uint8_t r = (currentColor >> 16) & 0xFF;
                uint8_t g = (currentColor >> 8) & 0xFF;
                uint8_t b = currentColor & 0xFF;

                // Add station blue (additive mixing)
                // Clamp to 255 maximum
                b = (b + STATION_B > 255) ? 255 : (b + STATION_B);

                // Set the combined color
                strip_.setPixelColor(ledIndex, strip_.Color(r, g, b));
            }
        }
    }
}

void DisplayManager::setTrainLEDs(const TrainPosition* trains, uint8_t count) {
    // Calculate breathing pulse brightness using sine wave
    // Breathing cycle: 2000ms (0.5 Hz) - smooth acceleration/deceleration
    unsigned long currentMillis = millis();
    float timeInCycle = (currentMillis % BREATHING_CYCLE_MS) / (float)BREATHING_CYCLE_MS;

    // Sine wave breathing: smooth at extremes, faster in middle
    // Map from sine wave (-1 to +1) to brightness range (0.05 to 1.0)
    // This keeps LEDs slightly visible even at minimum brightness
    float sinValue = sin(timeInCycle * 2.0f * PI - PI / 2.0f);
    float brightness = 0.05f + (sinValue + 1.0f) / 2.0f * 0.95f;  // Range: 0.05 to 1.0

    // Render trains with additive color mixing and breathing brightness
    for (uint8_t i = 0; i < count; i++) {
        if (trains[i].isActive) {
            uint8_t ledIndex = trains[i].ledIndex;
            if (ledIndex < NUM_LEDS) {
                // Get current pixel color
                uint32_t currentColor = strip_.getPixelColor(ledIndex);
                uint8_t r = (currentColor >> 16) & 0xFF;
                uint8_t g = (currentColor >> 8) & 0xFF;
                uint8_t b = currentColor & 0xFF;

                // Calculate train color with brightness applied
                uint8_t trainR = 0;
                uint8_t trainG = 0;

                if (trains[i].isNorthbound) {
                    // Northbound = red with breathing
                    trainR = (uint8_t)(NORTH_TRAIN_R * brightness);
                } else {
                    // Southbound = green with breathing
                    trainG = (uint8_t)(SOUTH_TRAIN_G * brightness);
                }

                // Add train color (additive mixing with clamping)
                r = (r + trainR > 255) ? 255 : (r + trainR);
                g = (g + trainG > 255) ? 255 : (g + trainG);

                // Set the combined color
                strip_.setPixelColor(ledIndex, strip_.Color(r, g, b));
            }
        }
    }
}

void DisplayManager::updateDisplay() {
    // Update the physical LED strip
    // Note: Pulse brightness is calculated in setTrainLEDs() based on millis()
    strip_.show();
}

void DisplayManager::setBrightness(uint8_t level) {
    strip_.setBrightness(level);
}

void DisplayManager::setAllLEDs(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < NUM_LEDS; i++) {
        strip_.setPixelColor(i, strip_.Color(r, g, b));
    }
    strip_.show();
}
