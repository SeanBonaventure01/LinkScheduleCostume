"""
Main GUI Application
Integrates LED display, controls, and C++ core logic
"""

import tkinter as tk
from tkinter import ttk
import link_rail_core
from led_display import VirtualLEDDisplay
from controls import SimulationControls
from datetime import datetime
import time


class LinkRailSimulatorGUI:
    """Main simulator GUI application"""

    def __init__(self, root):
        """Initialize the GUI"""
        self.root = root

        # Initialize C++ core modules
        print("Initializing core modules...")
        self.schedule = link_rail_core.ScheduleModule()
        self.schedule.loadSchedule()
        self.position_engine = link_rail_core.PositionEngine()
        self.position_engine.init(self.schedule)

        # Simulation state
        self.is_running = False
        self.is_paused = False
        self.sim_speed = 1.0  # Speed multiplier
        self.use_system_time = True
        self.sim_time = int(time.time())
        self.last_update_time = time.time()

        # Create GUI components
        self._create_widgets()

        # Start update loop
        self._update_loop()

        print("Simulation ready!")

    def _create_widgets(self):
        """Create all GUI widgets"""
        # Main container
        main_frame = ttk.Frame(self.root, padding=10)
        main_frame.pack(fill=tk.BOTH, expand=True)

        # Title
        title_label = ttk.Label(
            main_frame,
            text="Seattle Link Light Rail - Real-Time Simulator",
            font=("Arial", 16, "bold")
        )
        title_label.pack(pady=(0, 10))

        # LED Display
        self.led_display = VirtualLEDDisplay(main_frame, num_leds=100, led_size=10, spacing=2)
        self.led_display.pack(pady=10)

        # Set station labels from schedule data
        stations = []
        for i in range(self.schedule.getStationCount()):
            station = self.schedule.getStation(i)
            if station:
                # Shorten some long names for better display
                name = station.name
                name = name.replace("Shoreline North/185th", "Shoreline N")
                name = name.replace("Shoreline South/148th", "Shoreline S")
                name = name.replace("University of Washington", "UW")
                name = name.replace("Intl Dist/Chinatown", "Chinatown")
                name = name.replace("Lynnwood City Center", "Lynnwood")
                name = name.replace("Mountlake Terrace", "Mt. Terrace")
                name = name.replace("Columbia City", "Columbia")
                name = name.replace("Rainier Beach", "Rainier")
                name = name.replace("Tukwila Intl Blvd", "Tukwila")
                name = name.replace("SeaTac/Airport", "Airport")
                stations.append((station.ledIndex, name))
        self.led_display.set_station_labels(stations)

        # Controls
        self.controls = SimulationControls(
            main_frame,
            on_start=self._start_simulation,
            on_pause=self._pause_simulation,
            on_reset=self._reset_simulation,
            on_speed_change=self._set_speed,
            on_time_change=self._set_time
        )
        self.controls.pack(pady=10)

        # Status display
        status_frame = ttk.Frame(main_frame)
        status_frame.pack(fill=tk.X, pady=5)

        self.status_label = ttk.Label(
            status_frame,
            text="Status: Stopped",
            font=("Arial", 11)
        )
        self.status_label.pack()

        # Info panel
        info_frame = ttk.LabelFrame(main_frame, text="Statistics", padding=5)
        info_frame.pack(fill=tk.X, pady=5)

        self.stats_label = ttk.Label(
            info_frame,
            text="Trains: 0 | Stations: 23",
            font=("Arial", 10)
        )
        self.stats_label.pack()

    def _update_loop(self):
        """Main update loop (runs at ~30 fps)"""
        if self.is_running and not self.is_paused:
            current_real_time = time.time()
            delta_time = current_real_time - self.last_update_time
            self.last_update_time = current_real_time

            # Update simulation time
            if self.use_system_time:
                self.sim_time = int(time.time())
            else:
                # Advance simulation time based on speed multiplier
                self.sim_time += int(delta_time * self.sim_speed)

            # Update train positions (C++ core)
            self.position_engine.updateAllTrains(self.sim_time)

            # Get train positions
            trains = self.position_engine.getActiveTrainPositions()

            # Update LED display
            self._render_display(trains)

            # Update status
            self._update_status(trains)

        # Schedule next update (30 fps)
        self.root.after(33, self._update_loop)

    def _render_display(self, trains):
        """Render the LED display"""
        # Clear display
        self.led_display.clear()

        # Draw stations (blue, solid)
        for i in range(self.schedule.getStationCount()):
            station = self.schedule.getStation(i)
            if station:
                self.led_display.set_led(station.ledIndex, (0, 0, 255), flashing=False)

        # Draw trains (red/green, flashing)
        for train in trains:
            if train.isActive:
                # Northbound = red, Southbound = green
                color = (255, 0, 0) if train.isNorthbound else (0, 255, 0)
                self.led_display.set_led(train.ledIndex, color, flashing=True)

        # Update display
        self.led_display.update()

    def _update_status(self, trains):
        """Update status labels"""
        current_time = datetime.fromtimestamp(self.sim_time)
        time_str = current_time.strftime('%Y-%m-%d %H:%M:%S')

        status_text = f"Time: {time_str} | Speed: {self.sim_speed}x"
        if self.is_paused:
            status_text += " | PAUSED"
        elif self.is_running:
            status_text += " | RUNNING"
        else:
            status_text += " | STOPPED"

        self.status_label.config(text=status_text)

        # Update stats
        num_trains = len(trains)
        num_stations = self.schedule.getStationCount()
        self.stats_label.config(
            text=f"Active Trains: {num_trains} | Stations: {num_stations}"
        )

    def _start_simulation(self):
        """Start the simulation"""
        self.is_running = True
        self.is_paused = False
        self.last_update_time = time.time()
        print("Simulation started")

    def _pause_simulation(self):
        """Pause the simulation"""
        self.is_paused = True
        print("Simulation paused")

    def _reset_simulation(self):
        """Reset the simulation"""
        self.is_running = False
        self.is_paused = False
        self.sim_time = int(time.time())
        self.sim_speed = 1.0

        # Reinitialize position engine
        self.position_engine = link_rail_core.PositionEngine()
        self.position_engine.init(self.schedule)

        # Clear display
        self.led_display.clear()
        self.led_display.update()

        print("Simulation reset")

    def _set_speed(self, speed):
        """Set simulation speed multiplier"""
        self.sim_speed = float(speed)
        print(f"Speed set to {speed}x")

    def _set_time(self, custom_time):
        """Set simulation time"""
        if custom_time is None:
            # Use system time
            self.use_system_time = True
            self.sim_time = int(time.time())
            print("Using system time")
        else:
            # Use custom time
            self.use_system_time = False
            self.sim_time = int(custom_time.timestamp())
            print(f"Custom time set to {custom_time}")
