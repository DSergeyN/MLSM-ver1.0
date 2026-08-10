/*
 * timer.c
 *
 *  Created on: Nov 11, 2022
 *      Author: Nick
 */

#define MAXU32_T (~(uint32_t)0)
#define COMP_HALT_MUL (350UL)

#include "timer.h"

volatile uint32_t millisecond;

/**
 * @Scope Millisecond System tick timer
 */
interrupt VectorNumber_Vrti system_tick(void){
	millisecond++;
	CPMUIFLG&=(uint8_t)0x80U;
	return;
}

/**
 * @Brief Returns millisecond elapsed since last marker, 
 * 		  includes overflow processing.
 */
uint32_t ms_from(uint32_t ms){
	uint32_t freeze=__get_millis;
	if(freeze<ms)return MAXU32_T-ms+freeze;
	return freeze-ms;
}

/**
 * @Brief Halts code execution for specified time - ms,
 * 		  for use on initialisation stage only.
 */
void ms_halt(uint32_t ms){
	uint32_t notch=__get_millis;
	for(;ms_from(notch)<=ms;){};
	return;
}

/**
 * @Brief Halts code execution for specific time - ms,
 *        computative version of ms_halt.
 */
void ms_halt_com(uint32_t ms){
	ms*=COMP_HALT_MUL;
	for(;--(*(volatile uint32_t*)&ms);){};
	return;
}

