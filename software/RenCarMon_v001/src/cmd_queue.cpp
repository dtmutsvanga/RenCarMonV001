#include "cmd_queue.h"
#include <cstdlib>
#include <cstring>
#include "defaults.h"


static cmd_t *queue;
static uint8_t cmd_ctr=MAX_CMD_QUEUE;

uint8_t cmd_q_add_to_queue(uint8_t cmd, uint8_t *data, uint8_t data_len) 
{
	if(!cmd_ctr) return 0;
	cmd_t *nw_cmd;
	uint8_t *nw_data=NULL;
	
	nw_cmd=(cmd_t *)calloc(1, sizeof(cmd_t));
	if(!nw_cmd) return 0;
	if(data_len){
		nw_data=(uint8_t*)calloc(data_len, sizeof(uint8_t));
		if(!nw_data) return 0;
		memcpy(nw_data, data, data_len);
	}
	nw_cmd->cmd=cmd;
	nw_cmd->data_len=data_len;
	nw_cmd->data=nw_data;
	

	if(queue){
		queue->next=nw_cmd;
		}
	else {
		queue=nw_cmd;
	}				// Else if new command is first command receive by the system}
	
	cmd_ctr--;	
	return 1;					
}

cmd_t  *del_cmd(cmd_t *cmd){

	if(!cmd) return NULL;
	cmd_t *nxt=cmd->next;
		free(cmd->data);
		free(cmd);
		return nxt;
}
cmd_t *cmd_q_get_next() 
{
	if(queue){
		if(!queue->done) return queue;	// Command not processed, also for first element handling
		queue=del_cmd(queue);			// Command processed, delete and return next cmd
	}

	if(cmd_ctr < MAX_CMD_QUEUE) cmd_ctr++;

	return queue;
}

void cmd_q_delete(cmd_t *qu){
	if(!qu) return;
	while(qu){
		qu=del_cmd(qu);
	}
}

void cmd_q_init() 
{
	cmd_q_delete(queue);
	queue=NULL;
}
