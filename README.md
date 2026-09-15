<img width="900" height="1600" alt="image" src="https://github.com/user-attachments/assets/d468230c-8426-486f-9c4b-f0eeebfc61b1" />
<img width="900" height="1600" alt="imagen_2026-09-15_101236347" src="https://github.com/user-attachments/assets/e564a4f9-7bb5-4af4-89cc-d372ab080f32" />








       [ 16x2 LCD DISPLAY ]              [ ARDUINO UNO / NANO ]
       ┌───────────────────┐             ┌────────────────────┐
       │             [GND] ├─────────────┤ [GND]              │
       │             [VCC] ├─────────────┤ [5V]               │
       │    I2C Board      │             │                    │
       │             [SDA] ├─────────────┤ [A4] (SDA Pin)     │
       │             [SCL] ├─────────────┤ [A5] (SCL Pin)     │
       └───────────────────┘             └────────────────────┘
                                                   │
                                                   ▼
                                         [ HIGH-SPEED USB CABLE ]
                                                   │
                                                   ▼
                                           [ HOST PC (Windows) ]
                                        (Runs script.py in background)



# TRAX-LCD-Monitor
Automated PC resource monitor and real-time audio spectrum equalizer for 16x2 LCD using asymmetric Python execution and asynchronous Arduino firmware.





# TRAX-LCD-Monitor

An intelligent, hybrid hardware-software system that acts as an automated desktop utility. It seamlessly toggles between a high-precision PC Resource Monitor (CPU/RAM) and a real-time Audio Spectrum Equalizer by sniffing Windows core audio sessions. Designed for standard 16x2 LCD screens via I2C, running on a minimal footprint with zero physical buttons.

<p align="center">
  <!-- Optional: Uncomment the line below when you upload a demo image/gif -->
  <!-- <img src="demo.gif" width="450" alt="TRAX Monitor Overview"> -->
</p>

## Core Features
- **Dynamic Context Switching:** Automatically shifts to a 16-band vertical spectrum equalizer when active audio is detected outputting from Windows (Spotify, YouTube, games). Reverts back to hardware diagnostics upon silence.
- **Micro-Pixel Precision Rendering:** Enhances standard 16x2 block characters into smooth, granular horizontal progress bars by shifting 5 micro-pixels per step.
- **Asymmetric Data Architecture:** Offloads heavy math (audio loopback processing and OS metrics polling) to the host PC (Python), keeping the micro-controller execution non-blocking and lightweight.
- **Real-Time Data Smoothing:** Implements mathematical *Jitter Noise* filtering and Linear Interpolation (Lerp) to render organic, highly responsive bar fluctuations at ~25 FPS.

## 🔌 Hardware Architecture & Wiring

This project utilizes a software-driven pull-up resistance model (`INPUT_PULLUP`), eliminating the need for bulky physical debouncing components and maintaining a sleek desktop setup.

```text
       [ 16x2 LCD SCREEN ]               [ ARDUINO UNO / NANO ]
       ┌───────────────────┐             ┌────────────────────┐
       │             [GND] ├─────────────┤ [GND]              │
       │             [VCC] ├─────────────┤ [5V]               │
       │    I2C Board      │             │                    │
       │             [SDA] ├─────────────┤ [A4] (SDA Pin)     │
       │             [SCL] ├─────────────┤ [A5] (SCL Pin)     │
       └───────────────────┘             └────────────────────┘
                                                   │
                                                   ▼
                                         [ HIGH-SPEED USB CABLE ]
                                                   │
                                                   ▼
                                           [ HOST PC (Windows) ]
                                           (Runs script.py background)
```

##  Technical Challenges & Engineering Solutions

### 1. Overcoming the 8-Character CGRAM Limitation on HD44780 LCDs
**Challenge:** The standard 16x2 LCD can only store 8 custom characters in memory simultaneously. The vertical equalizer requires all 8 slots for amplitude bars, leaving 0 slots for the 5 micro-pixel horizontal bars needed for the hardware monitor.
**Solution:** Implemented **Dynamic Hot-Swapping Memory Cache Management**. The Arduino firmware actively listens for specific telemetry headers (`AUD:` or `SYS:`). Upon arrival, it instantly wipes the LCD's CGRAM and flashes the required pixel matrix on the fly without halting execution loops.

### 2. Eliminating Blocking Serial Telemetry Overheads
**Challenge:** Standard `Serial.parseInt()` blocks the main execution thread while waiting for data. If a packet is dropped or delayed, the entire system stutters, destroying animation fluidity and button responsiveness.
**Solution:** Developed a **Non-Blocking Serial Asynchronous Buffer Receiver**. The micro-controller polls `Serial.available()` byte-by-byte into a string buffer, executing calculations *only* when a newline character `\n` is read. This dropped frame latency to virtually zero.

### 3. Evading OS Level Driver Audio Latencies in Windows 11
**Challenge:** Native audio output routing in Windows gaming laptops (like the ASUS TUF F16) is heavily guarded by third-party enhancements (Dolby/Realtek), rendering standard Python audio APIs blind (returning constant zero metrics).
**Solution:** Integrated a low-level native **Windows WASAPI Loopback Audio Capture** using the `pyaudiowpatch` engine. This bypasses the OS mixer completely, reading raw sound wave arrays directly from the motherboard's playback endpoints and applying an automated background static noise floor threshold filter (`0.02`).

##  Tech Stack
- **Firmware:** C++, Arduino Core, Wire I2C Library, LiquidCrystal_I2C.
- **Host Software:** Python 3.10+, PyAudioWPatch, NumPy (Digital Signal Processing), Psutil (OS Diagnostics), PySerial (UART Telemetry Protocols).

---
*Developed as an engineering exercise in asynchronous communications and micro-controller memory optimizations.*



