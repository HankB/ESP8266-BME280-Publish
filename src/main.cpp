#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>   //https://github.com/arduino-libraries/NTPClient
#include <WiFiUdp.h>
#include <PubSubClient.h>

/* secrets.h should define the following as appropriate for your network.
const char* ssid = "...";
const char* password = "...";
const char* mqtt_server = "host.domain"; // "<host>.localddomain" works for me.
const char* ntp_server = "host.domain"; // "<host>.localddomain" works for me.
*/
#include "secrets.h"

#define serial_IO false

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

// MQTT callbck - not sure if this is really needed for a publish only application
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
    String clientId = "ESP8266Client-";
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
#endif

  setup_wifi();
  timeClient.begin();

  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(mqtt_callback);
}

void loop()
{
  static unsigned long lastMsg = 0;
  static const int msg_buffer_size = 50;
  char msg[msg_buffer_size];
  static time_t local_timestamp = 0;     // count millis()
  static unsigned long  previous_millis;
  static unsigned long uptime = 0;

  unsigned long before = micros();
  timeClient.update();
  unsigned long epoch = timeClient.getEpochTime();
  unsigned long after = micros();

  if (local_timestamp == 0)
  {
    local_timestamp = epoch; // initialize local counter
    previous_millis = millis(); // and seconds rollover couner
  }

  if (millis() - previous_millis >= 1000) // TODO: handle rollover in 49 days.
  {
    local_timestamp++;
    previous_millis = millis();
    uptime++;
  }

  if (!mqtt_client.connected()) {
    mqtt_reconnect();
  }
  mqtt_client.loop();


  unsigned long now = millis();   // will roll over in 49 days. TODO: handle gracefully.
  if (now - lastMsg > 5000)   // time to publish again?
  {
    lastMsg = now;
    snprintf(msg, msg_buffer_size, "t:%lu, et:%lu, drift:%lld, uptime:%lu", epoch, after-before, local_timestamp-epoch, uptime);
    mqtt_client.publish("timestamp", msg);
#if serial_IO
    Serial.print("Publish message: ");
    Serial.println(msg);
#endif
  }

#if serial_IO
  Serial.print(epoch);
  Serial.print(" ");
  Serial.println(after-before);
#endif

  delay(1000);
  }
