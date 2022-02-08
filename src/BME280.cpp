/* Code to fetch values from the BME280 temp, himidity and pressure sensor
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "BME280.h" // access BME280.cpp functions

static Adafruit_BME280 bme; // I2C

unsigned setup_BME280(void) {
  // configure BME280/I2C
  Wire.begin(D3, D4); // Make sure you have D3 & D4 hooked up to the BME280
  Wire.setClock(100000);
  return bme.begin(0x76);
}

// Calls to fetch readings

double getTemperature(void) {
    return bme.readTemperature()/5*9+32.0;
}

double getHumidity(void) {
    return bme.readHumidity();
}

double getPressure(void) {
    return bme.readPressure()/100;
}
