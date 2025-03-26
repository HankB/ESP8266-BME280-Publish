# ESP8266 Arduino WiFi NTP and MQTT

Project uses <https://github.com/HankB/ESP8266-NTP-MQTT-Time> as a starting point and features:

* WiFi to connect to the outside world.
* MQTT support to publish.
* NTP to establish time and provide time stamps.

## Motivation

Provide a starting point for projects that use these facilities

## Status

Not ready for production - too many problems. Please see issue #1. Now some problems resolved.

Publish timestamp, uptime and temperature, humidity and pressure using a format compatible with (private) HomeAssistant setup. If run long enoiugh, it starts proeducing constant bad values until the ESP8266 is reset.

## Wiring 

The Adafruit library communicates with the BME280 using I2C and assigning `D3`, `D4` as SDA and SCL. The BME280 is powered using the connection labeled as `3V` and grounded using `G`

## Toolchain

Using the Arduino platform via PlatformIO on VS Code (on Linux.)

## Errata

You must provide an `include/secrets.h` that lists WiFi credentials, MQTT broker name and NTP server. See `main.cpp` for more detail.

## helpers

Monitor MQTT, serial traffic, broker log using 

```text
mosquitto_sub -v -t \#
minicom --device /dev/ttyUSB0
tail -f /var/log/mosquitto/mosquitto.log
```
