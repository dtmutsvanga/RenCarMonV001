#include "logging.h"
#include "HardwareSerial.h"
#include "defaults.h"


typedef struct {
	uint8_t lvl;
	uint32_t sys_mask;
	uint8_t initd;
}log_t;

log_t log_mngr= {.lvl=LOG_LVL_NONE, .sys_mask=0xffffffff};

void log_init(uint8_t lvl, uint32_t sys_mask) 
{
	Serial.begin(LOG_BAUD_RATE);
	log_mngr.lvl=lvl;
	log_mngr.sys_mask=sys_mask;
	log_mngr.initd=1;
}

void log_set_lvl_mask(uint8_t lvl, uint32_t sys_mask) 
{
	log_mngr.sys_mask=sys_mask;
	log_mngr.lvl=lvl;
}

void log_msg(uint8_t lvl, uint32_t sys, const char *msg){
	if(!log_mngr.initd) return;
	if(lvl<log_mngr.lvl || !(sys&log_mngr.sys_mask)) return;

	Serial.println();
	switch(lvl){
		case LOG_LVL_INFO:
			Serial.print("INFO: ");
		break;
		case LOG_LVL_WARNING:
			Serial.print("WARNING: ");
		break;
		case LOG_LVL_CRITICAL:
			Serial.print("CRITICAL: ");
		break;
		default:
		Serial.print("UNKNOWN LEVEL");
		Serial.print(lvl);
		Serial.print(": ");
		break;
		Serial.print(msg);
	}
}

void log_msg_append(const char *msg) {
	Serial.print(msg);
}

void log_msg_append_i(int num){
	Serial.print(num);
}
