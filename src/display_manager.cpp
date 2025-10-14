#include "display_manager.h"
#include "config.h"

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

    Serial.println("[DisplayManager] init() - LED strip initialized");
}

void DisplayManager::clearAllLEDs() {
    strip_.clear();
}

void DisplayManager::setStationLEDs() {
    // TODO: Implement station LED rendering
    // Will set blue LEDs at station positions
}

void DisplayManager::setTrainLEDs(const TrainPosition* trains, uint8_t count) {
    // TODO: Implement train LED rendering
    // Will set red/green flashing LEDs at train positions
}

void DisplayManager::updateDisplay() {
    // TODO: Implement full display update with flash logic
    // For now just maintain the display
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
