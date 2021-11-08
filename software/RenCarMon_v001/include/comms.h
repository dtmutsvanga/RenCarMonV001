#ifndef __COMMS_H__
#define __COMMS_H__
#include <stdint.h>
#include "defaults.h"
#include "cmd_queue.h"
#include "fsm_states.h"

typedef void (*MQTT_Rx_CB)(void);

// Communications
typedef struct comms_data
{
	char WiFi_SSID[WIFI_MAX_LEN_SSID];
	char WiFi_PSWD[WIFI_MAX_LEN_PSWD];
	struct mqtt
	{
		char host[MQTT_MAX_LEN_HOST];
		char usrnm[MQTT_MAX_LEN_USRNM];
		char pswd[MQTT_MAX_LEN_PASWD];
		char clientid[MQTT_MAX_LEN_CLIENT_ID];
		uint16_t port;
	} smqtt;
} comms_t;

void comms_init( comms_t *init_data);
uint8_t comms_chk_new_evt(FSM_events_t *evt);
uint8_t comms_hse_keeping(uint8_t curr_state, FSM_events_t *curr_state_evt_data, uint32_t sys_time, uint8_t immediate);
#endif // __COMMS_H__
