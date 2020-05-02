#ifndef __RACM_WIFI_H__
#define __RACM_WIFI_H__
#include <stdint.h>
#include "defaults.h"

typedef void (*WiFi_connect_CB)(void);
typedef void (*WiFi_reconnect_CB)(void);

uint8_t wifi_add_connect_CB(WiFi_connect_CB fxn);
uint8_t wifi_add_disconnect_CB(WiFi_reconnect_CB fxn);
uint8_t wifi_init(char *wifi_ssid, uint8_t ssid_len, char *wifi_passwd, uint8_t pswd_len);
uint8_t wifi_conn_status();
#endif // __RACM_WIFI_H__
