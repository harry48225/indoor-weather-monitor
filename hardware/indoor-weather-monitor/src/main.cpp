// Basic demo for reading Humidity and Temperature
#include <Wire.h>
#include "WiFi.h"
#include "ESPmDNS.h"
#include "PubSubClient.h"
#include <Adafruit_HTS221.h>
#include <Adafruit_Sensor.h>

#define HOSTNAME "temperature-tentacle"

#define SENSOR_DELAY 20000

WiFiClient espClient;
PubSubClient mqttClient(espClient);

const char* SERVER_HOSTNAME = "helevorn";

float temperature_offset = -1;


Adafruit_HTS221 hts;

void ensure_connected_to_wifi_and_server() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  while (!mqttClient.connect(HOSTNAME)) {
    Serial.print(".");
  }
  

  
}

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

  hts.setDataRate(HTS221_RATE_ONE_SHOT);

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
  Serial.println();

  // setup mDNS
  if (!MDNS.begin(HOSTNAME)) {
        Serial.println("Error setting up MDNS responder!");
        while(1){
            delay(1000);

            // Maybe add a pin blink here
        }
  }

  // find the ip of the server, by resolving the hostname

  struct ip4_addr server;
  server.addr = 0;

  esp_err_t err = mdns_query_a(SERVER_HOSTNAME, 20000, &server);
  if(err){
    if(err == ESP_ERR_NOT_FOUND){
        Serial.println("Host was not found!");
        return;
    }
    Serial.println("Query Failed");
    return;
  }
  // connect to mqtt broker

  char* server_ip;
  asprintf(&server_ip, IPSTR, IP2STR(&server));
  Serial.printf("found: %s.local at ip: %s", SERVER_HOSTNAME, server_ip);
  Serial.println();

  mqttClient.setServer(server_ip, 1883);

  // attempt to connect to the broker with "temperature_tentacle" id
  Serial.println("attempting connection to MQTT broker");
  while (!mqttClient.connect(HOSTNAME)) {
    Serial.print(".");
  }

  Serial.println("Connected to mqtt broker established");

}

void loop() {

  mqttClient.loop();
  
  sensors_event_t temp;
  sensors_event_t humidity;
  hts.setActive(true);
  hts.begin_I2C();
  delay(100);
  while (!hts.getEvent(&humidity, &temp)) {// populate temp and humidity objects with fresh data
    delay(100);
    Serial.println("waiting");
  }
  hts.setActive(false);

  float temperature = temp.temperature - 4; // Seems 4C too high
  float relative_humidity = humidity.relative_humidity;

  Serial.printf("\r Temperature: %f degrees C, Humidity: %f % rH", temperature, relative_humidity);

  // attempt to publish the data

  mqttClient.publish("bedroom/shelf/temperature", String(temperature).c_str());
  mqttClient.publish("bedroom/shelf/relative_humidity", String(relative_humidity).c_str());

  int counter = 0;

  while (counter < SENSOR_DELAY) {

    mqttClient.loop();

    ensure_connected_to_wifi_and_server();
    counter += 10;
    delay(10);
  }
}