#ifndef __FSM_H__
#define __FSM_H__
#include <stdint.h>
#include "fsm_states.h"

typedef void (*state_fxn)(void);
typedef void (*FSM_fxn)(void);

typedef struct sFSM_states{
	eFSM_states id;
	FSM_events_t call_evt;
	void (*init)(sFSM_states *state);
	FSM_events_t (*run)(sFSM_states *state);
	void (*exit)(sFSM_states *state);
} FSM_states_t;


extern uint8_t state_table[ST_MAX_STATES][EV_MAX_EVENTS-1];
extern FSM_states_t States[ST_MAX_STATES];
extern FSM_states_t *FSM_Init();
extern FSM_states_t *FSM_next_state(FSM_states_t *const curr_state, FSM_events_t *const new_evt);
// Run fxns
extern FSM_events_t st_idle(FSM_states_t *state);
extern FSM_events_t st_armd(FSM_states_t *state);
extern FSM_events_t st_armd_wait(FSM_states_t *state);
extern FSM_events_t st_tim_mode(FSM_states_t *state);
extern FSM_events_t st_eng_run(FSM_states_t *state);
extern FSM_events_t st_admin(FSM_states_t *state);
// Exit fxns
extern void st_idle_exit(FSM_states_t *state);
extern void st_armd_exit(FSM_states_t *state);
extern void st_armd_wait_exit(FSM_states_t *state);
extern void st_tim_mode_exit(FSM_states_t *state);
extern void st_eng_run_exit(FSM_states_t *state);
extern void st_admin_exit(FSM_states_t *state);
// Init fxns
extern void st_idle_init(FSM_states_t *state);
extern void st_armd_init(FSM_states_t *state);
extern void st_armd_wait_init(FSM_states_t *state);
extern void st_tim_mode_init(FSM_states_t *state);
extern void st_eng_run_init(FSM_states_t *state);
extern void st_admin_init(FSM_states_t *state);
#endif // __FSM_H__
