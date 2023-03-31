#include "esp32_iiot_node.h"
#include "compiler_flags.h"

void setup() {
  Serial.begin(115200);
  esp_task_wdt_init(WDT_TIMEOUT_S, true);
  esp_task_wdt_add(NULL);
  #ifdef MANUAL_TIME
  configTime(0, 0, NTP_SERVER);
  influx_client.setWriteOptions(WriteOptions().writePrecision(WritePrecision::S));
  #endif
  WiFi.mode(WIFI_STA);
 
  #ifndef ECO_MODE
  xTaskCreatePinnedToCore(
    keepWifiAlive_task,
    "Wifi_task",
    4096,
    NULL,
    2,
    NULL,
    CONFIG_ARDUINO_RUNNING_CORE
  );

  xTaskCreate(
    keepBrokerAlive_task,
    "Broker_task",
    4096,
    NULL,
    1,
    &connectToBroker_task_handle
  );
  #endif

  xTaskCreate(
    readTemperatureandHumidity_task,
    "Temp_task",
    4096,
    NULL,
    5,
    &readTemperatureandHumidity_handle
  );
}

void loop(){
  esp_task_wdt_reset();
}

void readTemperatureandHumidity_task(void * parameters){
  float temperature_f, humidity_f;
  unsigned long long epochTime = 0;

  esp_task_wdt_add(NULL);
  ESP32_dht11_sensor.begin();
  DHT11_sensor.addTag("device", DEVICE);
  //DHT11_sensor.addTag("SSID", WIFI_SSID_1);
  #ifndef ECO_MODE
  vTaskSuspend(NULL);
  #endif
  for(;;){
    #ifdef VERBOSE
    Serial.println();
    Serial.println("[TEMP] CONNECTING:");
    #endif

    if(WiFi.status() != WL_CONNECTED)
      connectToWifi();  
    if(!mqtt_client.connected())
      connectToBroker();
    
    #ifdef VERBOSE
    Serial.println("[TEMP] CONNECTED");
    Serial.println("[TEMP] SENDING:");
    #endif
    #ifdef MANUAL_TIME
    while((epochTime = Get_Epoch_Time_s()) < 1680299493);
    #endif
    do{
      vTaskDelay(30);
      humidity_f = ESP32_dht11_sensor.readHumidity();
      temperature_f = ESP32_dht11_sensor.readTemperature();
    } while ( isnan(humidity_f) || isnan(temperature_f));
    pubToMQTT(temperature_f,humidity_f, epochTime);
    sendToInflux(temperature_f,humidity_f, epochTime);

    #ifdef VERBOSE
    Serial.println("[TEMP] DATA SENT");
    Serial.println();
    #endif

    esp_task_wdt_reset();
    #ifdef ECO_MODE
    goToDeepSleep();
    #else
    vTaskDelay(SAMPLING_RATE_MS / portTICK_PERIOD_MS);
    #endif
  }
}

/* ______________ DEEP SLEEP FUNCTION DEFINITIONS _____________ */

void goToDeepSleep(){
  #ifdef VERBOSE
  Serial.print("[ESP] GOING TO DEEP SLEEP FOR ");
  Serial.print(DEEP_SLEEP_TIME_MIN);
  Serial.print("min ...\n");
  #endif

  esp_sleep_enable_timer_wakeup(DEEP_SLEEP_TIME_US);
  esp_deep_sleep_start();
}


/* ______________ WIFI & MQTT FUNCTION DEFINITIONS _____________ */

void connectToWifi(){
  byte attempt = 0;

  #ifdef VERBOSE
  Serial.println("[WIFI] CONNECTING ...");
  #endif
  WiFi.begin(WIFI_SSID_1, WIFI_PASSWORD_1);
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  while(WiFi.status() != WL_CONNECTED){
    attempt++;
    #ifdef VERBOSE
    Serial.println("[WIFI] CONNECTION FAILED");
    Serial.print("[WIFI] RETRYING (");
    Serial.print(attempt);
    Serial.print(") IN ");
    Serial.print(WIFI_RETRY_ATTEMPT_S);
    Serial.println("s");
    #endif
    if (attempt > 5) {
      #ifdef VERBOSE
      Serial.print("[WIFI] CONNECTION TIMEOUT, RETRYING IN ");
      Serial.print(DEEP_SLEEP_TIME_MIN);
      Serial.println("min");
      #endif
      goToDeepSleep();
    }
    else{
      vTaskDelay(WIFI_RETRY_ATTEMPT_MS / portTICK_PERIOD_MS);
      #ifdef VERBOSE
      Serial.println("[WIFI] CONNECTING ...");
      #endif
    }
  }
  #ifdef VERBOSE
  Serial.print("[WIFI] CONNECTED TO ");
  Serial.println(WiFi.SSID());
  #endif
}

void connectToBroker() {
  byte attempt = 0;

  #ifdef VERBOSE
  Serial.println("[MQTT] CONNECTING ...");
  #endif

  while (!mqtt_client.connect(MQTT_CLIENT_ID)){
    attempt++;

    #ifdef VERBOSE
    Serial.print("[MQTT] FAILED WITH STATUS CODE = ");
    Serial.print(mqtt_client.state());
    Serial.print("[MQTT] RETRYING (");
    Serial.print(attempt);
    Serial.print(") IN ");
    Serial.print(MQTT_RETRY_ATTEMPT_S);
    Serial.println("s");
    #endif
    if (attempt > 5) {
      #ifdef VERBOSE
      Serial.println("[MQTT] CONNECTION TIMEOUT, RETRYING IN ");
      Serial.print(DEEP_SLEEP_TIME_MIN);
      Serial.println("min");
      #endif
      goToDeepSleep();
    }
    else {
      vTaskDelay(MQTT_RETRY_ATTEMPT_MS / portTICK_PERIOD_MS);
      #ifdef VERBOSE
      Serial.println("[MQTT] CONNECTING ...");
      #endif
    }
  }
  #ifdef VERBOSE
  Serial.print("[MQTT] CONNECTED TO ");
  Serial.print(MQTT_SERVER);
  Serial.print(":");
  Serial.println(MQTT_PORT);
  #endif
  mqtt_client.subscribe(TEMP_TOPIC);
}

void keepWifiAlive_task(void * parameters){
  esp_task_wdt_add(NULL);
  connectToWifi();
  vTaskResume(connectToBroker_task_handle);
  for(;;){
    vTaskDelay(CHECK_WIFI_TIMEOUT_MS / portTICK_PERIOD_MS);
    if(WiFi.status() == WL_CONNECTED){
      #ifdef VERBOSE
      Serial.print("[WIFI] STATUS: CONNECTED TO ");
      Serial.println(WiFi.SSID());
      #endif
    }
    else{
      #ifdef VERBOSE
      Serial.println("[WIFI] STATUS: DISCONNECTED");
      #endif
      connectToWifi();
    }
    esp_task_wdt_reset();
  }
}

void keepBrokerAlive_task(void * parameters){
  esp_task_wdt_add(NULL);
  vTaskSuspend(NULL);
  connectToBroker();
  vTaskResume(readTemperatureandHumidity_handle);
  for(;;){
    vTaskDelay(CHECK_BROKER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if(mqtt_client.connected()){
      #ifdef VERBOSE
      Serial.print("[MQTT] STATUS: CONNECTED TO ");
      Serial.print(MQTT_SERVER);
      Serial.print(":");
      Serial.println(MQTT_PORT);
      #endif
    }
    else{
      #ifdef VERBOSE
      Serial.println("[MQTT] STATUS: DISCONNECTED");
      #endif
      connectToBroker();
    }
    esp_task_wdt_reset();
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  #ifdef VERBOSE
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  #endif
  for (int i=0;i<length;i++) {
    #ifdef VERBOSE
    Serial.print((char)payload[i]);
    #endif
  }
  #ifdef VERBOSE
  Serial.println();
  #endif
}


/* ___________________ DATA SENDING FUNCTIONS __________________ */

void pubToMQTT(float temperature_f, float humidity_f, unsigned long long timestamp){
  char payload[256];
  doc["temperature"] = temperature_f;
  doc["humidity"] = humidity_f;
  #ifdef MANUAL_TIME
  doc["time"] = timestamp;
  #endif
  size_t n = serializeJson(doc, payload);
  if(mqtt_client.publish(TEMP_TOPIC, payload, n)){
    #ifdef VERBOSE
    Serial.print("[MQTT] NEW DATA (");
    Serial.print(temperature_f);
    Serial.print("C/");
    Serial.print(humidity_f);
    Serial.print("%");
    #ifdef MANUAL_TIME
    Serial.print(" ");
    Serial.print(timestamp);
    #endif
    Serial.print(") PUBLISHED TO ");
    Serial.println(TEMP_TOPIC);
  }
  else {
    Serial.print("[MQTT] ERROR DATA NOT PUBLISHED (");
    Serial.print(temperature_f);
    Serial.print("C/");
    Serial.print(humidity_f);
    Serial.print("%");
    #ifdef MANUAL_TIME
    Serial.print(" ");
    Serial.print(timestamp);
    #endif
    Serial.println(")");
  #endif
  }
}

void sendToInflux(float temperature_f, float humidity_f, unsigned long long timestamp){  
  DHT11_sensor.clearFields();
  DHT11_sensor.addField("temperature_f", temperature_f);
  DHT11_sensor.addField("humidity_f", humidity_f);
  #ifdef MANUAL_TIME
  DHT11_sensor.setTime(timestamp);
  #endif

  if (!influx_client.writePoint(DHT11_sensor)) {
    #ifdef VERBOSE
    Serial.print("[INFLUXDB] ERROR DATA NOT SENT (");
    Serial.print(temperature_f);
    Serial.print("C/");
    Serial.print(humidity_f);
    Serial.print("%");
    #ifdef MANUAL_TIME
    Serial.print(" ");
    Serial.print(timestamp);
    #endif
    Serial.println(")");
    Serial.println(influx_client.getLastErrorMessage());
  }
  else {
    Serial.print("[INFLUXDB] NEW DATA (");
    Serial.print(temperature_f);
    Serial.print("C/");
    Serial.print(humidity_f);
    Serial.print("%");
    #ifdef MANUAL_TIME
    Serial.print(" ");
    Serial.print(timestamp);
    #endif
    Serial.print(") SENT TO ");
    Serial.println(INFLUXDB_URL);
    #endif
  }
}


/* _______________________ TIME FUNCTIONS ______________________ */

unsigned long long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
    return(0);
  time(&now);
  return now;
}

inline unsigned long long Get_Epoch_Time_ms() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (unsigned long long)tv.tv_sec*1000LL+tv.tv_usec/1000;
}

inline unsigned long long Get_Epoch_Time_s() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (unsigned long long)tv.tv_sec;
}