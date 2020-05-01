#ifndef __FSM_H__
#define __FSM_H__
#include <stdint.h>
#include "racm_types.h"

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

typedef void (*state_fxn)(void);
typedef void (*FSM_fxn)(void);
typedef struct {
	eFSM_events evt;
	varunion evt_data[3];
}FSM_events_t;

typedef struct {
	eFSM_states id;
	FSM_events_t call_evt;
	void (*init)(FSM_states_t *state);
	void (*run)(FSM_states_t *state);
	void (*exit)(FSM_states_t *state);
}FSM_states_t;

extern uint8_t state_table[ST_MAX_STATES][EV_MAX_EVENTS-1];
extern FSM_states_t States[ST_MAX_STATES];
extern void FSM_Init();
extern FSM_states_t *FSM_next_state(FSM_states_t *curr_state, FSM_events_t *new_evt);

extern void st_idle(FSM_states_t *state);
extern void st_armd(FSM_states_t *state);
extern void st_armd_wait(FSM_states_t *state);
extern void st_tim_mode(FSM_states_t *state);
extern void st_eng_run(FSM_states_t *state);
extern void st_admin(FSM_states_t *state);

extern void st_idle_exit(FSM_states_t *state);
extern void st_armd_exit(FSM_states_t *state);
extern void st_armd_wait_exit(FSM_states_t *state);
extern void st_tim_mode_exit(FSM_states_t *state);
extern void st_eng_run_exit(FSM_states_t *state);
extern void st_admin_exit(FSM_states_t *state);

extern void st_idle_init(FSM_states_t *state);
extern void st_armd_init(FSM_states_t *state);
extern void st_armd_wait_init(FSM_states_t *state);
extern void st_tim_mode_init(FSM_states_t *state);
extern void st_eng_run_init(FSM_states_t *state);
extern void st_admin_init(FSM_states_t *state);
#endif // __FSM_H__
