# WorkVital – Circuit Connections

## 1. Main Controller
**ESP32 DevKit V1**

ESP32 acts as the main controller. It collects sensor readings, processes the data, displays information on the OLED and activates the vibration motor during emergencies.

## 2. Pin Connection Table

| Component | Component Pin | ESP32 Pin |
|---|---|---|
| OLED Display (SSD1306) | SDA | GPIO 4 |
| OLED Display (SSD1306) | SCL | GPIO 5 |
| MLX90614 | SDA | GPIO 4 |
| MLX90614 | SCL | GPIO 5 |
| MAX30102 | SDA | GPIO 4 |
| MAX30102 | SCL | GPIO 5 |
| MQ-135 | Analog Output (AO) | GPIO 1 |
| SOS Button | Signal | GPIO 6 |
| Vibration Motor Driver | Control Input | GPIO 7 |

## 3. Power Connections

| Component | Power Connection |
|---|---|
| ESP32 | USB / suitable regulated supply |
| OLED Display | Suitable module supply |
| MLX90614 | Suitable module supply |
| MAX30102 | Suitable module supply |
| MQ-135 | According to module specification |
| Vibration Motor Driver | Suitable motor supply |

## 4. Communication

- OLED, MLX90614 and MAX30102 communicate with ESP32 through the I2C interface.
- MQ-135 provides an analog signal to ESP32.
- SOS button sends an emergency input signal.
- Vibration motor is controlled through an output signal from ESP32.

## 5. Working Principle

The ESP32 collects heart-rate, temperature and air-quality readings from the connected modules. The readings are displayed on the OLED screen. When the SOS button is pressed or the programmed heart-rate threshold is crossed, the vibration motor is activated.

**Note:** This document represents the pin assignments in the current Arduino code. Actual wiring and electrical compatibility must be verified before powering the circuit.
