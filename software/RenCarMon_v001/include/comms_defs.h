#ifndef __COMMS_DEFS_H__
#define __COMMS_DEFS_H__

enum MSG_CHANNEL
{
	NO_MSG = 0,
	TCP_SERV,
	HTTP_SERV,
	MQTT_CLIENT,
	FW_UPDATE_CLIENT
};

enum MSG_TYPE {
	MSG_TYPE_CMD = 0,
	MSG_TYPE_INFO,
	MSG_TYPE_SYS		// Wifi settings, mqtt etc
};

enum MSG_INFO {
	MSG_INFO_SYS_DATA,
	MSG_INFO_LOG
};
#endif // __COMMS_DEFS_H__
