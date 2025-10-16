# Seattle Link Light Rail Simulator

PC-based simulation for testing and visualizing the Link Light Rail LED display logic.

## Features

- **100 Virtual LEDs**: Simulates the physical LED strip
- **Real-time Train Movement**: Visualizes northbound (red) and southbound (green) trains
- **Station Display**: Shows all 23 stations as blue LEDs
- **Time Control**: Use system time or set custom time
- **Speed Control**: Run simulation at 1x to 120x speed
- **Interactive Controls**: Play, pause, and reset simulation

## Setup Instructions

### Prerequisites

- Python 3.8 or higher
- CMake 3.12 or higher
- C++ compiler (GCC, Clang, or MSVC)
- Git (for fetching PyBind11)

### Installation

1. **Install Python dependencies:**
   ```bash
   cd simulation/python
   pip install -r requirements.txt
   ```

2. **Build the C++ core library:**
   ```bash
   cd simulation/bindings
   mkdir build
   cd build
   cmake ..
   make
   ```

   The compiled library will be automatically copied to `simulation/python/`.

### Running the Simulation

```bash
cd simulation/python
python3 main.py
```

## Usage

### Playback Controls
- **Start**: Begin the simulation
- **Pause**: Pause the simulation
- **Reset**: Reset to initial state

### Speed Control
Choose from 1x, 2x, 5x, 10x, 30x, 60x, or 120x real-time speed.

### Time Source
- **System Time**: Use your computer's current time
- **Custom Time**: Set a specific time (HH:MM format, e.g., "05:00")

## Display Legend

- **Blue LEDs (solid)**: Stations
- **Red LEDs (flashing)**: Northbound trains
- **Green LEDs (flashing)**: Southbound trains

## Architecture

```
┌─────────────────────────────────────┐
│     Python GUI (Tkinter)            │
│  ┌──────────────────────────────┐   │
│  │  LED Display Widget          │   │
│  │  Controls Widget             │   │
│  └──────────────────────────────┘   │
└───────────────┬─────────────────────┘
                │ PyBind11
┌───────────────▼─────────────────────┐
│   C++ Core Library (.so/.dylib)     │
│  ┌──────────────────────────────┐   │
│  │  Schedule Module             │   │
│  │  Position Engine             │   │
│  └──────────────────────────────┘   │
└─────────────────────────────────────┘
```

## Troubleshooting

### Build Issues

**CMake can't find Python:**
```bash
# macOS
brew install python3

# Ubuntu/Debian
sudo apt-get install python3-dev

# Set Python path explicitly
cmake -DPython3_EXECUTABLE=$(which python3) ..
```

**PyBind11 not found:**
The CMakeLists.txt will automatically fetch PyBind11 from GitHub if not found locally.

### Runtime Issues

**Module not found:**
Make sure the compiled library (`link_rail_core.*.so` or similar) is in the `simulation/python/` directory.

**tkinter not found:**
```bash
# macOS
# tkinter comes with Python

# Ubuntu/Debian
sudo apt-get install python3-tk
```

## Development

The core logic modules (`schedule_module.cpp` and `position_engine.cpp`) are shared between the ESP32 firmware and this simulation, ensuring functional parity.

## Key Stations (LED Positions)

| Station | LED Index |
|---------|-----------|
| Lynnwood City Center | 0 |
| Northgate | 22 |
| Westlake | 44 |
| Stadium | 51 |
| Angle Lake | 99 |

## License

Part of the Seattle Link Light Rail LED Display project.
