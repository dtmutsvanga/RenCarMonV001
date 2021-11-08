#include <Arduino.h>
#include "dio.h"

const uint8_t rq[NUM_RQ] = {REL_STRT_PIN,REL_FUEL_PUMP_PIN, REL_IGN_PIN};
const uint8_t dq[NUM_DQ] = {DQ_COMM_SW_PIN, DQ_COMM_PWR_PIN};
const uint8_t di[NUM_DI] = {DI_COMM_MOD_STATE_PIN, DI_PANIC_BTN_PIN, DI_IGN_STATE_PIN};

void di_setup()
{
	// Initialize digital inputs
	for (uint8_t i = 0; i < NUM_DI; i++)
	{
		pinMode(di[i], DI_INPUT_MODE);
	}
}

void dq_setup()
{
	// Initialize digital outputs
	for (uint8_t i = 0; i < NUM_DQ; i++)
	{
		pinMode(dq[i], DQ_OUTPUT_MODE);
	}
}

void rq_setup()
{
	// Initialize Relay outputs
	for (uint8_t i = 0; i < NUM_RQ; i++)
	{
		pinMode(rq[i], RQ_OUTPUT_MODE);
	}
}

void dio_setup()
{
	di_setup();
	dq_setup();
	rq_setup();
}

void di_read()
{
	// Read digital inputs
	uint8_t prev_val;
	for (uint8_t i = 0; i < NUM_DI; i++)
	{
		dio.di_frnt[i]=0;
		prev_val = dio.di[i];
		dio.di[i] = digitalRead(di[i]);
		// Values different
		if(!prev_val == 0 && dio.di[i] == 1) 
		{
			dio.di_frnt[i]=1;
		}
	}
}

void dq_set()
{
	// Set digital inputs
	for (uint8_t i = 0; i < NUM_DQ; i++)
	{
		digitalWrite(dq[i], dio.dq[i]);
	}
	// Set relay outputs
	for (uint8_t i = 0; i < NUM_RQ; i++)
	{
		digitalWrite(rq[i], dio.rq[i]);
	}
}

