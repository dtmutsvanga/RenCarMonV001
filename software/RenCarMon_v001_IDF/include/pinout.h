/**
 * @file pinout.h
 * @author D.T. Mutsvanga  (dtmutsvanga@yahoo.com)
 * @brief Pinouts for the RenCarMon V001 based on the ESP8266 NodeMCU.
 * @version 0.1
 * @date 2020-04-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef PINOUT_H
#define PINOUT_H

#define ESP_8266_IDF		// Uncomment to use in ESP8266 IDF. Default - Arduino framework
#ifndef ESP_8266_IDF
#include <Arduino.h>
// Digital Inputs
#define DI_0	D2			// Digital Input 0
#define DI_1	D3			// Digital Input 1
#define DI_PWR	10			// Digital input Power 

#define DI_INPUT_MODE	INPUT_PULLUP
#define DI_PWR_ON	0
#define DI_PWR_OFF 	1

// Digital Outputs
#define DQ_PWR 		D4		// Digital Output Power 
#define DQ_0		D5		// Digital Output 0
#define DQ_1		D6		// Digital Output 1
#define DQ_LED_BL	LED_BUILTIN	

#define DQ_OUTPUT_MODE	OUTPUT
#define DQ_ON	HIGH	
#define DQ_OFF	LOW

// Relay Outputs
#define RQ_0	D7			// Relay Output 0
#define RQ_1	D8			// Relay Ouput 1

#define RQ_OUTPUT_MODE	OUTPUT
#define RQ_ON	HIGH	
#define RQ_OFF	LOW

#else //ESP_8266_IDF
#include "gpio.h"
// Digital Inputs
#define DI_0	GPIO_Pin_4			// Digital Input 0
#define DI_1	GPIO_Pin_5			// Digital Input 1
#define DI_PWR	GPIO_Pin_10			// Digital input Power 

#define DQ_INPUT_MODE	GPIO_Mode_Input
#define DI_PWR_ON	0
#define DI_PWR_OFF 	1

// Digital Outputs
#define DQ_PWR 		GPIO_Pin_2		// Digital Output Power 
#define DQ_0		GPIO_Pin_14		// Digital Output 0
#define DQ_1		GPIO_Pin_12		// Digital Output 1
#define DQ_LED_BL	GPIO_Pin_16	

#define DQ_OUTPUT_MODE	GPIO_Mode_Output
#define DQ_ON	1	
#define DQ_OFF	0

// Relay Outputs
#define RQ_0	GPIO_Pin_13			// Relay Output 0
#define RQ_1	GPIO_Pin_15			// Relay Ouput 1

#define RQ_OUTPUT_MODE	GPIO_Mode_Output
#define RQ_ON	1	
#define RQ_OFF	0

#endif //ESP_8266_IDF

#endif // PINOUT_H
