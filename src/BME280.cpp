/* Code to fetch values from the BME280 temp, himidity and pressure sensor
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

static Adafruit_BME280 bme; // I2C

void setup_BME280(void) {
  // configure BME280/I2C
  Wire.begin(D3, D4); // Make sure you have D3 & D4 hooked up to the BME280
  Wire.setClock(100000);
  unsigned status = bme.begin(0x76);
  if (!status) {
#if serial_IO
      Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
      Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      while (1) delay(10);
#else
      while(1) {
        digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on 
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED off 
        delay(90);
      }
#endif
  }
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
