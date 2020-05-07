#include "comms.h"
#include <stdio.h>
#include "racm_mqtt.h"
#include "defaults.h"
#include "racm_wifi.h"
#include "logging.h"
#include "comms_defs.h"
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <Arduino.h>

ESP8266WebServer server(80);
ESP8266WebServer tcp_server(1992);
ESP8266HTTPUpdateServer httpUpdater;

uint8_t comms_chk_new_evt(FSM_events_t *evt)
{
	cmd_t *cmd = cmd_q_get_next();
	if (!cmd)
		return 0;

	switch (cmd->cmd)
	{
	case EV_CMD_IDLE:
		log_msg(LOG_LVL_WARNING, SYS_APP, "EV_CMD_IDLE received");

		evt->evt = EV_CMD_IDLE;
		break;
	case EV_CMD_ADMIN:
		log_msg(LOG_LVL_WARNING, SYS_APP, "EV_CMD_ADMIN received");
		evt->evt = EV_CMD_ADMIN;
		break;
	case EV_CMD_ARM:
		log_msg(LOG_LVL_WARNING, SYS_APP, "EV_CMD_ARM received");
		evt->evt = EV_CMD_ARM;
		break;
	case EV_CMD_ENG_RUN:
		log_msg(LOG_LVL_WARNING, SYS_APP, "EV_CMD_ENG_RUN received");
		evt->evt = EV_CMD_ENG_RUN;
		evt->evt_data[0].u8 = (uint8_t)cmd->data[0];
		break;
	case EV_CMD_TIM_MODE:
		log_msg(LOG_LVL_WARNING, SYS_APP, "EV_CMD_TIM_MODE received");
		evt->evt = EV_CMD_TIM_MODE;
		evt->evt_data[0].u16 = *(uint16_t *)cmd->data;
		break;
	case EV_SYS_IN_E_OFF:
		log_msg(LOG_LVL_WARNING, SYS_APP, "EV_SYS_IN_E_OFF received");
		evt->evt = EV_SYS_IN_E_OFF;
		break;
	case EV_SYS_TIM_MODE_TO:
		log_msg(LOG_LVL_WARNING, SYS_APP, "EV_SYS_TIM_MODE_TO received");
		evt->evt = EV_SYS_TIM_MODE_TO;
		break;
	default:
		break;
	}

	cmd->done = 1;
	return 1;
}

uint8_t comms_hse_keeping(uint8_t curr_state, FSM_events_t *curr_state_evt_data, uint32_t sys_time, uint8_t immediate)
{
	// Handle firmware updates
	// Handle HTTP clients

	static uint32_t prev_update = 0;
	server.handleClient();
	// Update Mqtt periodically
	if (millis() - prev_update > 10000 || immediate)
	{
		char buff[140];
		buff[0] = MSG_INFO_SYS_DATA;
		int len = snprintf(buff + 1, 139, "{ \"St\": %d, \
							\"Evt\": %d,\
							\"ED1\": %d, \
							\"ED2\": %d, \
							\"ED3\": %d, \
							\"STme\": %lu}",
						   curr_state, curr_state_evt_data->evt,
						   curr_state_evt_data->evt_data[0].u32,
						   curr_state_evt_data->evt_data[1].u32,
						   curr_state_evt_data->evt_data[2].u32,
						   (long unsigned int)sys_time);

		//log_msg(LOG_LVL_INFO, SYS_COMM, "Msg Info Json:");
		//log_msg_append(buff + 1);
		//log_msg_append("\n Total Length = ");
		//log_msg_append_i(len);
		if (!mqtt_write((uint8_t *)buff, len))
			return 0;
		prev_update = millis();
	}
	return 1;
}

void comms_init( comms_t *init_data)
{
	cmd_q_init();
	log_msg(LOG_LVL_INFO, SYS_COMM_MQTT, "MQTT CONN PARAMS\n");
	log_msg_append("Host: ");
	log_msg_append(init_data->smqtt.host);
	log_msg_append(" CID: ");
	log_msg_append(init_data->smqtt.clientid);
	log_msg_append(" Usrnm: ");
	log_msg_append(init_data->smqtt.usrnm);
	log_msg_append(" Passwd: ");
	log_msg_append(init_data->smqtt.pswd);

	mqtt_init(init_data->smqtt.host, init_data->smqtt.port, init_data->smqtt.clientid, init_data->smqtt.usrnm, init_data->smqtt.pswd);
	wifi_add_connect_CB(MqttWiFiConnectCB);
	wifi_add_disconnect_CB(MqttWiFiConnectCB);
	wifi_init(init_data->WiFi_SSID, (uint8_t)sizeof(init_data->WiFi_SSID), init_data->WiFi_PSWD, (uint8_t)sizeof(init_data->WiFi_PSWD));
	
	// MQTT

	// HTTP Web updater
	httpUpdater.setup(&server, OTAPATH, OTAUSER, OTAPASSWORD);
	server.begin();
}
