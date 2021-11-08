/**
 * @file pinout_usage.h
 * @author D.T. Mutsvanga
 * @brief Pinout usage for the RenCarMon as per preset firmware
 * @version 0.1
 * @date 2020-04-22
 * 
 * @copyright Copyright Rentuku Autoation(c) 2020
 * 
 */

#include "pinout.h"
#include <stdint.h>

#ifndef PINOUT_USAGE
#define PINOUT_USEAGE

#define REL_STRT_PIN		RQ_0
#define REL_FUEL_PUMP_PIN	RQ_1
#define REL_IGN_PIN         RQ_2

#define DQ_COMM_SW_PIN		DQ_1
#define DQ_COMM_PWR_PIN     DQ_2

#define DI_COMM_MOD_STATE_PIN   DI_0
#define DI_PANIC_BTN_PIN	DI_1
#define DI_IGN_STATE_PIN	DI_2

// Inexes of respective pins in arrays
#define REL_STRT_IDX		0
#define REL_FUEL_PUMP_IDX	1
#define REL_IGN_PIN_IDX     2

#define DQ_COMM_SW_IDX	    0
#define DQ_COMM_PWR_IDX		1

#define DI_COMM_MOD_STATE_IDX	0
#define DI_PANIC_BTN_IDX	1
#define DI_IGN_STATE_IDX	2
#endif // PINOUT_USAGE
