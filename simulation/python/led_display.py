"""
Virtual LED Display Widget
Renders a horizontal strip of 100 LEDs using tkinter Canvas
"""

import tkinter as tk
from tkinter import Canvas
import time


class VirtualLEDDisplay:
    """Virtual LED strip display widget"""

    def __init__(self, parent, num_leds=100, led_size=10, spacing=2):
        """
        Initialize virtual LED display

        Args:
            parent: Parent widget
            num_leds: Number of LEDs (default 100)
            led_size: Size of each LED in pixels (default 10)
            spacing: Spacing between LEDs in pixels (default 2)
        """
        self.num_leds = num_leds
        self.led_size = led_size
        self.spacing = spacing
        self.flash_state = False
        self.last_flash_toggle = time.time()

        # Create canvas with extra height for station labels
        canvas_width = (led_size + spacing) * num_leds + spacing
        canvas_height = led_size + spacing * 2 + 80  # Extra space for labels
        self.canvas = Canvas(
            parent,
            width=canvas_width,
            height=canvas_height,
            bg="black",
            highlightthickness=0
        )

        # Create LED circles
        self.leds = []
        y = spacing + 40  # Offset to make room for labels above
        for i in range(num_leds):
            x = spacing + i * (led_size + spacing)
            led = self.canvas.create_oval(
                x, y, x + led_size, y + led_size,
                fill="gray20", outline="gray40", width=1
            )
            self.leds.append(led)

        # LED state storage
        self.led_colors = [(0, 0, 0)] * num_leds
        self.led_flashing = [False] * num_leds

        # Station label storage
        self.station_labels = []

    def set_led(self, index, color, flashing=False):
        """
        Set LED color and flashing state

        Args:
            index: LED index (0-99)
            color: RGB tuple (r, g, b) where each value is 0-255
            flashing: Whether this LED should flash
        """
        if 0 <= index < self.num_leds:
            self.led_colors[index] = color
            self.led_flashing[index] = flashing

    def clear(self):
        """Clear all LEDs to off state"""
        self.led_colors = [(0, 0, 0)] * self.num_leds
        self.led_flashing = [False] * self.num_leds

    def set_station_labels(self, stations):
        """
        Set station labels to display

        Args:
            stations: List of tuples (led_index, station_name)
        """
        # Clear existing labels
        for label_id in self.station_labels:
            self.canvas.delete(label_id)
        self.station_labels = []

        # Add labels for all stations
        led_y = self.spacing + 40  # Where the LEDs are positioned
        led_center_y = led_y + self.led_size / 2

        for i, (led_idx, name) in enumerate(stations):
            x = self.spacing + led_idx * (self.led_size + self.spacing) + self.led_size / 2

            # Alternate labels above and below to avoid overlap
            if i % 2 == 0:
                # Label above - angled tick mark going up-left at 45 degrees
                tick_length = 20
                tick_end_x = x - tick_length * 0.707  # cos(45°)
                tick_end_y = led_center_y - tick_length * 0.707  # sin(45°)

                tick = self.canvas.create_line(
                    x, led_center_y, tick_end_x, tick_end_y,
                    fill="gray60", width=1
                )
                self.station_labels.append(tick)

                # Label text angled away from LED line, reading left-to-right
                label = self.canvas.create_text(
                    tick_end_x - 3, tick_end_y,
                    text=name,
                    fill="white",
                    font=("Arial", 7),
                    angle=-45,
                    anchor="e"
                )
                self.station_labels.append(label)
            else:
                # Label below - angled tick mark going down-left at 45 degrees
                tick_length = 20
                tick_end_x = x - tick_length * 0.707  # cos(45°)
                tick_end_y = led_center_y + tick_length * 0.707  # sin(45°)

                tick = self.canvas.create_line(
                    x, led_center_y, tick_end_x, tick_end_y,
                    fill="gray60", width=1
                )
                self.station_labels.append(tick)

                # Label text angled away from LED line, reading left-to-right
                label = self.canvas.create_text(
                    tick_end_x - 3, tick_end_y,
                    text=name,
                    fill="white",
                    font=("Arial", 7),
                    angle=45,
                    anchor="e"
                )
                self.station_labels.append(label)

    def update(self):
        """Update LED display (call every frame)"""
        # Toggle flash state every 500ms (2 Hz)
        current_time = time.time()
        if current_time - self.last_flash_toggle > 0.5:
            self.flash_state = not self.flash_state
            self.last_flash_toggle = current_time

        # Update LED colors
        for i in range(self.num_leds):
            if self.led_flashing[i] and not self.flash_state:
                # Off during flash cycle
                color = "gray20"
            else:
                r, g, b = self.led_colors[i]
                if r == 0 and g == 0 and b == 0:
                    color = "gray20"  # Off
                else:
                    color = f"#{r:02x}{g:02x}{b:02x}"
            self.canvas.itemconfig(self.leds[i], fill=color)

    def pack(self, **kwargs):
        """Pack the canvas widget"""
        self.canvas.pack(**kwargs)

    def grid(self, **kwargs):
        """Grid the canvas widget"""
        self.canvas.grid(**kwargs)
