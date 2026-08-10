/*
 * pump.h
 *
 *  Created on: Nov 9, 2022
 *      Author: Nick
 */

#ifndef __PUMP__
#define __PUMP__

/* Required headers */
#include <mc9s12zvl32.h>
#include "types.h"		/* Common types and register access macros set */
#include "analog.h"
#include "timer.h"

/* External reference macros block */
#define MAX_DUTY ((uint8_t)255U)
#define MIN_DUTY ((uint8_t)0U)
/****/
#define TIM_WAIT_CURR (50UL)	/* Timer to wait for current stabilisation after startup */


/* Global variables for device operation */
extern uint16_t pump_active;
#define __pump_active ((const uint16_t)pump_active)

/* Functions prototypes */
uint8_t get_pump_dtcs(uint8_t);	/* Get pump DTCs flags aligned according transmission replies */
void pump_force(uint8_t);		/* Set air pump PWM period 
 	 	 	 	 	 	 	   	   Note! Actual behaviour is controlled by environmental definition 
 	 	 	 	 	 	 	   	   PWM_LEGAL - legacy hardware support*/


#endif /* __PUMP__ */
