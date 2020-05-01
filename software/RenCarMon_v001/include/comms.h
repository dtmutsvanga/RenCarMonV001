#ifndef __COMMS_H__
#define __COMMS_H__
#include <stdint.h>
#include "defaults.h"
#include "cmd_queue.h"

enum MSG_TYPE
{
	RACM_FEEDBACK = 0,
	RACM_ERR,
	RACM_DBG
};

enum MSG_CHANNEL
{
	NO_MSG = 0,
	TCP_SERV,
	HTTP_SERV,
	MQTT_CLIENT
};

typedef void (*MQTT_Rx_CB)(void);
typedef struct Comms
{
	struct cmd
	{
		struct state_arm
		{
			int8_t en;
			uint8_t rq_en; // Relay outputs to enable. Each bit represents a relay to be enabled
			uint8_t dq_en; // Digital outputs to enable.
			MSG_CHANNEL src;	// Source of command, wifi
		} state_arm;

		struct state_timer_mode
		{
			int8_t en;
			int time_min;
			MSG_CHANNEL src;
		} state_timer_mode;

		struct state_engine_run
		{
			int8_t en;
			int time_min;
			MSG_CHANNEL src;
		} state_engine_run;

		struct system_cmd
		{
			int8_t en;
			int cmd;
			MSG_CHANNEL src;
		} state_reset;
	} cmd;

	struct system_status{
		int8_t curr_state;
		uint32_t syetm_time;			// Current system time since boot
		int *state_time;				// Applies to engine run and time run
		uint8_t *dq;
		uint8_t *rq;
		uint8_t *di;
	}system_status;

	struct msg
	{
		MSG_TYPE type;
		char msg[MAX_COMM_MSGS_PER_CYCLE][MAX_COMM_MSG_LEN]; // Store up to 5 different messages
		MSG_CHANNEL channel[MAX_COMM_MSGS_PER_CYCLE];
	} msg;
} Comms_t;

#endif // __COMMS_H__
