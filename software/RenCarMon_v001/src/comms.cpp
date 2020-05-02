#include "comms.h"
#include "racm_mqtt.h"
#include "defaults.h"
#include "racm_wifi.h"
uint8_t comms_read(FSM_events_t *evt)
{
	cmd_t *cmd=cmd_q_get_next();
	if(!cmd) return 0;
	
	switch (cmd->cmd)
	{
	case EV_CMD_IDLE:
		evt->evt=EV_CMD_IDLE;
	break;
	case EV_CMD_ADMIN:
		evt->evt=EV_CMD_ADMIN;
	break;
	case EV_CMD_ARM:
		evt->evt=EV_CMD_ARM;
	break;
	case EV_CMD_ENG_RUN:
		evt->evt=EV_CMD_ENG_RUN;
		evt->evt_data[0].u8 = (uint8_t)cmd->data[0];
	break;
	case EV_CMD_TIM_MODE:
		evt->evt=EV_CMD_TIM_MODE;
		evt->evt_data[0].u16=*(uint16_t*)cmd->data;
	break;
	default:
		break;
	}

	cmd->done=1;
	return 1;
}

void comms_init()
{
	cmd_q_init();
	wifi_add_connect_CB(MqttWiFiConnectCB);
	wifi_add_disconnect_CB(MqttWiFiConnectCB);
	mqtt_init(MQTT_HOST, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USRNM, MQTT_PASSWD);
	wifi_init(WIFI_SSID,(uint8_t)sizeof(WIFI_SSID) ,WIFI_PASSWD, (uint8_t)WIFI_PASSWD);
}
