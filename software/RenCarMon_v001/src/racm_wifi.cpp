#include "racm_wifi.h"
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include "logging.h"


typedef struct
{
	char SSID[WIFI_MAX_LEN_SSID];
	char pswd[WIFI_MAX_LEN_PSWD];

	uint8_t reconnect_CBs_ctr;
	uint8_t connect_CBs_ctr;
	WiFi_reconnect_CB reconnect_cbs[WIFI_MAX_RECONNECT_CBs];
	WiFi_connect_CB connect_cbs[WIFI_MAX_CONNECT_CBs];
	WiFiEventHandler wifiConnectHandler;
	WiFiEventHandler wifiDisconnectHandler;
	Ticker wifiReconnectTimer;
	uint8_t cnnctd;
} WiFi_mngr_t;

static WiFi_mngr_t WiFi_mngr= {.SSID={0}, .pswd={}, .reconnect_CBs_ctr=0, .connect_CBs_ctr=0, .reconnect_cbs={NULL}, .connect_cbs={NULL}};

void connectToWifi(char *ssid, char *pswd)
{
	WiFi.begin(ssid, pswd);
}

void connectToWifi_CB()
{
	log_msg(LOG_LVL_INFO, SYS_COMM_WIFI, "WiFi Connecting...");
	log_msg_append(WiFi_mngr.SSID);
	log_msg_append(" Pswd: ");
	log_msg_append(WiFi_mngr.pswd);
	connectToWifi(WiFi_mngr.SSID, WiFi_mngr.pswd);
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
	log_msg(LOG_LVL_CRITICAL, SYS_COMM_WIFI, "WiFi Connected");
	for (int i = 0; i < WiFi_mngr.connect_CBs_ctr; i++)
	{
		log_msg(LOG_LVL_INFO, SYS_COMM_WIFI, "Calling Wifi callback No.");
		log_msg_append_i(i);
		WiFi_mngr.connect_cbs[i]();
	}
	WiFi_mngr.cnnctd=1;
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
	WiFi_mngr.cnnctd=0;
	log_msg(LOG_LVL_CRITICAL, SYS_COMM_WIFI, "Disconnected from Wi-Fi.");

	for (uint8_t i = 0; i < WiFi_mngr.reconnect_CBs_ctr; i++)
	{
		WiFi_mngr.reconnect_cbs[i]();
	}

	WiFi_mngr.wifiReconnectTimer.once(2, connectToWifi_CB);
}


uint8_t wifi_init(const char *wifi_ssid, uint8_t ssid_len,const char *wifi_passwd, uint8_t pswd_len)
{
	log_msg(LOG_LVL_INFO, SYS_COMM_WIFI, "WiFi Init. SSID: ");
	log_msg_append(wifi_ssid);
	log_msg_append(" ssid len= ");
	log_msg_append_i(ssid_len);
	log_msg_append("\t Passwd: ");
	log_msg_append(wifi_passwd);
	log_msg_append(" passwd len= ");
	log_msg_append_i(pswd_len);
	
	memcpy(WiFi_mngr.SSID, wifi_ssid, ssid_len);
	memcpy(WiFi_mngr.pswd, wifi_passwd, pswd_len);
	WiFi_mngr.wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
	WiFi_mngr.wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
	WiFi.mode(WIFI_AP_STA);
	WiFi.softAP(WIFI_AP_SSID,WIFI_AP_PASSWD, 1, 0, 2);
	connectToWifi(WiFi_mngr.SSID, WiFi_mngr.pswd);
	return true;
}

uint8_t wifi_conn_status() 
{
	return WiFi_mngr.cnnctd;
}

uint8_t wifi_add_connect_CB(WiFi_connect_CB fxn)
{
	if (WiFi_mngr.connect_CBs_ctr >= WIFI_MAX_CONNECT_CBs || !fxn)
		return 0;
	WiFi_mngr.connect_cbs[WiFi_mngr.connect_CBs_ctr] = fxn;
	WiFi_mngr.connect_CBs_ctr++;
	log_msg(LOG_LVL_INFO, SYS_COMM_WIFI, "WiFi connect callback added index = ");
	log_msg_append_i(WiFi_mngr.connect_CBs_ctr);
	return 1;
}

uint8_t wifi_add_disconnect_CB(WiFi_reconnect_CB fxn)
{
	if (WiFi_mngr.reconnect_CBs_ctr >= WIFI_MAX_RECONNECT_CBs || !fxn)
		return 0;
	WiFi_mngr.reconnect_cbs[WiFi_mngr.reconnect_CBs_ctr] = fxn;
	WiFi_mngr.reconnect_CBs_ctr++;
	return 1;
}
