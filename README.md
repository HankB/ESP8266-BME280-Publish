# ESP8266 NTP MQTT Time

Explore timekeeping on the ESP8266 using NTP and producing a value equivalent to UNIX time() (time since midnight January 1, 1970 GMT.)

## Motivation

Provide a means to schedule operations down to the second and report time associated with a payload via MQTT.

## Status

Some coding. Does nothing useful yet.

* connects to access point

## Toolchain

Using the Arduino platform via PlatformIO on VS Code (on Linux.)

## Errata

You must provide an `include/secrets.h` that lists WiFi credentials and MQTT broker name. See `main.cpp` for more detail.
