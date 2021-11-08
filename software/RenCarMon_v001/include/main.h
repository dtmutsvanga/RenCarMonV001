#ifndef __MAIN_H__
#define __MAIN_H__
#include <Arduino.h>
#include "dio.h"
#include "comms.h"
#include "fsm.h"
#include "logging.h"
#include "Ticker.h"
#include "ESP_EEPROM.h"
// EEPROM  data struct
#define VER_MAJ	0x0
#define VER_MIN 0x0
#define VER_PATCH 0x01
#define VERSION	((VER_MAJ<<0xc)|(VER_MIN<<0x8)|(VER_PATCH))		// 0xMa Mi 
#define EEPR_VALID_FLAG 0xBABC
typedef struct  
{
	uint16_t flag;
	uint16_t ver;
	// State
	struct state_data
	{
		eFSM_states last_state;
		FSM_events_t state_evt;
	}sstate;

	// Communications
	comms_t comms;
} racm_eep_data_t;

extern racm_eep_data_t eeprom_data;
void eeprom_get_data(racm_eep_data_t *data);
void eepr_save_state(racm_eep_data_t *eepr_data, eFSM_states id, FSM_events_t *ev);
void wake_from_ds();
void ds_hse_keeping();
#endif // __MAIN_H__
