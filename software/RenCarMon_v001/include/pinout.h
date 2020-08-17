/**
 * @brief	Pinouts for the RenCarMon V001 based on the ESP8266 NodeMCU.
 * @author	D.T. Mutsvanga 
 * @date 	22 April 2020
 */
#include <Arduino.h>
#ifndef PINOUT_H
#define PINOUT_H

//#define ESP_8266_IDF		// Uncomment to use in ESP8266 IDF. Default - Arduino framework
#ifndef ESP_8266_IDF
// Digital Inputs
#define DI_0	D2			// Digital Input 0
#define DI_1	D3			// Digital Input 1
#define DI_2	3			// Digital input Power 
#define NUM_DI	3			// Number of digital inputs
#define DI_INPUT_MODE	INPUT_PULLUP
#define DI_PWR_ON	0
#define DI_PWR_OFF 	1

// Digital Outputs
#define DQ_0		D6		// Digital Output 0 relay IGN
#define DQ_1		D5		// Digital Output 1 power sw to comm module
#define DQ_2 		D4		// Digital Output charge to comm module
#define DQ_LED_BL	LED_BUILTIN	
#define NUM_DQ		2		// Number of digital outputs
#define DQ_OUTPUT_MODE	OUTPUT
#define DQ_ON	HIGH	
#define DQ_OFF	LOW

// Relay Outputs
#define RQ_0	D7			// Relay Output 0
#define RQ_1	D8			// Relay Ouput 1
#define RQ_2    D6          // 12 relay output
#define NUM_RQ	3

#define RQ_OUTPUT_MODE	OUTPUT
#define RQ_ON	HIGH	
#define RQ_OFF	LOW

#else //ESP_8266_IDF
// Digital Inputs
#define DI_0	4			// Digital Input 0
#define DI_1	0			// Digital Input 1
#define DI_2	10			// Digital input Power 
#define NUM_DI	3
#define DI_INPUT_MODE	INPUT_PULLUP
#define DI_PWR_ON	0
#define DI_PWR_OFF 	1

// Digital Outputs
#define DQ_0		12		// Digital Output 1
#define DQ_1		14		// Digital Output 0
#define DQ_2		2		// Digital Output Power 
#define DQ_LED_BL	LED_BUILTIN	
#define NUM_DQ	3
#define DQ_OUTPUT_MODE	OUTPUT
#define DQ_ON	1	
#define DQ_OFF	0

// Relay Outputs
#define RQ_0	15			// Relay Ouput 1
#define RQ_1	13			// Relay Output 0
#define NUM_RQ	2
#define RQ_OUTPUT_MODE	OUTPUT
#define RQ_ON	1	
#define RQ_OFF	0

#endif //ESP_8266_IDF

#endif // PINOUT_H
