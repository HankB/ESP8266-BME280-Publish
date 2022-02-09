# ESP8266 BME280 Publish

Project uses <https://github.com/HankB/ESP8266-NTP-MQTT-Time> as a starting point and features:

* WiFi to connect to the outside world.
* MQTT support to publish.
* NTP to establish time and provide time stamps.
* BME280 support to read the sensor (temperature, humidity and pressure.)

## Motivation

Provide an alternative to using a Raspberry Pi Zero W to monitor these conditions.

## Status

Not ready for production - too many problems. Please see issue #1.

Publish timestamp, uptime and temperature.

## Toolchain

Using the Arduino platform via PlatformIO on VS Code (on Linux.)

## Errata

You must provide an `include/secrets.h` that lists WiFi credentials, MQTT broker name and NTP server. See `main.cpp` for more detail.

Only works if serial ouitput is enabled.

## helpers

Monitor MQTT, serial traffic, broker log using 

```text
mosquitto_sub -v -t \#
minicom --device /dev/ttyUSB0
tail -f /var/log/mosquitto/mosquitto.log
```
