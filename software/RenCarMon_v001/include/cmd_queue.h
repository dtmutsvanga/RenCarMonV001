#ifndef __CMD_QUEUE_H__
#define __CMD_QUEUE_H__

#include <stdint.h>
#define CMD_STATE_CHG	1
#define CMD_INFO		2
#define CMD_ERR			3
#define CMD_DEBUG		4

 typedef struct scmd
{
	uint8_t cmd;
	uint8_t data_len;
	uint8_t *data;
	uint8_t done;		// command processed
	struct scmd *next;			// Next command

}cmd_t;

void cmd_q_init();
uint8_t cmd_q_add_to_queue(uint8_t cmd, uint8_t *data, uint8_t data_len);
cmd_t *cmd_q_get_next();

#endif // __CMD_QUEUE_H__
