#ifndef __RACM_MQTT_H__
#define __RACM_MQTT_H__
#include "defaults.h"
#include <stdint.h>


void mqtt_init(const char* mqtt_host, uint16_t mqtt_port,const char* mqtt_client_id, \
								const char* mqtt_usrnm, const char* mqtt_passwd);
//void mqtt_reg_msg_rx_cb(MQTT_Rx_CB cb);
uint8_t mqtt_read(uint8_t *data);
uint8_t mqtt_write(uint8_t *data, int len);
void MqttWiFiDisconnectCB();
void MqttWiFiConnectCB();
#endif // __RACM_MQTT_H__
