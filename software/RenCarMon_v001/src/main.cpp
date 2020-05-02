#include <Arduino.h>
#include "dio.h"
#include "comms.h"
#include "fsm.h"
dio_t dio;
FSM_states_t *curr_state;
FSM_events_t new_evt;
void setup()
{
  Serial.begin(9600);
  dio_setup();
  comms_init();
  curr_state=FSM_Init();
  
  //setup_mqtt();
}

void loop()
{
  // TODO: Get event, run house keeping
  di_read();
  comms_read_evt(&new_evt);
  curr_state=FSM_next_state(curr_state, &new_evt);
  curr_state->run(curr_state);
  dq_set();
}


/**************** FSM fucntion Definitions****************** */
// Run fxns
FSM_events_t st_idle(FSM_states_t *state) {
  // Do nothing
}
FSM_events_t st_armd(FSM_states_t *state) {
  // Do nothing
}
FSM_events_t st_armd_wait(FSM_states_t *state) {
  //Check engine off event. If true, add engine off event to fsm queue
  if(!dio.di[DI_PWR_IDX]){
    cmd_q_add_to_queue(EV_SYS_IN_E_OFF, NULL, 0);
  }
}
FSM_events_t st_tim_mode(FSM_states_t *state) {
  
  // Check time. Generate TO event if TO has been reached
  if(millis() - state->call_evt.evt_data[1].u32 > (uint32_t)(state->call_evt.evt_data[0].u16*60*1000)){
    // Generate a timeout event
    cmd_q_add_to_queue(EV_SYS_TIM_MODE_TO, NULL, 0);
  }
}
FSM_events_t st_eng_run(FSM_states_t *state) {
  // Do nothing
}
FSM_events_t st_admin(FSM_states_t *state) {
  // Check for admin commands to system.
}

// Exit fxns
void st_idle_exit(FSM_states_t *state) {
  // Do nothing
}
void st_armd_exit(FSM_states_t *state) {
  // Turn off all peripherals that were initalted by init
  dio.dq[REL_IGN_IDX]=0;
  dio.dq[REL_FUEL_PUMP_IDX]=0;
}
void st_armd_wait_exit(FSM_states_t *state) {
  // Do nothing.
}
void st_tim_mode_exit(FSM_states_t *state) {
  // Reset timer variables
}
void st_eng_run_exit(FSM_states_t *state) {
  // Disable engine
}
void st_admin_exit(FSM_states_t *state) {
  // Do nothing
}
// Init fxns
void st_idle_init(FSM_states_t *state) {
// Turn off all peripherls
}
void st_armd_init(FSM_states_t *state) {
// Turn on requested peripherals for dmin mode
 dio.dq[REL_IGN_IDX]=1;
  dio.dq[REL_FUEL_PUMP_IDX]=1;
}
void st_armd_wait_init(FSM_states_t *state) {
// Do nothing. Clear flag engine off
 dio.dq[REL_IGN_IDX]=0;
  dio.dq[REL_FUEL_PUMP_IDX]=0;
}
void st_tim_mode_init(FSM_states_t *state) {
// Initialize time variables. Maybe use a timer?
  state->call_evt.evt_data[1].u32=(uint32_t)millis();   // Start time. 
}
void st_eng_run_init(FSM_states_t *state) {
// Turn on engine
}
void st_admin_init(FSM_states_t *state) {
// Do nothing
}
