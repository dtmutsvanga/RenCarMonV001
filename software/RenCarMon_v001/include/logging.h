#ifndef __LOGGING_H__
#define __LOGGING_H__
#include <stdint.h>
#include "log_defs.h"
#include "defaults.h"
#include <stdarg.h>

#define SYMBOL_TO_STRING(x) #x
extern char log_buff[LOG_MAX_MSG_LEN];
void log_init(uint8_t lvl, uint32_t sys_mask);
void log_set_lvl_mask(uint8_t lvl, uint32_t sys_mask);
void log_msg(uint8_t lvl, uint32_t sys,const char *msg);
void log_msg_append(const char *msg);
void log_msg_append_i(int num);
#endif // __LOGGING_H__
