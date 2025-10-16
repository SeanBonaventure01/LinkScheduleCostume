#!/usr/bin/env python3
"""
Seattle Link Light Rail Simulator
Main entry point for the simulation GUI
"""

import tkinter as tk
from gui import LinkRailSimulatorGUI

def main():
    """Main entry point"""
    root = tk.Tk()
    root.title("Seattle Link Light Rail Simulator")
    root.resizable(False, False)

    app = LinkRailSimulatorGUI(root)

    # Center window on screen
    root.update_idletasks()
    width = root.winfo_width()
    height = root.winfo_height()
    x = (root.winfo_screenwidth() // 2) - (width // 2)
    y = (root.winfo_screenheight() // 2) - (height // 2)
    root.geometry(f'{width}x{height}+{x}+{y}')

    root.mainloop()

if __name__ == "__main__":
    main()
