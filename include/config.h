#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
#define WIFI_SSID "seans-iphone"
#define WIFI_PASSWORD "link-halloween25"
#define WIFI_CONNECT_TIMEOUT 10000     // 10 seconds timeout for connection

// NTP Configuration
#define NTP_SERVER "pool.ntp.org"
#define NTP_SYNC_INTERVAL 3600000      // 1 hour
#define TIMEZONE_OFFSET_SECONDS -28800  // UTC-8 (PST)

// LED Configuration
#define NUM_LEDS 100
#define LED_PIN 32                      // GPIO 32 (D32) - WS2812B data line
#define LED_BRIGHTNESS 64               // 0-255
#define FRAME_RATE 30

// Train Configuration
#define BREATHING_CYCLE_MS 2000         // Breathing cycle: 1000ms fade up + 1000ms fade down (0.5 Hz)
#define TRAIN_UPDATE_INTERVAL 1000      // milliseconds

// Color definitions (RGB values for NeoPixel)
#define STATION_R 0
#define STATION_G 0
#define STATION_B 255

#define NORTH_TRAIN_R 255
#define NORTH_TRAIN_G 0
#define NORTH_TRAIN_B 0

#define SOUTH_TRAIN_R 0
#define SOUTH_TRAIN_G 255
#define SOUTH_TRAIN_B 0

#endif // CONFIG_H
