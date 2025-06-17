


https://github.com/user-attachments/assets/015a3ca3-1b53-4a46-96ef-e879f9124d6a



# ESP8266 Ultrasonic Distance Monitor with I²C LCD

A simple ESP8266 project that measures distance using an HC-SR04 ultrasonic sensor, displays it on a 16×2 I²C LCD, and prints readings to the Serial Monitor. Wi-Fi connectivity is initialized (with optional ThingSpeak integration).

## Hardware Required

- **ESP8266** (e.g. NodeMCU, Wemos D1 Mini)
- **HC-SR04 Ultrasonic Sensor**
- **16×2 I²C LCD** (with PCF8574 I²C backpack)
- **Jumper wires** and **breadboard** or soldered connections
- **5V USB power supply** (or 3.3V regulated for ESP8266)

---

## Wiring

| ESP8266 Pin | HC-SR04         | I²C LCD         |
| ----------- | --------------- | --------------- |
| `D6`        | Trig            | —               |
| `D5`        | Echo            | —               |
| `GND`       | GND             | GND             |
| `3V3`       | Vcc             | Vcc (5V-tolerant) |
| —           | —               | SDA → `D2`      |
| —           | —               | SCL → `D1`      |

> **Note:** On some ESP8266 boards SDA/SCL are fixed to `D2`/`D1`. Check your board’s pin mapping.

---

## Software Requirements

- Arduino IDE (1.8.x or later)  
- ESP8266 Board Support (via Boards Manager)  
- Libraries (install via Library Manager):  
  - [LiquidCrystal_I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C)  
  - ESP8266WiFi  

- Optional for ThingSpeak:
  - [ThingSpeak](https://github.com/mathworks/thingspeak-arduino)

---

## Installation & Configuration

1. **Clone or Download** this repository.  
2. **Create `secrets.h`** in the project folder with your Wi-Fi and ThingSpeak keys:
   ```cpp
   #define SECRET_SSID      "YourNetworkSSID"
   #define SECRET_PASS      "YourNetworkPassword"
   #define SECRET_CH_ID     123456 // Your ThingSpeak Channel ID
   #define SECRET_WRITE_APIKEY "WRITE_API_KEY"
   #define SECRET_READ_APIKEY_COUNTER "READ_API_KEY"
