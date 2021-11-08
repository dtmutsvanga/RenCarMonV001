#include "main.h"

dio_t dio;
FSM_states_t *curr_state, *nxt_state;
FSM_events_t new_evt;
eFSM_states saved_st_id;
racm_eep_data_t eeprom_data;
uint32_t time_to_sleep_ms = CONTROLLER_AUTO_OFF_DELAY_MS;
uint32_t time_ign_off_ms = 0;
void setup()
{
  // Setup dio
  dio_setup();
  di_read();
  // Before setting DI, do DS housekeeping
  wake_from_ds();
  dq_set();
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
  curr_state->run(curr_state);
  comms_chk_new_evt(&new_evt);
  if ((nxt_state = FSM_next_state(curr_state, &new_evt)))
  {
    curr_state = nxt_state;
    // Publish new state
    comms_hse_keeping(curr_state->id, &curr_state->call_evt, millis(), 1);
  }
  dq_set();

  comms_hse_keeping(curr_state->id, &curr_state->call_evt, millis(), 0);
  ds_hse_keeping();
}

/**************** FSM fucntion Definitions****************** */
// Run fxns
void st_idle_init(FSM_states_t *state, uint8_t save)
{
  state->call_evt.evt_data[0].u32=0;
  state->call_evt.evt_data[1].u8=0;
  state->call_evt.evt_data[2].u32=0;
   if(save)eepr_save_state(&eeprom_data, state->id, &state->call_evt);
}

void st_idle(FSM_states_t *state)
{
  // Do nothing
  /*
  uint8_t curr_en_state=dio.di[DI_PWR_IDX];
  uint8_t cur_pvt_st=   state->call_evt.evt_data[0].u8;
  uint8_t prev_en_state=state->call_evt.evt_data[1].u8;
  uint32_t en_off_time= state->call_evt.evt_data[2].u32;
  // Do check if engine off
  
  switch (cur_pvt_st)
  {
  case 0:
    if(!prev_en_state && curr_en_state){
    en_off_time=millis();
    cur_pvt_st=1;
  }
    break;
  case 1:
  // If time has passed, turn off controller
    if(millis() - en_off_time > (uint32_t)CONTROLLER_AUTO_OFF_DELAY_MS){
      dio.dq[DQ_PWR_IDX]=0;
    }
    else if(!curr_en_state){
         cur_pvt_st=0;
    }
  break;
  }
  state->call_evt.evt_data[0].u8=cur_pvt_st;
  state->call_evt.evt_data[1].u8=curr_en_state;
  state->call_evt.evt_data[2].u32=en_off_time;
  */
}
// Exit fxns
void st_idle_exit(FSM_states_t *state)
{
  // Do nothing
}

void st_armd_init(FSM_states_t *state, uint8_t save)
{
  // Turn on requested peripherals for dmin mode
  //dio.rq[REL_STRT_IDX] = 1;
  dio.rq[REL_FUEL_PUMP_IDX] = 1;
  if(save)eepr_save_state(&eeprom_data, state->id, &state->call_evt);
}
void st_armd(FSM_states_t *state)
{
  // Do nothing
}

void st_armd_exit(FSM_states_t *state)
{
  // Turn off all peripherals that were initalted by init
  //dio.rq[REL_STRT_IDX] = 0;
  dio.rq[REL_FUEL_PUMP_IDX] = 0;
}

void st_armd_wait_init(FSM_states_t *state, uint8_t save)
{
  // Do nothing. Clear flag engine off
  dio.rq[REL_STRT_IDX] = 0;
  dio.rq[REL_FUEL_PUMP_IDX] = 0;
  if(save)eepr_save_state(&eeprom_data, ST_ARMED, &state->call_evt);
}

void st_armd_wait(FSM_states_t *state)
{
  //Check engine off . If true, add engine off event to fsm queue
  if (1/*dio.di[DI_PWR_IDX]*/)
  {
    log_msg(LOG_LVL_WARNING, SYS_APP, "Power Off Event");
    delay(500);
    cmd_q_push_2_head(EV_SYS_IN_E_OFF, NULL, 0);
  }
}

void st_armd_wait_exit(FSM_states_t *state)
{
  // Do nothing.
}

void st_admin_exit(FSM_states_t *state)
{
  // Do nothing
}


void st_tim_mode_init(FSM_states_t *state, uint8_t save)
{
  // Initialize time variables. Maybe use a timer?
  if(save)eepr_save_state(&eeprom_data, ST_ARMED, &state->call_evt);
  state->call_evt.evt_data[1].u32 = (uint32_t)millis(); // Start time.
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
void st_tim_mode_exit(FSM_states_t *state)
{
  // Reset timer variables
}

void st_eng_run_init(FSM_states_t *state, uint8_t save)
{
  // Turn on the ignition key
  dio.rq[REL_IGN_PIN_IDX]=1;
  // Save the time IGN pin was started
  state->call_evt.evt_data[1].u32=millis();
  state->call_evt.evt_data[2].u8=0;   // Initial state 
  if(save)eepr_save_state(&eeprom_data, ST_IDLE, &state->call_evt);
}

void st_eng_run(FSM_states_t *state)
{
   // Check if car is off, and in park. If not, return to idel
  if(!dio.di[DI_IGN_STATE_IDX]){
    cmd_q_push_2_head(EV_CMD_IDLE, NULL, 0);
    return;
  }
  // Ignition is turned on first, for 3 seconds
  switch (state->call_evt.evt_data[2].u8)
  {
    // Ignition on, starter not yet cranked
  case 0:
    if(millis() - state->call_evt.evt_data[1].u32 > ENGINE_IGN_TM_B4_START){
    // Turn on the starter. after turning on IGN for some time
    dio.rq[REL_STRT_IDX]=1;
    state->call_evt.evt_data[1].u32=millis();
    state->call_evt.evt_data[2].u8=1; // state change
  }
    break;
  case 1:
   if(millis() - state->call_evt.evt_data[1].u32 > STARTER_PULSE_TIME){
    // Turn off the starter.
    dio.rq[REL_STRT_IDX]=0;
    state->call_evt.evt_data[1].u32=millis();
    state->call_evt.evt_data[2].u8=2; // state change
  }
  break;

  // Case wait for time to end
  case 2:
   if(millis() - state->call_evt.evt_data[1].u32 > state->call_evt.evt_data[0].u8 *60*1000){
    // Turn on the starter.
    dio.rq[REL_IGN_PIN_IDX]=0;
    cmd_q_push_2_head(EV_CMD_IDLE, NULL, 0);
  }
  break;
  default:
    break;
  }
  
}

void st_eng_run_exit(FSM_states_t *state)
{
  // Disable engine
  dio.rq[REL_IGN_PIN_IDX]=0;
  dio.rq[REL_STRT_IDX]=0;
}

void st_admin_init(FSM_states_t *state, uint8_t save)
{
  // Do nothing
  if(save)eepr_save_state(&eeprom_data, state->id, &state->call_evt);
}
void st_admin(FSM_states_t *state)
{
  // Check for admin commands to system.
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

void wake_from_ds(){
  uint32_t rtc_time=0;
  uint32_t rtc_flg=0;
  // If ignition is on, reset RTC ctr invalidate flag
  if(!dio.di[DI_IGN_STATE_IDX]){
    ESP.rtcUserMemoryWrite(DS_RTC_TME_CTR_ADDR, &rtc_time, 4);
    ESP.rtcUserMemoryWrite(DS_RTC_FLAG_ADDR, &rtc_flg, 4);
    return;
  }
  
  ESP.rtcUserMemoryRead(DS_RTC_TME_CTR_ADDR, &rtc_time, 4);
  ESP.rtcUserMemoryRead(DS_RTC_FLAG_ADDR, &rtc_flg, 4);

// If flag is invalid, set timeout and return 
  if(rtc_flg != DS_RTC_FLAG || ESP.getResetInfo() != "Deep-Sleep Wake"){
      time_to_sleep_ms = CONTROLLER_AUTO_OFF_DELAY_MS;
      return;
  }

  // If timeout is reached, return to update communications
  if(rtc_time > DS_COMM_UPDT_s) {
    time_to_sleep_ms = DS_COMM_UPDT_TIME;
    return;
  }

  // Increase counter, return to deep sleep
  rtc_time += ( DS_SHRT_TME_s);
  rtc_flg= DS_RTC_FLAG;
  ESP.rtcUserMemoryWrite(DS_RTC_TME_CTR_ADDR, &rtc_time, 4);
  ESP.rtcUserMemoryWrite(DS_RTC_FLAG_ADDR, &rtc_flg, 4);
  ESP.deepSleep(DS_SHRT_TME_s*1000000, RF_DISABLED);
  return;
} 

void ds_hse_keeping() {
  uint32_t rtc_time=0;
  uint32_t rtc_flg=0;
  // Set sleep timeout if ignition has been turned off
  /*if(dio.di_frnt[DI_IGN_STATE_IDX]) {
      time_ign_off_ms = millis();
  }*/
  // Time ign off is the last time ign was on
  if(dio.di[DI_IGN_STATE_IDX]) {
    if(millis() - time_ign_off_ms > time_to_sleep_ms) {
    rtc_time = 0;
    rtc_flg= DS_RTC_FLAG;
    ESP.rtcUserMemoryWrite(DS_RTC_TME_CTR_ADDR, &rtc_time, 4);
    ESP.rtcUserMemoryWrite(DS_RTC_FLAG_ADDR, &rtc_flg, 4);
    ESP.deepSleep(DS_SHRT_TME_s*1000000, RF_DISABLED);
    }
  }
  else {
    time_ign_off_ms=millis();
  }
}