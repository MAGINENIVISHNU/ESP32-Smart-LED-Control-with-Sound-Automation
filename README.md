# ESP32 Smart Lighting Control System with Sound Automation

## Project Overview
This project uses an ESP32 DevKit V1 to control two lights through a 2-channel relay module. A sound sensor detects sound events and automatically toggles Light 1. Light 2 can be controlled manually using a web dashboard.

## Features
- Wi-Fi-based control using a mobile phone or computer
- Web dashboard for controlling two lights
- Sound-triggered automation for Light 1
- Manual ON/OFF control for Light 2
- Enable or disable sound detection
- Relay test controls and live status updates

## Hardware Requirements
- ESP32 DevKit V1
- Digital sound sensor (KY-038)
- 2-channel relay module
- LEDs for low-voltage testing
- Jumper wires
- USB cable and Wi-Fi hotspot/router

## Pin Connections

| Component | ESP32 Pin |
|---|---|
| Sound sensor DO | GPIO 27 |
| Relay IN1 (Light 1) | GPIO 26 |
| Relay IN2 (Light 2) | GPIO 25 |
| Sensor VCC and GND | As required by the module |

## How It Works
1. The ESP32 connects to a Wi-Fi network.
2. Open the ESP32's IP address in a web browser on the same network.
3. Use the dashboard to control the lights.
4. When sound detection is enabled, a detected sound event toggles Light 1.
5. Light 2 can be switched manually from the dashboard.

## Setup Instructions
1. Install the Arduino IDE.
2. Install ESP32 board support.
3. Connect the components according to the pin table.
4. Open the Arduino sketch and enter your Wi-Fi network name and password.
5. Upload the code to the ESP32.
6. Open Serial Monitor to find the ESP32's IP address.
7. Visit that IP address in a browser connected to the same Wi-Fi network.

## Safety
Test with low-voltage LEDs first. Mains-powered AC bulbs can cause electric shock or fire if wired incorrectly. Use properly rated, enclosed equipment and qualified assistance.

## Project Status
Working prototype with Wi-Fi dashboard and sound-triggered lighting control.

## Author
VISHNU
