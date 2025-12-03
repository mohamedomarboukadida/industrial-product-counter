# 🏭 Industrial Product Counter

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![ESP32](https://img.shields.io/badge/ESP32-CAM-green.svg)](https://www.espressif.com/)

 **Automatic Object Counting System** using ESP32-CAM Module and OpenCV.  Perfect for industrial automation, inventory management, and production line monitoring.

---

## 📋 Table of Contents

- [Features](#-features)
- [Hardware Requirements](#-hardware-requirements)
- [Wiring Diagram](#-wiring-diagram)
- [Software Requirements](#-software-requirements)
- [Installation](#-installation)
- [Usage](#-usage)
- [Configuration](#-configuration)
- [Troubleshooting](#-troubleshooting)
- [Contributing](#-contributing)

---

## ✨ Features

- ✅ **Real-time object detection** using OpenCV
- ✅ **Wireless video streaming** via ESP32-CAM
- ✅ **Automatic object counting** with visual feedback
- ✅ **Adjustable detection parameters** for different environments
- ✅ **Low-cost solution** (~$10 for ESP32-CAM)
- ✅ **FPS counter** for performance monitoring
- ✅ **Snapshot capture** functionality
- ✅ **Bounding box visualization** for detected objects

---

## 🔧 Hardware Requirements

| Component | Quantity | Description |
|-----------|----------|-------------|
| ESP32-CAM Board | 1 | AI-Thinker module with OV2640 camera |
| FTDI Module | 1 | USB-to-Serial programmer (FT232RL) |
| Micro-USB Cable | 1 | For power and programming |
| Jumper Wires | 10 | Female-to-Female recommended |

### Optional Components
- External 5V power supply (for stable operation)
- 3D printed case for ESP32-CAM
- LED ring light for better illumination

---

## 🔌 Wiring Diagram

Connect the ESP32-CAM to the FTDI programmer as shown below:

<img width="760" height="565" alt="Screenshot 2025-12-02 180203" src="https://github.com/user-attachments/assets/7b0173fc-1157-4712-849c-521cabb0e793" />

### Connection Table

| ESP32-CAM | FTDI Programmer | Wire Color |
|-----------|-----------------|------------|
| GND | GND | Black |
| 5V | VCC | Red |
| U0R | TX | Green |
| U0T | RX | Blue |
| GPIO0 | GND | Yellow (for programming only) |

> ⚠️ **Important:** GPIO0 must be connected to GND only during code upload.  Disconnect it before running the program.

---

## 💻 Software Requirements

### Arduino IDE
- Arduino IDE 1.8.x or 2.x
- ESP32 Board Package
- esp32cam library

### Python
- Python 3. 8 or higher
- OpenCV
- NumPy

---

## 📥 Installation

### 1. Clone the Repository

```bash
git clone https://github.com/mohamedomarboukadida/industrial-product-counter.git
cd industrial-product-counter
```

### 2. Arduino Setup

1. **Install Arduino IDE** from [arduino.cc](https://www.arduino.cc/en/software)

2. **Add ESP32 Board Package:**
   - Go to `File` → `Preferences`
   - Add this URL to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to `Tools` → `Board` → `Boards Manager`
   - Search for "ESP32" and install

3. **Install esp32cam Library:**
   - Go to `Sketch` → `Include Library` → `Manage Libraries`
   - Search for "esp32cam" and install

4. **Configure and Upload:**
   - Open `arduino/esp32cam_stream/esp32cam_stream. ino`
   - Update WiFi credentials:
     ```cpp
     const char* WIFI_SSID = "your_wifi_name";
     const char* WIFI_PASS = "your_wifi_password";
     ```
   - Select Board: `Tools` → `Board` → `AI Thinker ESP32-CAM`
   - Connect FTDI and upload

### 3. Python Setup

```bash
cd python
pip install -r requirements.txt
```

Or install manually:
```bash
pip install opencv-python numpy
```

---

## 🚀 Usage

### Step 1: Power Up ESP32-CAM

1.  Disconnect GPIO0 from GND
2. Power the ESP32-CAM
3. Open Serial Monitor (115200 baud)
4. Note the IP address displayed

### Step 2: Run the Counter

```bash
cd python
python object_counter.py
```

### Step 3: Configure IP Address

Edit the IP address in `object_counter.py` to match your ESP32-CAM:

```python
ESP32_IP = ""  # Replace with your IP
```

### Keyboard Controls

| Key | Action |
|-----|--------|
| `q` | Quit the program |
| `s` | Save a snapshot |

---

## ⚙️ Configuration

Adjust these parameters in `object_counter.py` for optimal detection:

```python
# Detection parameters
MIN_CONTOUR_AREA = 500      # Minimum object size (pixels²)
MAX_CONTOUR_AREA = 50000    # Maximum object size (pixels²)
BLUR_KERNEL = (11, 11)      # Blur intensity
CANNY_LOW = 30              # Edge detection sensitivity (low)
CANNY_HIGH = 150            # Edge detection sensitivity (high)
DILATION_ITERATIONS = 2     # Edge thickness
```

### Tips for Best Results

| Scenario | Adjustment |
|----------|------------|
| Small objects not detected | Decrease `MIN_CONTOUR_AREA` |
| Too many false detections | Increase `MIN_CONTOUR_AREA` |
| Objects not fully outlined | Increase `DILATION_ITERATIONS` |
| Noisy edges | Increase `BLUR_KERNEL` size |

---

## 🔍 How It Works

```
┌─────────────┐     WiFi      ┌─────────────┐
│  ESP32-CAM  │ ───────────── │   Computer  │
│  (Camera)   │   Stream      │  (OpenCV)   │
└─────────────┘               └─────────────┘
                                     │
                              ┌──────┴──────┐
                              │  Pipeline   │
                              ├─────────────┤
                              │ 1.  Capture  │
                              │ 2.  Grayscale│
                              │ 3. Blur     │
                              │ 4.  Canny    │
                              │ 5.  Dilate   │
                              │ 6.  Contours │
                              │ 7. Count    │
                              └─────────────┘
```

---

## 🐛 Troubleshooting

<details>
<summary><b>Camera fails to initialize</b></summary>

- Ensure stable 5V power supply
- Check camera ribbon cable connection
- Try pressing the reset button

</details>

<details>
<summary><b>Cannot upload code</b></summary>

- Verify GPIO0 is connected to GND
- Check TX/RX connections (should be crossed)
- Try a different USB port
- Hold BOOT button while uploading

</details>

<details>
<summary><b>WiFi connection fails</b></summary>

- Double-check SSID and password
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)
- Move closer to the router

</details>

<details>
<summary><b>Python script can't connect</b></summary>

- Verify the IP address matches Serial Monitor output
- Ensure computer is on the same network
- Check firewall settings
- Try pinging the ESP32-CAM IP

</details>

<details>
<summary><b>Objects not being detected</b></summary>

- Ensure good contrast between objects and background
- Adjust lighting conditions
- Modify detection parameters
- Use a solid color background

</details>

---

## 📊 Performance

| Resolution | FPS | Use Case |
|------------|-----|----------|
| 320x240 (Low) | 10-15 | Real-time counting |
| 640x480 (Mid) | 5-8 | Balanced |
| 800x600 (High) | 2-4 | Detailed inspection |

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4.  Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 🙏 Acknowledgments

- [ESP32-CAM Documentation](https://github. com/espressif/esp32-camera)
- [OpenCV Library](https://opencv. org/)
- [Arduino Community](https://www. arduino.cc/)

---
