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

#define TEMP_TOPIC "weather/temperature"
/*_____________________________________________________________*/