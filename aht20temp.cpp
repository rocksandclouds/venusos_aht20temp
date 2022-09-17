#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include "DFRobot_AHT20.h"

// WiFi parameters to be configured
const char* ssid = "SSID"; // Write here your router's username
const char* password = "PASSWORD"; // Write here your router's password

// MQTT Broker
const char *mqtt_broker = "192.168.66.188";
const int mqtt_port = 1883;
const char *topic_write_temp = "W/VRM-ID/temperature/4/Temperature";
const char *topic_write_humi = "W/VRM-ID/temperature/4/Humidity";

WiFiClient espClient;
PubSubClient client(espClient);
DFRobot_AHT20 aht20;


void setup() {
  // Set  serial baud to 9600;
  Serial.begin(9600);
  while(!Serial){
      }
      uint8_t status;
      while((status = aht20.begin()) != 0){
        Serial.print("AHT20 sensor initialization failed. error status : ");
        Serial.println(status);
        delay(1000);
      }
  
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  }

void loop() {
  client.loop();

// send messages to MQTT;

  //connecting to the mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the MQTT broker\n", client_id.c_str());
      if (client.connect(client_id.c_str())) {
          Serial.println("MQTT broker connected");
          client.publish("device/fe004/Status","{\"clientId\": \"fe004\", \"connected\": 1, \"version\": \"v0.9\", \"services\": {\"t1\": \"temperature\"}}");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
      if(aht20.startMeasurementReady(true)){
        Serial.print("temperature(-40~85 C): ");
        Serial.print(aht20.getTemperature_C());
        Serial.print(" C, ");
        Serial.print("humidity(0~100): ");
        Serial.print(aht20.getHumidity_RH());
        Serial.println(" %RH");
      }


  char msgTemperature[30];
  char valTemperature[10];
  dtostrf(aht20.getTemperature_C(), 8, 1, valTemperature);
  sprintf(msgTemperature, "{\"value\": %s }", valTemperature);
  client.publish(topic_write_temp, msgTemperature);
  Serial.print("Temperature: ");
  Serial.println(valTemperature);
  char msgHumidity[30];
  char valHumidity[10];
  dtostrf(aht20.getHumidity_RH(), 8, 1, valHumidity);
  sprintf(msgHumidity, "{\"value\": %s }", valHumidity);
  client.publish(topic_write_humi, msgHumidity);
  delay(8000);
}
