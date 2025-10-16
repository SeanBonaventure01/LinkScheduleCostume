"""
Simulation Controls Widget
Provides play/pause, speed control, and time manipulation
"""

import tkinter as tk
from tkinter import ttk
from datetime import datetime


class SimulationControls:
    """Simulation control panel widget"""

    def __init__(self, parent, on_start, on_pause, on_reset, on_speed_change, on_time_change):
        """
        Initialize simulation controls

        Args:
            parent: Parent widget
            on_start: Callback for start button
            on_pause: Callback for pause button
            on_reset: Callback for reset button
            on_speed_change: Callback for speed changes (takes speed multiplier)
            on_time_change: Callback for time changes (takes datetime or None for system time)
        """
        self.frame = ttk.Frame(parent, padding=10)

        # Callbacks
        self.on_start = on_start
        self.on_pause = on_pause
        self.on_reset = on_reset
        self.on_speed_change = on_speed_change
        self.on_time_change = on_time_change

        # State
        self.is_running = False
        self.use_system_time = tk.BooleanVar(value=True)

        # Create controls
        self._create_playback_controls()
        self._create_speed_controls()
        self._create_time_controls()

    def _create_playback_controls(self):
        """Create play/pause/reset buttons"""
        playback_frame = ttk.LabelFrame(self.frame, text="Playback", padding=5)
        playback_frame.grid(row=0, column=0, padx=5, pady=5, sticky="ew")

        self.start_button = ttk.Button(
            playback_frame,
            text="▶ Start",
            command=self._on_start_click
        )
        self.start_button.grid(row=0, column=0, padx=2)

        self.pause_button = ttk.Button(
            playback_frame,
            text="⏸ Pause",
            command=self._on_pause_click,
            state=tk.DISABLED
        )
        self.pause_button.grid(row=0, column=1, padx=2)

        self.reset_button = ttk.Button(
            playback_frame,
            text="⏹ Reset",
            command=self._on_reset_click
        )
        self.reset_button.grid(row=0, column=2, padx=2)

    def _create_speed_controls(self):
        """Create speed control slider"""
        speed_frame = ttk.LabelFrame(self.frame, text="Simulation Speed", padding=5)
        speed_frame.grid(row=0, column=1, padx=5, pady=5, sticky="ew")

        speeds = [
            ("1x", 1),
            ("2x", 2),
            ("5x", 5),
            ("10x", 10),
            ("30x", 30),
            ("60x", 60),
            ("120x", 120)
        ]

        self.speed_var = tk.IntVar(value=1)

        for idx, (label, value) in enumerate(speeds):
            rb = ttk.Radiobutton(
                speed_frame,
                text=label,
                variable=self.speed_var,
                value=value,
                command=self._on_speed_change
            )
            rb.grid(row=0, column=idx, padx=2)

    def _create_time_controls(self):
        """Create time mode controls"""
        time_frame = ttk.LabelFrame(self.frame, text="Time Source", padding=5)
        time_frame.grid(row=1, column=0, columnspan=2, padx=5, pady=5, sticky="ew")

        ttk.Radiobutton(
            time_frame,
            text="System Time",
            variable=self.use_system_time,
            value=True,
            command=self._on_time_mode_change
        ).grid(row=0, column=0, padx=5)

        ttk.Radiobutton(
            time_frame,
            text="Custom Time:",
            variable=self.use_system_time,
            value=False,
            command=self._on_time_mode_change
        ).grid(row=0, column=1, padx=5)

        # Time entry (HH:MM format)
        self.time_entry = ttk.Entry(time_frame, width=10)
        self.time_entry.insert(0, "05:00")  # Default to 5:00 AM
        self.time_entry.grid(row=0, column=2, padx=5)
        self.time_entry.config(state=tk.DISABLED)

        ttk.Button(
            time_frame,
            text="Set Time",
            command=self._on_set_time_click
        ).grid(row=0, column=3, padx=5)

    def _on_start_click(self):
        """Handle start button click"""
        self.is_running = True
        self.start_button.config(state=tk.DISABLED)
        self.pause_button.config(state=tk.NORMAL)
        self.on_start()

    def _on_pause_click(self):
        """Handle pause button click"""
        self.is_running = False
        self.start_button.config(state=tk.NORMAL)
        self.pause_button.config(state=tk.DISABLED)
        self.on_pause()

    def _on_reset_click(self):
        """Handle reset button click"""
        self.is_running = False
        self.start_button.config(state=tk.NORMAL)
        self.pause_button.config(state=tk.DISABLED)
        self.on_reset()

    def _on_speed_change(self):
        """Handle speed change"""
        speed = self.speed_var.get()
        self.on_speed_change(speed)

    def _on_time_mode_change(self):
        """Handle time mode change"""
        if self.use_system_time.get():
            self.time_entry.config(state=tk.DISABLED)
            self.on_time_change(None)  # Use system time
        else:
            self.time_entry.config(state=tk.NORMAL)

    def _on_set_time_click(self):
        """Handle set time button click"""
        if not self.use_system_time.get():
            try:
                time_str = self.time_entry.get()
                hours, minutes = map(int, time_str.split(':'))
                now = datetime.now()
                custom_time = now.replace(hour=hours, minute=minutes, second=0, microsecond=0)
                self.on_time_change(custom_time)
            except ValueError:
                print("Invalid time format. Use HH:MM")

    def pack(self, **kwargs):
        """Pack the controls frame"""
        self.frame.pack(**kwargs)

    def grid(self, **kwargs):
        """Grid the controls frame"""
        self.frame.grid(**kwargs)
