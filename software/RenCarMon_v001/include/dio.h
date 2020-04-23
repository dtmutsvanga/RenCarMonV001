#ifndef DIO_H
#define DIO_H
#include "pinout_usage.h"

typedef struct dio{
	uint8_t di[NUM_DI];
	uint8_t dq[NUM_DQ];
	uint8_t rq[NUM_RQ];
}dio_t;

 extern dio_t dio;
 extern void di_setup();
 void dq_setup();
 void rq_setup();
 void dio_setup();
 void di_read();
 void dq_set();

void extra();
#endif // DIO_H

