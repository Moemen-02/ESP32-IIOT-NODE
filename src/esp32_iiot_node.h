#ifndef _ESP_IIOT_NODE_
#define _ESP_IIOT_NODE_

#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <InfluxDbClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <esp_task_wdt.h>
#include <stdarg.h>
#include "def.h"
#include "auth.h" // Replace with #include "auth_1.h"
                  // and add your wifi, MQTT server and InfluxDB server credentials to auth_1.h

TaskHandle_t readTemperatureandHumidity_handle = NULL;
TaskHandle_t connectToBroker_task_handle = NULL;
TaskHandle_t pullEpochTime_task_handle = NULL;

DynamicJsonDocument doc(256);

void connectToWifi();
void connectToBroker();
void keepWifiAlive_task(void *);
void keepBrokerAlive_task(void * parameters);
void callback(char* topic, byte* payload, unsigned int length);
void readTemperatureandHumidity_task(void *);
void goToDeepSleep();
void pubToMQTT(float, float, unsigned long long);
void sendToInflux(float, float, unsigned long long);
unsigned long long getTime();
unsigned long long Get_Epoch_Time_ms();
unsigned long long Get_Epoch_Time_s();


WiFiClient esp32_WiFiClient;
PubSubClient mqtt_client(MQTT_SERVER, MQTT_PORT, &callback, esp32_WiFiClient);
InfluxDBClient influx_client(INFLUXDB_URL,INFLUXDB_ORG,INFLUXDB_BUCKET,INFLUXDB_TOKEN);
Point DHT11_sensor("weather_f");
DHT ESP32_dht11_sensor(14, DHT11);

#endif