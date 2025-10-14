#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "position_engine.h"

/**
 * Display Manager
 * Controls WS2812B LED strip using Adafruit NeoPixel library
 */
class DisplayManager {
public:
    DisplayManager();

    /**
     * Initialize LED strip
     */
    void init();

    /**
     * Clear all LEDs
     */
    void clearAllLEDs();

    /**
     * Set station LEDs (solid blue)
     */
    void setStationLEDs();

    /**
     * Set train LEDs (flashing red/green)
     * @param trains Array of train positions
     * @param count Number of trains
     */
    void setTrainLEDs(const TrainPosition* trains, uint8_t count);

    /**
     * Update display (call in loop)
     */
    void updateDisplay();

    /**
     * Set LED brightness
     * @param level Brightness level (0-255)
     */
    void setBrightness(uint8_t level);

    /**
     * Set all LEDs to a specific color
     * @param r Red value (0-255)
     * @param g Green value (0-255)
     * @param b Blue value (0-255)
     */
    void setAllLEDs(uint8_t r, uint8_t g, uint8_t b);

private:
    Adafruit_NeoPixel strip_;
    bool flashState_;
    unsigned long lastFlashToggle_;
};

#endif // DISPLAY_MANAGER_H
