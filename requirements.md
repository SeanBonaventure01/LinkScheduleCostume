# Seattle Link Light Rail LED Display
## Software Design Document

**Version:** 1.0  
**Date:** October 13, 2025  
**Project:** Real-Time Transit Visualization System

---

## 1. Executive Summary

This document defines the software requirements and implementation specifications for an ESP32-based LED display that visualizes the Seattle Link Light Rail system in real-time. The system uses 100 WS2812B addressable LEDs to represent stations and trains, synchronized with actual schedules via NTP time synchronization.

---

## 2. System Overview

### 2.1 Hardware Components
- **Microcontroller:** ESP32
- **LED Strip:** WS2812B addressable RGB LEDs (100 LEDs)
- **Network:** WiFi (phone hotspot or home network)
- **Power Supply:** Adequate for ESP32 and LED strip (5V, ~6A recommended for 100 LEDs at full brightness)

### 2.2 Visual Representation
- **Stations:** Solid blue LEDs at fixed positions
- **Northbound Trains:** Flashing red LEDs
- **Southbound Trains:** Flashing green LEDs
- **Animation:** Real-time movement based on actual schedule

---

## 3. Functional Requirements

### 3.1 Time Synchronization (FR-TS)
**FR-TS-001:** System shall connect to WiFi network on startup  
**FR-TS-002:** System shall synchronize time using NTP protocol  
**FR-TS-003:** System shall attempt NTP sync every 1 hour when WiFi connected  
**FR-TS-004:** System shall maintain time using internal RTC when WiFi disconnected  
**FR-TS-005:** System shall store last successful sync timestamp  
**FR-TS-006:** System shall apply Pacific Time Zone offset (UTC-8/UTC-7 with DST)

### 3.2 Schedule Management (FR-SM)
**FR-SM-001:** System shall store complete Link Light Rail timetable  
**FR-SM-002:** System shall support weekday and weekend schedules  
**FR-SM-003:** System shall determine first and last train times  
**FR-SM-004:** System shall calculate train headways (time between trains)  
**FR-SM-005:** System shall store inter-station travel times  
**FR-SM-006:** System shall handle multiple light rail lines (1 Line, 2 Line)  
**FR-SM-007:** System shall wrap schedule to next day when current time is after last train  
**FR-SM-008:** System shall handle overnight service (trains running past midnight)

### 3.3 Train Position Calculation (FR-TP)
**FR-TP-001:** System shall determine all active trains based on current time  
**FR-TP-002:** System shall calculate each train's current position on route  
**FR-TP-003:** System shall interpolate position between stations  
**FR-TP-004:** System shall map train position to specific LED index  
**FR-TP-005:** System shall update train positions every 1 second  
**FR-TP-006:** System shall remove trains that have completed their routes  
**FR-TP-007:** System shall treat times after last train as next day's schedule  
**FR-TP-008:** System shall correctly handle the transition period between 01:00 AM and 05:00 AM

### 3.4 LED Display Control (FR-LED)
**FR-LED-001:** System shall illuminate station LEDs as solid blue (RGB: 0, 0, 255)
**FR-LED-002:** Stations shall never flash - blue component is always on
**FR-LED-003:** System shall illuminate northbound trains as flashing red (RGB: 255, 0, 0)
**FR-LED-004:** System shall illuminate southbound trains as flashing green (RGB: 0, 255, 0)
**FR-LED-005:** System shall flash train LEDs at 2 Hz (500ms on, 500ms off)
**FR-LED-006:** System shall use additive color mixing when elements overlap at same LED
**FR-LED-007:** System shall combine RGB values additively (e.g., red + green = yellow [255, 255, 0])
**FR-LED-008:** Station blue component shall remain constant even when train overlaps
**FR-LED-009:** System shall handle LED brightness control (configurable)
**FR-LED-010:** System shall update LED strip at minimum 30 fps
**FR-LED-011:** System shall turn off all LEDs during non-service hours (optional feature)

**Color Mixing Examples:**
- Northbound train (red) + Station (blue) = Magenta [255, 0, 255]
- Southbound train (green) + Station (blue) = Cyan [0, 255, 255]
- Northbound train (red) + Southbound train (green) = Yellow [255, 255, 0]
- Northbound train (red) + Southbound train (green) + Station (blue) = White [255, 255, 255]
- When trains flash off, only station blue remains [0, 0, 255]

### 3.5 Error Handling (FR-EH)
**FR-EH-001:** System shall continue operation if WiFi connection fails
**FR-EH-002:** System shall display status LED or serial output for diagnostics
**FR-EH-003:** System shall recover gracefully from LED strip communication errors
**FR-EH-004:** System shall handle invalid time data
**FR-EH-005:** System shall provide watchdog timer protection

### 3.6 PC Simulation (FR-SIM)
**FR-SIM-001:** System shall include a host PC simulation environment
**FR-SIM-002:** Simulation shall display a graphical representation of the LED strip
**FR-SIM-003:** Simulation shall use system time or allow user-specified starting time
**FR-SIM-004:** Simulation shall support adjustable time acceleration (simulation speed)
**FR-SIM-005:** Simulation shall reuse all core logical components (Schedule Module, Position Engine, Time Manager)
**FR-SIM-006:** Simulation shall only replace the Display Manager with a GUI-based renderer
**FR-SIM-007:** Simulation shall maintain functional parity with embedded system behavior
**FR-SIM-008:** Simulation shall provide controls for time manipulation and speed adjustment
**FR-SIM-009:** Simulation shall display current simulation time and speed multiplier
**FR-SIM-010:** Simulation shall allow pausing, resuming, and resetting the simulation
**FR-SIM-011:** Simulation shall use Python bindings (PyBind11 or ctypes) to expose C++ core modules
**FR-SIM-012:** Simulation GUI shall be implemented in Python using tkinter or PyQt
**FR-SIM-013:** Core C++ modules shall be compiled as a shared library for Python import
**FR-SIM-014:** Python GUI shall communicate with C++ core through clean API interface

---

## 4. Non-Functional Requirements

### 4.1 Performance
**NFR-P-001:** LED update rate shall maintain >30 fps  
**NFR-P-002:** Time drift shall not exceed 1 second per hour without NTP sync  
**NFR-P-003:** System shall boot and begin display within 10 seconds  
**NFR-P-004:** CPU utilization shall remain below 80% average

### 4.2 Reliability
**NFR-R-001:** System shall operate continuously for 30+ days without restart  
**NFR-R-002:** System shall automatically reconnect to WiFi when available  
**NFR-R-003:** Memory leaks shall not occur during normal operation

### 4.3 Maintainability
**NFR-M-001:** Code shall be modular with clear separation of concerns  
**NFR-M-002:** Schedule data shall be easily updatable without code changes  
**NFR-M-003:** Configuration parameters shall be clearly defined

---

## 5. Software Architecture

### 5.1 Module Overview

```
┌─────────────────────────────────────────────────────┐
│                   Main Controller                    │
│                  (ESP32 Core Loop)                   │
└────────────┬─────────────────────────────┬──────────┘
             │                             │
    ┌────────▼────────┐         ┌─────────▼──────────┐
    │  Time Manager   │         │  Display Manager   │
    │   - NTP Sync    │         │  - LED Control     │
    │   - RTC Track   │         │  - Animation       │
    │   - TZ Offset   │         │  - Brightness      │
    └────────┬────────┘         └─────────┬──────────┘
             │                             │
    ┌────────▼────────┐         ┌─────────▼──────────┐
    │ Schedule Module │         │  Position Engine   │
    │  - Timetables   │◄────────┤  - Train Tracking  │
    │  - Route Data   │         │  - Interpolation   │
    │  - Station Map  │         │  - LED Mapping     │
    └─────────────────┘         └────────────────────┘
             │
    ┌────────▼────────┐
    │  WiFi Manager   │
    │  - Connection   │
    │  - Auto-reconnect│
    └─────────────────┘
```

### 5.2 Module Specifications

#### 5.2.1 WiFi Manager
**Purpose:** Handle WiFi connectivity and maintain connection

**Responsibilities:**
- Connect to configured WiFi network on startup
- Monitor connection status
- Attempt reconnection on disconnect (every 30 seconds)
- Provide connection status to other modules

**Key Functions:**
```cpp
void initWiFi(const char* ssid, const char* password);
bool isConnected();
void maintainConnection();
```

**State Machine:**
- DISCONNECTED → CONNECTING → CONNECTED
- CONNECTED → DISCONNECTED (on connection loss)

#### 5.2.2 Time Manager
**Purpose:** Maintain accurate system time via NTP and RTC fallback

**Responsibilities:**
- Perform NTP synchronization when WiFi available
- Track time using internal clock between syncs
- Apply Pacific timezone offset with DST handling
- Provide current time to other modules

**Key Functions:**
```cpp
void syncNTP();
time_t getCurrentTime();
bool isTimeSynced();
uint32_t getSecondsSinceSync();
void updateInternalClock();
```

**Data Structures:**
```cpp
struct TimeState {
    time_t lastSyncTime;
    uint32_t lastSyncMillis;
    bool isSynced;
    int timezoneOffset;  // Seconds from UTC
};
```

#### 5.2.3 Schedule Module
**Purpose:** Store and provide access to Link Light Rail schedule data

**Responsibilities:**
- Define all station locations and names
- Store timetable data (first train, last train, headways)
- Provide inter-station travel times
- Calculate when trains should exist in system

**Data Structures:**
```cpp
struct Station {
    char name[32];
    uint8_t ledIndex;
    float distanceFromStart;  // Kilometers
};

struct TrainSchedule {
    uint16_t firstTrainMinutes;   // Minutes since midnight
    uint16_t lastTrainMinutes;
    uint8_t headwayMinutes;       // Time between trains
    bool isWeekend;
};

struct InterStationSegment {
    uint8_t stationA;
    uint8_t stationB;
    uint16_t travelTimeSeconds;
};
```

**Key Functions:**
```cpp
void loadSchedule();
Station* getStation(uint8_t index);
uint8_t getStationCount();
uint16_t getTravelTime(uint8_t fromStation, uint8_t toStation);
TrainSchedule* getCurrentSchedule(time_t currentTime);
uint16_t getCurrentMinuteOfDay(time_t currentTime);
bool isServiceHours(uint16_t minuteOfDay);
```

**Schedule Wrap-Around Logic:**
When current time is after the last train (e.g., 02:00 AM), the system treats it as "early morning" and wraps to the next day's schedule:

```cpp
// Convert current time to minutes since midnight
uint16_t getCurrentMinuteOfDay(time_t currentTime) {
    struct tm* timeinfo = localtime(&currentTime);
    return timeinfo->tm_hour * 60 + timeinfo->tm_min;
}

// Check if in service hours or if we should wrap to tomorrow
bool isServiceHours(uint16_t minuteOfDay) {
    // Last train ~01:00 (60 minutes), First train ~05:00 (300 minutes)
    // If between 01:00 and 05:00, consider it "next day early morning"
    if (minuteOfDay >= 60 && minuteOfDay < 300) {
        return false;  // In the gap - no active trains
    }
    return true;
}

// Get effective schedule time (handles wrap-around)
uint16_t getEffectiveScheduleTime(uint16_t minuteOfDay) {
    // If after midnight but before first train, treat as tomorrow's early morning
    // This means trains running from yesterday (00:00-01:00) are still active
    if (minuteOfDay < 60) {
        // Between midnight and 01:00 - trains from yesterday still running
        return minuteOfDay + 1440;  // Add 24 hours in minutes
    }
    return minuteOfDay;
}
```

**Example Scenarios:**
- **11:30 PM (23:30):** Normal operation, trains running
- **12:30 AM (00:30):** Trains from yesterday's last departures still completing routes
- **01:30 AM (01:30):** No trains active - gap period
- **04:30 AM (04:30):** No trains active - gap period  
- **05:00 AM (05:00):** First trains of new day begin


#### 5.2.4 Position Engine
**Purpose:** Calculate real-time position of all active trains

**Responsibilities:**
- Determine which trains should be active based on current time
- Calculate each train's position along route
- Interpolate position between stations
- Map position to LED index

**Data Structures:**
```cpp
struct Train {
    uint8_t id;
    bool isNorthbound;
    uint8_t currentStation;
    uint8_t nextStation;
    float progress;           // 0.0 to 1.0 between stations
    time_t departureTime;
    bool isActive;
};

struct TrainPosition {
    uint8_t ledIndex;
    bool isNorthbound;
    bool isActive;
};
```

**Key Functions:**
```cpp
void updateAllTrains(time_t currentTime);
void calculateTrainPosition(Train* train, time_t currentTime);
uint8_t mapPositionToLED(float position);
TrainPosition* getActiveTrainPositions(uint8_t* count);
void spawnNewTrains(time_t currentTime);
void removeCompletedTrains();
```

**Algorithm for Position Calculation:**
1. Get current time and convert to minutes since midnight
2. Handle wrap-around logic for overnight/early morning
3. Determine train's departure time from origin
4. Calculate elapsed time since departure
5. Determine current segment (between which two stations)
6. Calculate progress within segment (0.0 - 1.0)
7. Interpolate LED position based on station LED indices and progress
8. Mark train as inactive if route completed

**Overnight Schedule Handling:**
```cpp
void updateAllTrains(time_t currentTime) {
    uint16_t minuteOfDay = getCurrentMinuteOfDay(currentTime);
    
    // Check if in service gap (01:00 - 05:00)
    if (!isServiceHours(minuteOfDay)) {
        // Remove all active trains during gap period
        removeAllTrains();
        return;
    }
    
    // For times 00:00-01:00, allow trains from previous day to complete
    uint16_t effectiveTime = getEffectiveScheduleTime(minuteOfDay);
    
    // Update existing trains
    for (each active train) {
        calculateTrainPosition(train, currentTime);
        if (train completed route) {
            removeCompletedTrain(train);
        }
    }
    
    // Spawn new trains based on schedule
    spawnNewTrains(currentTime, effectiveTime);
}
```

#### 5.2.5 Display Manager
**Purpose:** Control WS2812B LED strip using Adafruit NeoPixel library

**Responsibilities:**
- Initialize Adafruit NeoPixel strip
- Render station LEDs (solid blue)
- Render train LEDs (flashing red/green)
- Handle flash timing for trains
- Manage brightness and color mixing
- Update LED strip at consistent frame rate

**Key Functions:**
```cpp
void initLEDs();
void clearAllLEDs();
void setStationLEDs();
void setTrainLEDs(TrainPosition* trains, uint8_t count);
void updateDisplay();
void setBrightness(uint8_t level);
```

**Adafruit NeoPixel Implementation:**
```cpp
// Initialization
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void initLEDs() {
    strip.begin();
    strip.setBrightness(LED_BRIGHTNESS);
    strip.clear();
    strip.show();
}

void setBrightness(uint8_t level) {
    strip.setBrightness(level);
}
```

**Rendering Logic with Additive Color Mixing:**
```cpp
// Pseudo-code for each frame
clearAllLEDs();

// First pass: Set station base colors (always on, never flash)
for (each station) {
    ledBuffer[station.ledIndex].r += 0;
    ledBuffer[station.ledIndex].g += 0;
    ledBuffer[station.ledIndex].b += 255;  // Blue always on
}

// Update flash state for trains
if (currentMillis - lastFlashToggle > 500) {
    flashState = !flashState;
    lastFlashToggle = currentMillis;
}

// Second pass: Add train colors (only when flash state is ON)
if (flashState) {
    for (each active train) {
        if (train.isNorthbound) {
            ledBuffer[train.ledIndex].r += 255;  // Add red
        } else {
            ledBuffer[train.ledIndex].g += 255;  // Add green
        }
    }
}

// Clamp values to 255 max and update strip
for (each LED) {
    ledBuffer[i].r = min(ledBuffer[i].r, 255);
    ledBuffer[i].g = min(ledBuffer[i].g, 255);
    ledBuffer[i].b = min(ledBuffer[i].b, 255);
    strip.setPixelColor(i, ledBuffer[i].r, ledBuffer[i].g, ledBuffer[i].b);
}

strip.show();
```

**Additive Color Mixing Rules:**
- Stations always contribute blue (0, 0, 255) - never flash
- Trains contribute red or green when flash state is ON
- RGB values are added together and clamped to 255 maximum
- Multiple trains at same LED combine their colors additively

#### 5.2.6 Main Controller
**Purpose:** Orchestrate all modules and manage main execution loop

**Responsibilities:**
- Initialize all subsystems
- Execute main loop with proper timing
- Coordinate module interactions
- Handle system-level errors

**Main Loop Flow:**
```cpp
void loop() {
    // WiFi maintenance (non-blocking)
    wifiManager.maintain();
    
    // Time synchronization (non-blocking check)
    timeManager.update();
    
    // Update train positions (every 1 second)
    if (millis() - lastTrainUpdate > 1000) {
        time_t now = timeManager.getCurrentTime();
        positionEngine.updateAllTrains(now);
        lastTrainUpdate = millis();
    }
    
    // Update display (every ~33ms for 30fps)
    if (millis() - lastDisplayUpdate > 33) {
        displayManager.updateDisplay();
        lastDisplayUpdate = millis();
    }
}
```

---

## 6. Data Specifications

### 6.1 Link Light Rail Line Data

**Stations (1 Line Example - Northgate to Angle Lake):**
```
Station Name              | LED Index | Distance (km)
--------------------------|-----------|-------------
Northgate                 | 0         | 0.0
Roosevelt                 | 6         | 2.4
U District                | 11        | 3.8
University of Washington  | 16        | 5.2
Capitol Hill              | 23        | 7.5
Westlake                  | 30        | 9.8
University Street         | 33        | 10.5
Pioneer Square            | 36        | 11.2
International District    | 39        | 11.9
Stadium                   | 43        | 13.0
SODO                      | 48        | 14.8
Beacon Hill               | 54        | 16.9
Mount Baker               | 60        | 19.2
Columbia City             | 66        | 21.5
Othello                   | 72        | 23.8
Rainier Beach             | 78        | 26.1
Tukwila International Blvd| 87        | 30.0
Angle Lake                | 99        | 35.0
```

**Travel Times Between Stations:**
Average: 2-3 minutes between adjacent stations

### 6.2 Schedule Parameters

**Weekday Service:**
- First northbound train: 05:00
- Last northbound train: 01:00 (next day)
- First southbound train: 05:15
- Last southbound train: 01:15 (next day)
- Peak headway: 6 minutes
- Off-peak headway: 10 minutes
- Late night headway: 15 minutes

**Weekend Service:**
- First train: 06:00
- Last train: 01:00 (next day)
- Headway: 10-12 minutes

---

## 7. Implementation Requirements

### 7.1 Required Libraries
- **Adafruit_NeoPixel** (v1.10.0+) - WS2812B control
- **WiFi.h** - ESP32 WiFi connectivity (built-in)
- **time.h** - Time manipulation (built-in)
- **NTPClient** or custom NTP implementation

**Library Installation:**
```cpp
// Install via Arduino Library Manager or PlatformIO
// Adafruit NeoPixel by Adafruit
```

**Adafruit NeoPixel Configuration:**
```cpp
#include <Adafruit_NeoPixel.h>

// Initialize strip
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Color definitions (32-bit format)
#define STATION_COLOR strip.Color(0, 0, 255)      // Blue
#define NORTH_TRAIN_COLOR strip.Color(255, 0, 0)  // Red  
#define SOUTH_TRAIN_COLOR strip.Color(0, 255, 0)  // Green
```

### 7.2 Configuration Parameters
```cpp
// WiFi Configuration
#define WIFI_SSID "YourPhoneHotspot"
#define WIFI_PASSWORD "password"
#define WIFI_RECONNECT_INTERVAL 30000  // 30 seconds

// NTP Configuration
#define NTP_SERVER "pool.ntp.org"
#define NTP_SYNC_INTERVAL 3600000      // 1 hour
#define TIMEZONE_OFFSET_SECONDS -28800  // UTC-8 (PST)

// LED Configuration
#define NUM_LEDS 100
#define LED_PIN 5
#define LED_BRIGHTNESS 128              // 0-255
#define FRAME_RATE 30

// Train Configuration
#define FLASH_INTERVAL 500              // milliseconds
#define TRAIN_UPDATE_INTERVAL 1000      // milliseconds

// Colors
#define STATION_COLOR CRGB(0, 0, 255)   // Blue
#define NORTH_TRAIN_COLOR CRGB(255, 0, 0)  // Red
#define SOUTH_TRAIN_COLOR CRGB(0, 255, 0)  // Green
```

### 7.3 Memory Considerations
- **Flash:** Schedule data stored in PROGMEM (program memory)
- **SRAM:** Dynamic train objects (estimated ~20 trains max)
- **Heap:** LED buffer (~300 bytes for 100 RGB LEDs)

**Estimated Memory Usage:**
- LED buffer: 300 bytes
- Train array: 20 trains × 20 bytes = 400 bytes
- Station data: 20 stations × 36 bytes = 720 bytes
- Schedule data: ~1KB in flash
- Total SRAM: ~2KB (ESP32 has 520KB)

### 7.4 File Structure
```
project/
├── src/                          // ESP32 embedded code
│   ├── main.cpp                  // Main program entry
│   ├── wifi_manager.cpp/h        // WiFi management
│   ├── time_manager.cpp/h        // NTP and time tracking
│   ├── schedule_module.cpp/h     // Timetable data
│   ├── position_engine.cpp/h     // Train position logic
│   ├── display_manager.cpp/h     // LED control
│   └── config.h                  // Configuration constants
├── core/                         // Shared core modules (ESP32 + Simulation)
│   ├── schedule_module.cpp/h     // Platform-independent schedule logic
│   ├── position_engine.cpp/h     // Platform-independent position logic
│   └── time_utils.cpp/h          // Platform-independent time utilities
├── simulation/                   // PC simulation
│   ├── bindings/
│   │   ├── pybind11_wrapper.cpp  // PyBind11 interface
│   │   └── CMakeLists.txt        // Build configuration for shared library
│   ├── python/
│   │   ├── main.py               // Python GUI application entry point
│   │   ├── gui.py                // GUI implementation (tkinter/PyQt)
│   │   ├── led_display.py        // Virtual LED strip widget
│   │   ├── controls.py           // Time and speed control widgets
│   │   └── requirements.txt      // Python dependencies
│   └── README.md                 // Simulation setup instructions
├── data/
│   └── schedule.json             // Schedule data (optional)
└── platformio.ini                // PlatformIO configuration
```

### 7.5 PC Simulation Implementation

#### 7.5.1 Architecture Overview

The PC simulation uses a hybrid C++/Python architecture:

```
┌─────────────────────────────────────────────────────────┐
│                Python GUI Layer (simulation/python/)     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │ GUI Controls │  │  LED Display │  │ Time Manager │  │
│  │  (tkinter)   │  │   (Canvas)   │  │   Controls   │  │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  │
│         │                  │                  │          │
│         └──────────────────┼──────────────────┘          │
│                            │                             │
└────────────────────────────┼─────────────────────────────┘
                             │ Python Bindings (PyBind11)
┌────────────────────────────┼─────────────────────────────┐
│                  C++ Core Library (.so/.dll/.dylib)      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   Schedule   │  │   Position   │  │     Time     │  │
│  │    Module    │  │    Engine    │  │   Utilities  │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└─────────────────────────────────────────────────────────┘
```

#### 7.5.2 C++ Core Library

**Purpose:** Compile core logic modules as a shared library for Python

**Key Components:**
- Schedule Module (unchanged from ESP32 version)
- Position Engine (unchanged from ESP32 version)
- Time utilities (platform-independent time functions)

**PyBind11 Interface:**
```cpp
// simulation/bindings/pybind11_wrapper.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "schedule_module.h"
#include "position_engine.h"

namespace py = pybind11;

PYBIND11_MODULE(link_rail_core, m) {
    m.doc() = "Link Light Rail simulation core";

    // Station struct binding
    py::class_<Station>(m, "Station")
        .def(py::init<>())
        .def_readwrite("name", &Station::name)
        .def_readwrite("ledIndex", &Station::ledIndex)
        .def_readwrite("distanceFromStart", &Station::distanceFromStart);

    // TrainPosition struct binding
    py::class_<TrainPosition>(m, "TrainPosition")
        .def(py::init<>())
        .def_readwrite("ledIndex", &TrainPosition::ledIndex)
        .def_readwrite("isNorthbound", &TrainPosition::isNorthbound)
        .def_readwrite("isActive", &TrainPosition::isActive);

    // ScheduleModule class binding
    py::class_<ScheduleModule>(m, "ScheduleModule")
        .def(py::init<>())
        .def("loadSchedule", &ScheduleModule::loadSchedule)
        .def("getStation", &ScheduleModule::getStation)
        .def("getStationCount", &ScheduleModule::getStationCount)
        .def("getTravelTime", &ScheduleModule::getTravelTime)
        .def("getCurrentSchedule", &ScheduleModule::getCurrentSchedule);

    // PositionEngine class binding
    py::class_<PositionEngine>(m, "PositionEngine")
        .def(py::init<>())
        .def("updateAllTrains", &PositionEngine::updateAllTrains)
        .def("getActiveTrainPositions", &PositionEngine::getActiveTrainPositions);
}
```

**Build Configuration (CMakeLists.txt):**
```cmake
cmake_minimum_required(VERSION 3.12)
project(link_rail_core)

set(CMAKE_CXX_STANDARD 17)

# Find Python and PyBind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# Add core source files
add_library(link_rail_core MODULE
    pybind11_wrapper.cpp
    ../../core/schedule_module.cpp
    ../../core/position_engine.cpp
    ../../core/time_utils.cpp
)

target_include_directories(link_rail_core PRIVATE
    ../../include
    ../../core
)

target_link_libraries(link_rail_core PRIVATE pybind11::module)
set_target_properties(link_rail_core PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                                  SUFFIX "${PYTHON_MODULE_EXTENSION}")
```

#### 7.5.3 Python GUI Implementation

**Main Application (main.py):**
```python
#!/usr/bin/env python3
import tkinter as tk
from gui import LinkRailSimulatorGUI

def main():
    root = tk.Tk()
    app = LinkRailSimulatorGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()
```

**GUI Structure (gui.py):**
```python
import tkinter as tk
from tkinter import ttk
import link_rail_core  # C++ module
from led_display import VirtualLEDDisplay
from controls import SimulationControls
from datetime import datetime, timedelta
import time

class LinkRailSimulatorGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Link Light Rail Simulator")

        # Initialize C++ core
        self.schedule = link_rail_core.ScheduleModule()
        self.schedule.loadSchedule()
        self.position_engine = link_rail_core.PositionEngine()

        # Simulation state
        self.is_running = False
        self.is_paused = False
        self.sim_speed = 1.0  # 1x real-time
        self.sim_time = time.time()
        self.use_system_time = True

        # Create GUI components
        self.create_widgets()
        self.update_loop()

    def create_widgets(self):
        # LED Display
        self.led_display = VirtualLEDDisplay(self.root, num_leds=100)
        self.led_display.pack(pady=20)

        # Controls
        self.controls = SimulationControls(
            self.root,
            on_start=self.start_simulation,
            on_pause=self.pause_simulation,
            on_reset=self.reset_simulation,
            on_speed_change=self.set_speed,
            on_time_change=self.set_time
        )
        self.controls.pack(pady=10)

        # Status display
        self.status_label = tk.Label(self.root, text="", font=("Arial", 12))
        self.status_label.pack(pady=5)

    def update_loop(self):
        if self.is_running and not self.is_paused:
            # Update simulation time
            if self.use_system_time:
                self.sim_time = time.time()
            else:
                self.sim_time += (1.0/30.0) * self.sim_speed  # 30 fps

            # Update train positions (C++ core)
            self.position_engine.updateAllTrains(int(self.sim_time))

            # Get train positions
            trains = self.position_engine.getActiveTrainPositions()

            # Update LED display
            self.led_display.clear()
            self.draw_stations()
            self.draw_trains(trains)
            self.led_display.update()

            # Update status
            current_time = datetime.fromtimestamp(self.sim_time)
            self.status_label.config(
                text=f"Time: {current_time.strftime('%Y-%m-%d %H:%M:%S')} | "
                     f"Speed: {self.sim_speed}x | Trains: {len(trains)}"
            )

        # Schedule next update (30 fps)
        self.root.after(33, self.update_loop)

    def draw_stations(self):
        for i in range(self.schedule.getStationCount()):
            station = self.schedule.getStation(i)
            self.led_display.set_led(station.ledIndex, (0, 0, 255))  # Blue

    def draw_trains(self, trains):
        for train in trains:
            if train.isActive:
                color = (255, 0, 0) if train.isNorthbound else (0, 255, 0)
                self.led_display.set_led(train.ledIndex, color, flashing=True)
```

**Virtual LED Display (led_display.py):**
```python
import tkinter as tk
from tkinter import Canvas
import time

class VirtualLEDDisplay:
    def __init__(self, parent, num_leds=100, led_size=8, spacing=2):
        self.num_leds = num_leds
        self.led_size = led_size
        self.spacing = spacing
        self.flash_state = False
        self.last_flash_toggle = time.time()

        # Create canvas
        canvas_width = (led_size + spacing) * num_leds + spacing
        canvas_height = led_size + spacing * 2
        self.canvas = Canvas(parent, width=canvas_width, height=canvas_height, bg="black")

        # Create LED circles
        self.leds = []
        for i in range(num_leds):
            x = spacing + i * (led_size + spacing)
            y = spacing
            led = self.canvas.create_oval(
                x, y, x + led_size, y + led_size,
                fill="gray20", outline="gray40"
            )
            self.leds.append(led)

        # LED state storage
        self.led_colors = [(0, 0, 0)] * num_leds
        self.led_flashing = [False] * num_leds

    def set_led(self, index, color, flashing=False):
        if 0 <= index < self.num_leds:
            self.led_colors[index] = color
            self.led_flashing[index] = flashing

    def clear(self):
        self.led_colors = [(0, 0, 0)] * self.num_leds
        self.led_flashing = [False] * self.num_leds

    def update(self):
        # Toggle flash state every 500ms
        current_time = time.time()
        if current_time - self.last_flash_toggle > 0.5:
            self.flash_state = not self.flash_state
            self.last_flash_toggle = current_time

        # Update LED colors
        for i in range(self.num_leds):
            if self.led_flashing[i] and not self.flash_state:
                color = "gray20"  # Off during flash
            else:
                r, g, b = self.led_colors[i]
                color = f"#{r:02x}{g:02x}{b:02x}"
            self.canvas.itemconfig(self.leds[i], fill=color)

    def pack(self, **kwargs):
        self.canvas.pack(**kwargs)
```

#### 7.5.4 Build and Run Instructions

**Setup:**
```bash
# Install dependencies
pip install pybind11 tkinter

# Build C++ shared library
cd simulation/bindings
mkdir build && cd build
cmake ..
make

# Copy library to Python directory
cp link_rail_core.* ../../python/

# Run simulation
cd ../../python
python main.py
```

**Requirements (requirements.txt):**
```
pybind11>=2.10.0
tkinter
```

#### 7.5.5 Simulation Features

**Time Controls:**
- System time mode: Use real computer time
- Custom time mode: Set specific starting time
- Speed control: 1x, 2x, 5x, 10x, 60x (1 minute = 1 second)

**Playback Controls:**
- Play/Pause: Start and pause simulation
- Reset: Return to initial state
- Step: Advance by single time increment

**Display Features:**
- 100 virtual LEDs displayed horizontally
- Station indicators (blue, solid)
- Northbound trains (red, flashing)
- Southbound trains (green, flashing)
- Real-time train count and time display

---

## 8. Testing Requirements

### 8.1 Unit Tests
- Time manager accuracy (drift measurement)
- Position calculation algorithm verification
- LED mapping correctness
- Train spawn/removal logic

### 8.2 Integration Tests
- WiFi connection and NTP sync flow
- Complete train lifecycle from spawn to completion
- Display rendering with multiple trains
- Timezone and DST transitions

### 8.3 System Tests
- 24-hour continuous operation test
- WiFi disconnect/reconnect scenarios
- Time accuracy over extended periods
- Memory leak detection

### 8.4 Test Cases

**TC-001: NTP Synchronization**
- Preconditions: WiFi connected
- Steps: Power on system, wait for NTP sync
- Expected: Time accurate within 1 second of actual time

**TC-002: WiFi Loss Recovery**
- Preconditions: System running with valid time
- Steps: Disconnect WiFi, observe for 10 minutes
- Expected: Time drift <10 seconds, display continues

**TC-003: Train Position Accuracy**
- Preconditions: Known schedule loaded
- Steps: Set time to specific moment, observe train positions
- Expected: Trains appear at calculated positions matching schedule

**TC-004: Display Frame Rate**
- Preconditions: System running
- Steps: Measure LED update frequency
- Expected: Maintain >30 fps

---

## 9. Future Enhancements

### 9.1 Phase 2 Features
- Sound Transit API integration for real-time delay data
- Web interface for configuration
- OTA (Over-The-Air) firmware updates
- Multiple line support with color coding
- Service alerts display
- Configurable brightness via web interface

### 9.2 Phase 3 Features
- Mobile app for control and monitoring
- Historical data logging
- Predictive arrival time calculation
- Integration with other transit systems (buses)

---

## 10. References

### 10.1 External Documentation
- ESP32 Technical Reference Manual
- WS2812B Datasheet
- Sound Transit Schedule Information
- NTP Protocol Specification (RFC 5905)

### 10.2 Related Standards
- IEEE 802.11 (WiFi)
- ISO 8601 (Date and Time Format)
- RGB Color Space Standard

---

## 11. Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-10-13 | Initial | Initial document creation |

---

## Appendix A: Abbreviations

- **API** - Application Programming Interface
- **DST** - Daylight Saving Time
- **ESP32** - Espressif Systems 32-bit Microcontroller
- **fps** - Frames Per Second
- **LED** - Light Emitting Diode
- **NTP** - Network Time Protocol
- **OTA** - Over-The-Air
- **RGB** - Red Green Blue
- **RTC** - Real-Time Clock
- **SRAM** - Static Random-Access Memory
- **WiFi** - Wireless Fidelity
- **WS2812B** - WorldSemi addressable LED chip