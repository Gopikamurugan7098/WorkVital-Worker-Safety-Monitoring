# WorkVital – System Block Diagram

## System Architecture

Sensors
   |
   v
ESP32 Microcontroller
   |
   v
MQTT Communication
   |
   v
Firebase Cloud
   |
   v
Monitoring Dashboard
   |
   v
Early Warning Alert

## Working Principle

1. Sensors monitor workplace environmental conditions.
2. ESP32 collects sensor readings.
3. MQTT transfers the collected data.
4. Firebase stores and manages the data.
5. Dashboard displays monitoring results.
6. Alerts notify users when unsafe conditions are detected.
