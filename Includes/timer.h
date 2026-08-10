/*
 * timer.h
 *
 *  Created on: Nov 11, 2022
 *      Author: Nick
 */

#ifndef __TIMER__
#define __TIMER__

/* Required headers */
#include <mc9s12zvl32.h>
#include <types.h>

/* External interaction variables */
extern volatile uint32_t millisecond;
#define __get_millis (*(volatile uint32_t*)&millisecond) /* Get current millisecond value since last overflow      */

/* Functions prototypes */
uint32_t ms_from(uint32_t);  /* Returns millisecond elapsed since last marker, includes overflow processing.        */
void ms_halt(uint32_t);		 /* Halts code execution for specified time - ms, for use on initialisation stage only. */
void ms_halt_com(uint32_t);	 /* Halts code execution for specified time - ms, same as ms_halt() but uses computative resources instead of IRQ */

#endif /* __TIMER__ */
