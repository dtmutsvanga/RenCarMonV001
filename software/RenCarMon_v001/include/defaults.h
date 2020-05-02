#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

#define WIFI_SSID	"IoT"
#define WIFI_PASSWD	"10T_2020"
#define WIFI_MAX_LEN_SSID	15
#define WIFI_MAX_LEN_PSWD	10
#define SERIAL_NUM	("RenAuto" + String(WiFi.macAddress()))
#define WIFI_MAX_CONNECT_CBs	5
#define WIFI_MAX_RECONNECT_CBs	5
#define WIFI_RECONNECT_PERIOD	2

#define MQTT_HOST	"www.maqiatto.com"
#define MQTT_PORT	1883
#define MQTT_USRNM	"dtmutsvanga@yahoo.com"
#define MQTT_PASSWD	"vision2020"
#define MQTT_CLIENT_ID	"NissanTiida"
#define MQTT_RECONNECT_PRD_s		2

#define MAX_COMM_MSG_LEN		50
#define MAX_COMM_MSGS_PER_CYCLE	5
#define MAX_SEND_RETRY			5
#define MAX_CMD_QUEUE 20

#define MAX_ENG_RUN_TIME_MIN	30
#define MAX_TIMER_MODE_TIME_MIN	43200		
#define LOG_BAUD_RATE 9600
#define LOG_MAX_MSG_LEN	30
#endif // __DEFAULTS_H__
