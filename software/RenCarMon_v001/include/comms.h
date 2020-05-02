#ifndef __COMMS_H__
#define __COMMS_H__
#include <stdint.h>
#include "defaults.h"
#include "cmd_queue.h"
#include "fsm_states.h"

typedef void (*MQTT_Rx_CB)(void);
typedef struct Comms
{
	cmd_t *curr_msg;
	
} Comms_t;

void comms_init();
uint8_t comms_read_evt(FSM_events_t *evt);
#endif // __COMMS_H__
