/*
 * eeprom.h
 *
 *  Created on: Nov 22, 2022
 *      Author: Nick
 */

#ifndef __EEPROM__
#define __EEPROM__

#include <mc9s12zvl32.h>
#include <types.h>

/* TODO: Temporary memory usage
 * 0 - Massage programm;
 * 1 - Massage intensity;
 * 2 - MemNumber if false in broadcast frame.
 */

#define MEM_LUMBAR_BASE ((uint8_t)0U)
#define MEM_LUMBAR_STEP ((uint8_t)4U)

#define MEM_SIDE_S_SERV ((uint8_t)16U) /* Four extra bytes ahead to support legacy POWER_OFF */
#define MEM_SIDE_S_BASE ((uint8_t)20U)
#define MEM_SIDE_S_STEP ((uint8_t)2U)

#define MEM_SETINGS_MAX ((uint8_t)3U)

/** 
 * @Refers   Datatypes 
 * @Specific ERRORS for EEPROM functions 
 */
typedef enum{
	ERR_address=1,
	ERR_erase,
	ERR_write
}EEPROM_err_t;

/* Functions prototypes */
EEPROM_err_t read_16w(uint8_t, uint16_t*);	/* Read 16-bit word */
EEPROM_err_t write_16w(uint8_t, uint16_t);	/* Write 16-bit word, includes sector erase and restore neighbour data if required */

#endif /* __EEPROM__ */
