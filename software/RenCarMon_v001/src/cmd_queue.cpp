#include "cmd_queue.h"
#include <cstdlib>
#include <cstring>
#include "defaults.h"
#include "logging.h"
static cmd_t *queue=NULL;
static uint8_t cmd_ctr = MAX_CMD_QUEUE;

cmd_t *create_new_cmd(uint8_t cmd, uint8_t *data, uint8_t data_len){
	if (!cmd_ctr)
		return NULL;
	cmd_t *nw_cmd;
	uint8_t *nw_data = NULL;

	nw_cmd = (cmd_t *)calloc(1, sizeof(cmd_t));
	if (!nw_cmd)
		return NULL;
	if (data_len>0)
	{
		nw_data = (uint8_t *)calloc(data_len, sizeof(uint8_t));
		if (!nw_data)
			return NULL;
		memcpy(nw_data, data, data_len);
	}
	nw_cmd->cmd = cmd;
	nw_cmd->data_len = data_len;
	nw_cmd->data = nw_data;
	return nw_cmd;
}
uint8_t cmd_q_push(uint8_t cmd, uint8_t *data, uint8_t data_len)
{
	cmd_t *nw_cmd=create_new_cmd(cmd, data, data_len);
	if(!nw_cmd){
		log_msg(LOG_LVL_CRITICAL, SYS_COMM, "Could not allocate memory for new command!");
		return 0;
	}
	if (queue)
	{
		cmd_t *curr = queue;
		while (curr)
		{
			curr = curr->next;
		}
		curr = nw_cmd;
	}
	else
	{
		queue = nw_cmd;
	} // Else if new command is first command receive by the system}

	cmd_ctr--;
	return 1;
}

uint8_t cmd_q_push_2_head(uint8_t cmd, uint8_t *data, uint8_t data_len)
{
	cmd_t *nw_cmd=create_new_cmd(cmd, data, data_len);
		if(!nw_cmd){
		log_msg(LOG_LVL_CRITICAL, SYS_COMM, "Could not allocate memory for new command!");
		return 0;
	}
	nw_cmd->next=queue;
	queue=nw_cmd;
	return 1;
}
cmd_t *del_cmd(cmd_t *cmd)
{

	if (!cmd)
		return NULL;
	cmd_t *nxt = cmd->next;
	free(cmd->data);
	free(cmd);
	return nxt;
}
cmd_t *  cmd_q_get_next()
{
	if (queue)
	{
		if (!queue->done)
			return queue;		// Command not processed, also for first element handling
		queue = del_cmd(queue); // Command processed, delete and return next cmd
	}
	else {
		return NULL;
	}

	if (cmd_ctr < MAX_CMD_QUEUE)
		cmd_ctr++;

	return queue;
}

void cmd_q_delete(cmd_t *qu)
{
	if (!qu)
		return;
	while (qu)
	{
		qu = del_cmd(qu);
	}
}

void cmd_q_init()
{
	cmd_q_delete(queue);
	queue = NULL;
}
