#include "main.h"

dio_t dio;
FSM_states_t *curr_state, *nxt_state;
FSM_events_t new_evt;
eFSM_states saved_st_id;
racm_eep_data_t eeprom_data;
void setup()
{
  dio_setup();
  dio.dq[DQ_PWR_IDX] = 1;
  dq_set();
  di_read();
  log_init(LOG_LVL_WARNING, SYS_LOG_ALL);

  eeprom_get_data(&eeprom_data);
  new_evt.evt = eeprom_data.sstate.state_evt.evt;
  curr_state = FSM_Init(eeprom_data.sstate.last_state, &eeprom_data.sstate.state_evt);
  comms_init(&eeprom_data.comms);
}

void loop()
{
  // TODO: Get event, run house keeping
  di_read();
  comms_chk_new_evt(&new_evt);
  if ((nxt_state = FSM_next_state(curr_state, &new_evt)))
  {
    curr_state = nxt_state;
    // Publish new state
    comms_hse_keeping(curr_state->id, &curr_state->call_evt, millis(), 1);
  }
  curr_state->run(curr_state);
  dq_set();

  comms_hse_keeping(curr_state->id, &curr_state->call_evt, millis(), 0);
}

/**************** FSM fucntion Definitions****************** */
// Run fxns
void st_idle(FSM_states_t *state)
{
  // Do nothing
}
void st_armd(FSM_states_t *state)
{
  // Do nothing
}
void st_armd_wait(FSM_states_t *state)
{
  //Check engine off event. If true, add engine off event to fsm queue
  if (dio.di[DI_PWR_IDX])
  {
    log_msg(LOG_LVL_WARNING, SYS_APP, "Power Off Event");
    delay(500);
    cmd_q_push_2_head(EV_SYS_IN_E_OFF, NULL, 0);
  }
}
void st_tim_mode(FSM_states_t *state)
{

  // Check time. Generate TO event if TO has been reached
  if (millis() - state->call_evt.evt_data[1].u32 > (uint32_t)(state->call_evt.evt_data[0].u16 * 60 * 1000))
  {
    // Generate a timeout event. Push to head as it is a priority event
    cmd_q_push_2_head(EV_SYS_TIM_MODE_TO, NULL, 0);
  }
}
void st_eng_run(FSM_states_t *state)
{
  // Do nothing
}
void st_admin(FSM_states_t *state)
{
  // Check for admin commands to system.
}

// Exit fxns
void st_idle_exit(FSM_states_t *state)
{
  // Do nothing
}
void st_armd_exit(FSM_states_t *state)
{
  // Turn off all peripherals that were initalted by init
  dio.rq[REL_IGN_IDX] = 0;
  dio.rq[REL_FUEL_PUMP_IDX] = 0;
}
void st_armd_wait_exit(FSM_states_t *state)
{
  // Do nothing.
}
void st_tim_mode_exit(FSM_states_t *state)
{
  // Reset timer variables
}
void st_eng_run_exit(FSM_states_t *state)
{
  // Disable engine
}
void st_admin_exit(FSM_states_t *state)
{
  // Do nothing
}
// Init fxns
void st_idle_init(FSM_states_t *state, uint8_t save)
{
   if(save)eepr_save_state(&eeprom_data, state->id, &state->call_evt);
}
void st_armd_init(FSM_states_t *state, uint8_t save)
{
  // Turn on requested peripherals for dmin mode
  dio.rq[REL_IGN_IDX] = 1;
  dio.rq[REL_FUEL_PUMP_IDX] = 1;
  if(save)eepr_save_state(&eeprom_data, state->id, &state->call_evt);
}
void st_armd_wait_init(FSM_states_t *state, uint8_t save)
{
  // Do nothing. Clear flag engine off
  dio.rq[REL_IGN_IDX] = 0;
  dio.rq[REL_FUEL_PUMP_IDX] = 0;
  if(save)eepr_save_state(&eeprom_data, ST_ARMED, &state->call_evt);
}
void st_tim_mode_init(FSM_states_t *state, uint8_t save)
{
  // Initialize time variables. Maybe use a timer?
  if(save)eepr_save_state(&eeprom_data, ST_ARMED, &state->call_evt);
  state->call_evt.evt_data[1].u32 = (uint32_t)millis(); // Start time.
}
void st_eng_run_init(FSM_states_t *state, uint8_t save)
{
  // Turn on engine
  if(save)eepr_save_state(&eeprom_data, state->id, &state->call_evt);
}
void st_admin_init(FSM_states_t *state, uint8_t save)
{
  // Do nothing
  if(save)eepr_save_state(&eeprom_data, state->id, &state->call_evt);
}

void eeprom_get_data(racm_eep_data_t *data)
{
  EEPROM.begin(sizeof(eeprom_data));
  log_msg(LOG_LVL_WARNING, SYS_APP, "EEPROM %");
  log_msg_append_i(EEPROM.percentUsed());
  if (EEPROM.percentUsed() > 0)
    EEPROM.get((int)0, *data);
  if ((data->flag != (uint16_t)EEPR_VALID_FLAG))
  {
    log_msg(LOG_LVL_CRITICAL, SYS_APP, "No data in EEPROM");
    // Major and minor versions are not matching or data invalid, but data was persent. Therefore clear the eprom
    // Get defaults
    data->flag = EEPR_VALID_FLAG;
    data->ver = VERSION;
    data->sstate.last_state = ST_IDLE;
    data->sstate.state_evt.evt = EV_CMD_IDLE;
    //WIFI
    snprintf(data->comms.WiFi_SSID, WIFI_MAX_LEN_SSID, WIFI_SSID);
    snprintf(data->comms.WiFi_PSWD, WIFI_MAX_LEN_PSWD, WIFI_PASSWD);

    log_msg(LOG_LVL_INFO, SYS_APP, data->comms.WiFi_SSID);
    //MQTT
    snprintf(data->comms.smqtt.host, MQTT_MAX_LEN_HOST, MQTT_HOST);
    snprintf(data->comms.smqtt.pswd, MQTT_MAX_LEN_PASWD, MQTT_PASSWD);
    snprintf(data->comms.smqtt.clientid, MQTT_MAX_LEN_CLIENT_ID, MQTT_CLIENT_ID);
    snprintf(data->comms.smqtt.usrnm, MQTT_MAX_LEN_USRNM, MQTT_USRNM);
    data->comms.smqtt.port = MQTT_PORT;
  }
  else
  {
    log_msg(LOG_LVL_CRITICAL, SYS_APP, "Data retrieved from EEPROM");
  }
  EEPROM.end();
}

void eepr_save_state(racm_eep_data_t *eepr_data, eFSM_states id, FSM_events_t *ev){
  // Turn off all peripherls// Save new state to eeprom
    EEPROM.begin(sizeof(racm_eep_data_t));
    eepr_data->sstate.last_state = id;
    eepr_data->sstate.state_evt = *ev;
    EEPROM.put(0, *eepr_data);
    if (EEPROM.commit())
    {
      log_msg(LOG_LVL_CRITICAL, SYS_APP, "New state saved! State ID = ");
      log_msg_append_i(eepr_data->sstate.last_state);
    }
    else
      log_msg(LOG_LVL_CRITICAL, SYS_APP, "Could not save data to EEPROM!");
    EEPROM.end();
}
