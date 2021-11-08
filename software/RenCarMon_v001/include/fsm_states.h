#ifndef __FSM_STATES_H__
#define __FSM_STATES_H__
#include "racm_types.h"
#define FSM_MAX_EVT_DATA_NUM 3
enum eFSM_events{
	EV_CMD_IDLE = 0,
	EV_CMD_ARM,
	EV_CMD_TIM_MODE,
	EV_CMD_ENG_RUN,
	EV_CMD_ADMIN,
	EV_SYS_IN_E_OFF,		// SYS -- System generated event. Input Engine off
	EV_SYS_TIM_MODE_TO,	// Timer mode timeout
	EV_MAX_EVENTS
};

enum eFSM_states {
	ST_IDLE = 0,
	ST_ARMED,
	ST_ARMED_WAIT,		// 
	ST_TIM_MODE,
	ST_ADMIN,
	ST_ENG_RUN,
	ST_MAX_STATES
};

typedef struct {
	eFSM_events evt;
	varunion evt_data[FSM_MAX_EVT_DATA_NUM];
}FSM_events_t;

#endif // __FSM_STATES_H__
