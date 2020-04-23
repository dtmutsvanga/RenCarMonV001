#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

#define DEFAULT_WIFI_SSID	"IoT"
#define DEFAULT_WIFI_PASSWD	"10T_2020"
#define SERIAL_NUM	("RenAuto" + String(WiFi.macAddress()))

#define MQTT_HOST	"www.maqiatto.com"
#define MQTT_PORT	1883
#define MQTT_USRNM	"dtmutsvanga@yahoo.com"
#define MQTT_PASSWD	"vision2020"
#define MQTT_CLIENT_ID	"NissanTiida"

#define MAX_COMM_MSG_LEN		50
#define MAX_COMM_MSGS_PER_CYCLE	5
#define MAX_SEND_RETRY			5
#endif // __DEFAULTS_H__
