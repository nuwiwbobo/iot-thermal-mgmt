# IoT Thermal Management System

ESP32-based IoT thermal management system with BMP280 (I2C) temperature sensor, L298N motor driver, dual-mode control (AUTO/MANUAL) via web dashboard and physical buttons, and data logging.

## Features

- **Sensor**: BMP280 (I2C) — digital temperature & pressure, ±1.0°C accuracy
- **Actuator**: 12V DC fan via L298N motor driver (PWM speed control, 5kHz, 8-bit)
- **Control modes**:
  - **AUTO**: P-controller with setpoint and hysteresis (±0.5°C deadband)
  - **MANUAL**: Direct PWM control via web slider
- **Web dashboard**: Real-time charts (temp & fan), live JSON status, sliders, data table
- **Data logging**: 100-entry ring buffer in RAM + LittleFS persistence (CSV export)
- **WiFi**: STA mode with AP fallback (`ThermalCtrl-AP` / `thermal123`)
- **Safety**: 100% fan override at >60°C, shutdown at >75°C
- **Inputs**: 2 physical buttons (mode toggle, value adjust) with debounce + long-press

## Hardware

| Component | Pin |
|-----------|-----|
| BMP280 SDA | GPIO21 (I2C) |
| BMP280 SCL | GPIO22 (I2C) |
| L298N ENA | GPIO16 (PWM) |
| L298N IN1 | GPIO17 (forward HIGH) |
| L298N IN2 | GPIO18 (LOW) |
| Button Mode | GPIO4 (internal pull-up) |
| Button Value | GPIO5 (internal pull-up) |
| Battery monitor | GPIO35 (ADC) |

Power: 3S Li-ion (11.1V) → L298N VS → 5V regulator → ESP32 VIN. Common GND.

## Build & Flash

```bash
source ~/esp/esp-idf/export.sh
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

## API Endpoints

- `GET /` — dashboard HTML
- `GET /api/status` — JSON status
- `POST /api/control` — JSON control (`{"mode":0|1,"setpoint":float,"fan":0-100}`)
- `GET /api/log/csv` — download CSV
- `GET /api/log/plt` — download GNUplot script
- `POST /api/log/clear` — clear log

## Configuration

All pin/timing/threshold constants are in [`main/config.h`](main/config.h).

## Project Structure

```
main/
├── CMakeLists.txt
├── config.h           — Pin/timing/threshold constants
├── main.c             — Entry point, 3 ESP timers, super-loop
├── sensor.c/.h        — BMP280 I2C driver + temp compensation
├── actuator.c/.h      — L298N PWM fan control + PID
├── input.c/.h         — Button debounce + press handling
├── storage.c/.h       — Ring buffer + LittleFS CSV logging
├── network.c/.h       — WiFi + HTTP REST API
└── dashboard.h        — Embedded HTML/CSS/JS dashboard
partitions.csv         — Flash layout (4MB: NVS + app + SPIFFS)
diagram.mmd            — Wiring diagram (Mermaid block-beta)
diagram-flow.mmd       — Wiring diagram (Mermaid flowchart)
report/main.pdf        — Capstone report
```

## License

Capstone project.
