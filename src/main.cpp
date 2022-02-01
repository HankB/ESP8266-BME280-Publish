#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>   //https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>
#include <PubSubClient.h>

#include "BME280.h" // access BME280.cpp functions

/* secrets.h should define the following as appropriate for your network.
const char* ssid = "...";
const char* password = "...";
const char* mqtt_server = "host.domain"; // "<host>.localddomain" works for me.
const char* ntp_server = "host.domain"; // "<host>.localddomain" works for me.
*/
#include "secrets.h"

#define serial_IO true

// For NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server);

// For MQTT
WiFiClient espClient;
PubSubClient mqtt_client(espClient);

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
  WiFi.hostname("esp.1");
  WiFi.begin(ssid, password);

  // loop while we retry to associate
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

// MQTT callbck - not sure if this is really needed for a publish
// only application. Perhaps have another host publish timestamp
// messages and eliminate thhe need for the NTP client
void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
#if serial_IO
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
#endif
}

void mqtt_reconnect()
{
  // Loop until we're reconnected
  while (!mqtt_client.connected())
  {
#if serial_IO
    Serial.print("Attempting MQTT connection...");
#endif
    // Create a random client ID
    String clientId = "ESP-Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt_client.connect(clientId.c_str()))
    {
#if serial_IO
      Serial.println("connected");
#endif
    }
    else
    {
#if serial_IO
      Serial.print("failed, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" try again in 5 seconds");
#endif
      // Wait 5 seconds before retrying, blip the LED to indicate the situation
      digitalWrite(LED_BUILTIN, LOW); // Turn the LED on
      delay(20);
      digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
      delay(4980);
    }
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output

#if serial_IO
  Serial.begin(115200);
  while (!Serial)
    ; // time to get serial running
  Serial.println("serial started");
#endif

  setup_wifi();
  timeClient.begin();
#if serial_IO
  Serial.println("NTP client started");
#endif

  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(mqtt_callback);

  setup_BME280();
}


// main loop ALA Arduino sketches
void loop()
{
  static unsigned long last_msg_timestamp = 0;
  static const int msg_buffer_size = 150;
  char msg[msg_buffer_size];
  static unsigned long uptime = 0;

  timeClient.update();
  unsigned long current_time_stamp = timeClient.getEpochTime();

  // make sure MQTT still connected
  if (!mqtt_client.connected()) {
    mqtt_reconnect();
  }
  mqtt_client.loop();


  if (current_time_stamp - last_msg_timestamp >= 5)     // time to publish again? Every 5 seconds
  {
    if(last_msg_timestamp != 0 ) uptime += (current_time_stamp-last_msg_timestamp);
    last_msg_timestamp = current_time_stamp;
    snprintf(msg, msg_buffer_size, "{ \"t\": \"%lu\",  \"uptime\": \"%lu\", "
        "\"temperature\": %3.1f, \"pressure\": %3.1f, \"humidity\": %3.1f }",
      current_time_stamp, uptime, getTemperature(), getPressure(), getHumidity());
    mqtt_client.publish("test/timestamp", msg);
#if serial_IO
    Serial.print("Publish message: ");
    Serial.println(msg);
#endif
  }

#if serial_IO
  Serial.print("current timestamp");
  Serial.println(current_time_stamp);
#endif

  delay(1000);
  }
