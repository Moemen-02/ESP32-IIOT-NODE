/*____________________ WiFi Authentification __________________*/

#define WIFI_SSID_1 " wifi_ssid "
#define WIFI_PASSWORD_1 " wifi_password "
/*_____________________________________________________________*/


/*__________________ MQTT Server Authentification _____________*/

const char* MQTT_SERVER = " mqtt_server_url ";
#define MQTT_PORT 1883
const char* MQTT_CLIENT_ID = "ESP32_";
const char* MQTT_SERVER_NAME = " mqtt_server_name ";
const char* MQTT_SERVER_PASSWORD = " mqtt_server_password ";
/*_____________________________________________________________*/


/*__________________ InfluxDB Authentification ________________*/

#define INFLUXDB_URL " influxdb_server_url "
#define INFLUXDB_TOKEN " influxdb_token "
#define INFLUXDB_ORG " influxdb_organisarion_name "
#define INFLUXDB_BUCKET " influxdb_bucket_name "
#define DEVICE "ESP32"
#define TZ_INFO "CET-1"
#define NTP_SERVER "africa.pool.ntp.org"
/*_____________________________________________________________*/


/*__________________________ TIMEOUTS _________________________*/

#define SAMPLING_RATE_MIN 5
#define WDT_TIMEOUT_MIN 10
#define WIFI_RETRY_ATTEMPT_S 5
#define MQTT_RETRY_ATTEMPT_S 5
#define CHECK_WIFI_TIMEOUT_MIN 3
#define CHECK_BROKER_TIMEOUT_MIN 3

#define WIFI_RETRY_ATTEMPT_MS WIFI_RETRY_ATTEMPT_S*1000
#define MQTT_RETRY_ATTEMPT_MS MQTT_RETRY_ATTEMPT_S*1000
#define CHECK_WIFI_TIMEOUT_MS CHECK_WIFI_TIMEOUT_MIN*60*1000
#define CHECK_BROKER_TIMEOUT_MS CHECK_BROKER_TIMEOUT_MIN*60*1000
#define SAMPLING_RATE_MS SAMPLING_RATE_MIN*60*1000

#define DEEP_SLEEP_TIME_US SAMPLING_RATE_MS*1000
#define DEEP_SLEEP_TIME_MIN SAMPLING_RATE_MS/1000/60
#define WDT_TIMEOUT_S WDT_TIMEOUT_MIN*60
/*_____________________________________________________________*/


/*__________________________ MQTT TOPICS ______________________*/

#define TEMP_TOPIC " mqtt_topic "
/*_____________________________________________________________*/
