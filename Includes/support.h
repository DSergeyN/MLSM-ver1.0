/*
 * support.h
 *
 *  Created on: Feb 13, 2023
 *      Author: Nick
 */

#ifndef __SUPPORT__
#define __SUPPORT__

/*---- Required application headers ----*/
#include "timer.h"
#include "pump.h"
#include "analog.h"
#include "eeprom.h"
#include "gpio.h"
/****/
#include "valve.h" /* Root pneumatic control */
/****************************************/

typedef struct{
	uint8_t backrest;
	uint8_t cushion;
	uint8_t active_l;
	uint8_t active_r;
}sides_pr_t;

/* Global variables for device operation */
extern sides_pr_t sides_pr;
/*****************************************/

/**** Functions prototypes ****/
void read_side_s(void);
void save_side_s(uint8_t);						/* Save current SideSupport state to argument - memory position slot */
uint8_t apply_side_support(uint8_t, uint8_t);
uint8_t adjust_side_support(uint8_t, uint8_t);
uint8_t acceleration_side_support(uint8_t);
/******************************/

#endif /* __SUPPORT__ */
