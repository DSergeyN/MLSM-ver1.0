/*
 * pump.c
 *
 *  Created on: Nov 9, 2022
 *      Author: Nick
 */

#include "pump.h"

#define DTCs_FLAG_OVERCURR ((uint8_t)0x08U)
#define DTCs_FLAG_OPEN_CIR ((uint8_t)0x04U)

uint16_t pump_active;

/**
 * @Brief Get pump diagnostic status, reset state on argument
 */
uint8_t get_pump_dtcs(uint8_t erase){
	/****/
	static uint8_t flags=(uint8_t)0U;
	static uint8_t pump_hold=(uint8_t)0U;
	static uint32_t pump_wait=0UL;
	/****/
	if(erase)flags=(uint8_t)0U;
	else if(pump_hold^pump_active){
		pump_hold=pump_active;
		pump_wait=__get_millis;
	}
	else if(ms_from(pump_wait)>TIM_WAIT_CURR){
		if(pump_hold){
			switch(pump_current()){
				case PUMP_FAIL_SHORT: flags|=DTCs_FLAG_OVERCURR; break;
				case PUMP_FAIL_OPENL: flags|=DTCs_FLAG_OPEN_CIR; break;
				default: break;
			}
		}
		else if(pump_current()==PUMP_FAIL_SHORT)flags|=DTCs_FLAG_OPEN_CIR;
	}
	return flags;
}

/**
 * @Brief Set air pump PWM period
 */
void pump_force(uint8_t f){
#if(PWM_LEGAL)					/* Use directive since current air pump doesn't support normal PWM operation */
	PWMDTY2=f;
#else
	PWMDTY2=f?MAX_DUTY:MIN_DUTY;
#endif
	pump_active=f?1U:0U;
	return;
}
