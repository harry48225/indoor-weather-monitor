// Basic demo for reading Humidity and Temperature
#include <Wire.h>
#include "WiFi.h"
#include "ESPmDNS.h"
#include "PubSubClient.h"
#include <Adafruit_HTS221.h>
#include <Adafruit_Sensor.h>

#define HOSTNAME "temperature-tentacle"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char* server = "192.168.0.69";

Adafruit_HTS221 hts;
void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit HTS221 test!");

  // Try to initialize!
  if (!hts.begin_I2C()) {
    Serial.println("Failed to find HTS221 chip");
    while (1) { delay(10); }
  }
  Serial.println("HTS221 Found!");

  Serial.print("Data rate set to: ");
  switch (hts.getDataRate()) {
   case HTS221_RATE_ONE_SHOT: Serial.println("One Shot"); break;
   case HTS221_RATE_1_HZ: Serial.println("1 Hz"); break;
   case HTS221_RATE_7_HZ: Serial.println("7 Hz"); break;
   case HTS221_RATE_12_5_HZ: Serial.println("12.5 Hz"); break;
  }

  // connect to the wifi network

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("trying to connect to: %s with password: %s \n", WIFI_SSID, WIFI_PASS);

  WiFi.setHostname(HOSTNAME);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.printf("\n connected!, ip: ");
  Serial.print(WiFi.localIP());

  // setup mDNS - might not be nessesary

  if (!MDNS.begin(HOSTNAME)) {
        Serial.println("Error setting up MDNS responder!");
        while(1){
            delay(1000);

            // Maybe add a pin blink here
        }
  }

  // connect to mqtt broker

  mqttClient.setServer(server, 1883);

  // attempt to connect to the broker with "temperature_tentacle" id

  if (mqttClient.connect(HOSTNAME)) {
    Serial.println("Connected to mqtt broker established");
  }
  else {
    Serial.println("failed connection");
  }

}

void loop() {

  mqttClient.loop();
  
  sensors_event_t temp;
  sensors_event_t humidity;
  hts.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.printf("\r Temperature: %f degrees C, Humidity: %f % rH", temp.temperature, humidity.relative_humidity);

  // attempt to publish the data

  mqttClient.publish("temperature", String(temp.temperature).c_str());
  mqttClient.publish("relative_humidity", String(humidity.relative_humidity).c_str());

  delay(500);
}