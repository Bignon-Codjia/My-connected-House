# My-connected-House
![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue?style=for-the-badge&logo=espressif)
![Language: C++](https://img.shields.io/badge/Language-C++-green?style=for-the-badge&logo=c%2B%2B)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow?style=for-the-badge)

A professional, "low-cost" approach to home automation. This project transforms an ESP32 into a standalone Wi-Fi Access Point hosting a responsive web dashboard to monitor environmental data and control hardware relays in real-time—no external router or internet required.

---

## 🌟 Key Features

- **Autonomous Network:** Operates in Access Point (AP) mode. Connect directly to the ESP32's own Wi-Fi.
- **Real-time Monitoring:** Live temperature and humidity updates using the DHT22 sensor.
- **Quad-Relay Control:** Manage up to 4 electrical appliances via a sleek, mobile-friendly interface.
- **Memory Optimized:** The entire HTML/CSS/JS frontend is stored in **PROGMEM** (Flash memory) to keep SRAM free for system stability.
- **Responsive UI:** Modern dashboard designed with CSS gradients and SVG icons, optimized for both desktop and mobile screens.

---

## 🛠 Hardware Requirements

| Component | Pin (GPIO) | Description |
| :--- | :--- | :--- |
| **ESP32 DevKit V1** | N/A | Main Microcontroller |
| **DHT22 Sensor** | GPIO 4 | Temp/Humidity Sensing |
| **4-Channel Relay** | 26, 27, 14, 12 | Appliance Switching |
| **Power Supply** | 5V / 2A | Stable power for ESP32 and Relays |

---

## Installation & Setup

### 1. Software Prerequisites
- **Arduino IDE** or **PlatformIO**
- **ESP32 Board Support** (v2.0.x or higher)
- **Required Libraries:**
  - `DHT sensor library` by Adafruit
  - `Adafruit Unified Sensor`

### 2. Configuration
Open the source code and locate the Wi-Fi credentials:
```cpp
const char* ssidAP = "My_ESP32_Dashboard"; 
const char* passwordAP = "YOUR_PASSWORD_HERE"; // Min 8 characters

```

### 3. Deployment

1. Connect your ESP32 to your computer.
2. Select **DOIT ESP32 DEVKIT V1** in your IDE.
3. Click **Upload**.
4. Open the Serial Monitor (115200 baud) to confirm the server is live.

---

## User Guide

1. **Connect:** On your smartphone or laptop, search for the Wi-Fi network `My_ESP32_Dashboard`.
2. **Access:** Open your web browser and go to: `[http://192.168.4.1](http://192.168.4.1)`
3. **Interact:**
* Click **ON/OFF** to trigger the relays.
* Watch the **Activity Log** for command confirmation.
* Sensor data updates automatically every 5 seconds.



---

## Code Architecture

The system is built on an **Event-Driven API** model:

* **Frontend:** A single-page application (SPA) embedded in the C++ code as a raw string literal. It uses the `fetch()` API to communicate with the backend without refreshing the page.
* **RESTful API Endpoints:**
* `GET /`: Serves the HTML dashboard.
* `GET /api/lamp?id=X&state=Y`: Handles relay logic (Active Low).
* `GET /api/sensors`: Returns a JSON object containing sensor readings.


* **Error Handling:** Includes `isnan()` checks for sensor reliability and HTTP 400/500 status codes for robust communication.

---

## 👤 Author

**Bignon Codjia**
*Engineering Student | IoT Developer | Content Creator*

