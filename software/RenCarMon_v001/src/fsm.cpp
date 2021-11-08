#include "fsm.h"
#include "logging.h"
#include <cstdlib>

uint8_t state_table[ST_MAX_STATES][EV_MAX_EVENTS] = {
			      /* CMD_IDL  | 		CMD_ARM		| 	CMD_TIM_MODE|	CMD_ENG_RUN	|   CMD_ADMIN   |	SYS_IN_EOFF | SYS_TM_TO		*/
	/* IDLE */ {ST_IDLE		, ST_ARMED_WAIT	,	ST_TIM_MODE		, ST_ENG_RUN	, ST_ADMIN      , ST_IDLE		  , ST_IDLE			},
	/* ARMD */ {ST_IDLE		, ST_ARMED		  ,	ST_ARMED		  , ST_ARMED		, ST_ARMED      , ST_ARMED		, ST_ARMED			},
	/* ARMW */ {ST_IDLE		, ST_ARMED_WAIT	,	ST_ARMED_WAIT	, ST_ARMED_WAIT,  ST_ARMED_WAIT, ST_ARMED		, ST_ARMED_WAIT		},
	/* TIMM */ {ST_IDLE		, ST_ARMED_WAIT	,	ST_TIM_MODE		, ST_TIM_MODE	, ST_TIM_MODE   , ST_TIM_MODE	, ST_ARMED_WAIT		},
	/* ADM N*/ {ST_IDLE		, ST_ARMED_WAIT	,	ST_ADMIN		  , ST_ADMIN		, ST_ADMIN      , ST_ADMIN		, ST_ADMIN			},
	/* ERUN */ {ST_IDLE		, ST_ARMED_WAIT ,	ST_ENG_RUN		, ST_ENG_RUN	, ST_ENG_RUN    , ST_ENG_RUN	, ST_ENG_RUN		}
	};

FSM_states_t States[ST_MAX_STATES];

	// Initialize finite state machine functions
FSM_states_t *FSM_Init(eFSM_states last_state, FSM_events_t *evt) 
{
  log_msg(LOG_LVL_WARNING, SYS_APP, "FSM_Init. State =");
  log_msg_append_i(last_state);
	uint8_t i=0;
	States[i].id=ST_IDLE;
  States[i].run=st_idle;
  States[i].exit=st_idle_exit;
  States[i].init=st_idle_init;
  i++;

  States[i].id=ST_ARMED;
  States[i].run=st_armd;
  States[i].exit=st_armd_exit;
  States[i].init=st_armd_init;
  i++;

  States[i].id=ST_ARMED_WAIT;
  States[i].run=st_armd_wait;
  States[i].exit=st_armd_wait_exit;
  States[i].init=st_armd_wait_init;
  i++;

  States[i].id=ST_TIM_MODE;
  States[i].run=st_tim_mode;
  States[i].exit=st_tim_mode_exit;
  States[i].init=st_tim_mode_init;
  i++;

  States[i].id=ST_ADMIN;
  States[i].run=st_admin;
  States[i].exit=st_admin_exit;
  States[i].init=st_admin_init;
  i++;

  States[i].id=ST_ENG_RUN;
  States[i].run=st_eng_run;
  States[i].exit=st_eng_run_exit;
  States[i].init=st_eng_run_init;
  i++;

  // Generate idle state
  States[last_state].call_evt = *evt;
  States[last_state].init(States+last_state, 0);  // Dont save state during initialization
  return  States+last_state;
}

FSM_states_t *FSM_next_state(FSM_states_t *const curr_state, FSM_events_t *const new_evt) 
{
	FSM_states_t *nxt_state=nullptr;
	eFSM_states nxt_state_id=(eFSM_states)state_table[curr_state->id][new_evt->evt];
	if(nxt_state_id != curr_state->id){
    log_msg(LOG_LVL_WARNING, SYS_APP, "State Changing to : ");
    log_msg_append_i(nxt_state_id);
		curr_state->exit(curr_state);
		nxt_state= &States[nxt_state_id];
		nxt_state->call_evt=*new_evt;
		nxt_state->init(nxt_state, 1);
	}
	else {
		nxt_state=NULL;
	}
	return nxt_state;
}
