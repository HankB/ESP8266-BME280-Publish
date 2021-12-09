#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>   //https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>

/* secrets.h should define the following as appropriate for your network.
const char* ssid = "...";
const char* password = "...";
const char* mqtt_server = "host.domain"; // "<host>.localddomain" works for me.
const char* ntp_server = "host.domain"; // "<host>.localddomain" works for me.
*/
#include "secrets.h"

#define serial_IO true

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server);

void setup_wifi(void)
{

  delay(10);
  // We start by connecting to a WiFi network
#if serial_IO
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
#if serial_IO
    delay(500);
    Serial.print(".");
#else
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
    delay(400);
#endif
  }
#if serial_IO
  Serial.println("\r\nConnected");
#endif
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

#if serial_IO
  Serial.begin(115200);
  while (!Serial)
    ; // time to get serial running
#endif

  setup_wifi();
  timeClient.begin();
}

void loop()
{
  unsigned long before = micros();
  timeClient.update();
  unsigned long epoch = timeClient.getEpochTime();
  unsigned long after = micros();


  Serial.print(epoch);
  Serial.print(" ");
  Serial.println(after-before);

  delay(1000);
}
