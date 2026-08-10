/*
 * lumbar.h
 *
 *  Created on: Jan 25, 2023
 *      Author: Nick
 */

/**************************************************************************//**
* @addtogroup Поясничная_поддержка
* @{
******************************************************************************/

#ifndef __LUMBAR__
#define __LUMBAR__

/*---- Required system headers ---------*/
#include <mc9s12zvl32.h>
#include <types.h>

/*---- Required application headers ----*/
#include "timer.h"
#include "pump.h"
#include "analog.h"
#include "eeprom.h"
#include "gpio.h"
/****/
#include "valve.h" /* Root pneumatic control */
/****************************************/

/**** Lumbar operation ****/
enum{
	_NA_,
	_UP_,
	_MD_,
	_LO_,
	_TS_
};

/**
 * @Refers   Datatypes 
 * @Specific Lumbar valves datatypes
 */	
typedef struct{
	uint8_t low_flat: 1;
	uint8_t low_fill: 1;
	uint8_t mid_flat: 1;
	uint8_t mid_fill: 1;
	uint8_t upp_flat: 1;
	uint8_t upp_fill: 1;
	uint8_t 		: 1;
	uint8_t			: 1;	
}LMB_All_Valves_t;

#define LMB_VALVES_MASK (0x0FFFU) /* Mask for SPI diagnostic reply with valves positions used in lumbar support */

typedef union{
	LMB_All_Valves_t val;
	uint8_t feed_cmd;
}lumbar_t;

/**
 * @Refers   Datatypes 
 * @Specific Lumbar pressure datatype
 */
typedef struct{
	uint8_t max_p;
	uint8_t upper;
	uint8_t middl;
	uint8_t lower;
}lmb_press_t;

/* Global variables for device operation */
extern lumbar_t lumbar;
extern lmb_press_t lmb_press;
extern lmb_press_t lmb_memory;
/*****************************************/

/**** Functions prototypes ********/
void lmb_correct_p(void);
void read_lumbar(void);  				  /* Get initial air pressure in lumbar support bags after Power ON or Wake Up */
void save_lumbar(uint8_t);				  /* Save current Lumbar state to argument - memory position slot */
uint8_t apply_lumbar(uint8_t, uint8_t);	  /* Apply lumbar memory setting: 1 argument memory_number, 2 argument impact event i.e. key press e.t.c */
uint8_t adjust_lumbar
	(const uint8_t,
	 const uint8_t,
	 const uint8_t,
	 const uint8_t);

#endif /* __LUMBAR__ */

/**
* @}
*/
