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
    // Only render trains when flash state is ON
    if (!flashState_) {
        return;  // During flash OFF, don't add train colors
    }

    // Render trains with additive color mixing
    for (uint8_t i = 0; i < count; i++) {
        if (trains[i].isActive) {
            uint8_t ledIndex = trains[i].ledIndex;
            if (ledIndex < NUM_LEDS) {
                // Get current pixel color
                uint32_t currentColor = strip_.getPixelColor(ledIndex);
                uint8_t r = (currentColor >> 16) & 0xFF;
                uint8_t g = (currentColor >> 8) & 0xFF;
                uint8_t b = currentColor & 0xFF;

                // Add train color (red for northbound, green for southbound)
                if (trains[i].isNorthbound) {
                    // Northbound = red
                    r = (r + NORTH_TRAIN_R > 255) ? 255 : (r + NORTH_TRAIN_R);
                } else {
                    // Southbound = green
                    g = (g + SOUTH_TRAIN_G > 255) ? 255 : (g + SOUTH_TRAIN_G);
                }

                // Set the combined color (additive mixing)
                strip_.setPixelColor(ledIndex, strip_.Color(r, g, b));
            }
        }
    }
}

void DisplayManager::updateDisplay() {
    // Update flash state (2 Hz = 500ms interval)
    unsigned long currentMillis = millis();
    if (currentMillis - lastFlashToggle_ >= FLASH_INTERVAL) {
        flashState_ = !flashState_;
        lastFlashToggle_ = currentMillis;
    }

    // Update the physical LED strip
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
