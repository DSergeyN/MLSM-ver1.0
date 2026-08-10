/*
 * valve.h
 *
 *  Created on: Nov 10, 2022
 *      Author: Nick
 */

#ifndef __VALVE__
#define __VALVE__

#include <mc9s12zvl32.h>
#include <types.h>

#include "system.h"
#include "timer.h"
#include "pump.h"
#include "analog.h"
#include "eeprom.h"
#include "gpio.h"
/* Pneumatic subsystems */
#include "massage.h"
#include "lumbar.h"
#include "support.h"

/****/
#define LEGACY_OFF (1)	/* Support for legacy off by relay - read/store pneumatic parameters to EEPROM */
/****/

/**** SPI commands for L9733 IC ***/
#define SPI_CMD_VALVE (0xAC00U)
#define SPI_CMD_DIAGT (0xA300U)
#define SPI_CMD_ILIMT (0xAA00U)
#define SPI_CMD_GETDI (0x0000U)
/**********************************/

#define BASE_PNUMO_TIM (50UL)	   /* Basic timer for pneumatic intermediate operations */
#define ISOLATE_PUMP_W (50UL)
#define __ONLY_P_SENSE (100UL)
#define ISOLATE_P_SENS (ISOLATE_PUMP_W+100UL)	/*  */

/**** Pneumatic system limits *****/
#define MAX_BAG_PRESS ((uint8_t)60U)
#define FLT_BAG_PRESS ((uint8_t)10U)
#define PR_HYSTERESIS ((uint8_t)10U)
#define AEM_BAG_PRESS ((uint8_t)5U)
/**********************************/

#define SCHEDULE_P_TIM (600000UL)  /* Timer to check pneumatic system pressures state */
#define DEFERRED_P_TIM (45000UL)   /* Timer to check other pneumatic systems pressure after any modifications */
#define DECR_P_TIM		(5000UL)   /* Time to decrement DEFERRED_P_TIM in case of sequental impacts */

extern uint16_t const p_drop_corr[];
#define __get_drop_pressure(P) (((P)>MAX_BAG_PRESS)?(P):((uint8_t)p_drop_corr[(P)]))

/**** L9733 IC selector ***********/
enum{
	_R_SIDE,
	_L_SIDE,
	_V_LMB
};
/**********************************/

/** 
 * @Refers   Datatypes 
 * @Specific SPI processing structure.
 */
typedef struct{
	uint16_t r_side_st;
	uint16_t l_side_st;
	uint16_t lumb_v_st;
	uint16_t request;
	 uint8_t drd, busy;
}spi_work_t;

/* Drain for sensor calibration system */
typedef struct{
	uint16_t open;
	uint16_t exit;
	uint32_t notch;	
}drain_sys_t;

/**
 * @Refers   Datatypes 
 * @Specific Memmory setting and edit state.
 */
typedef struct{
	uint8_t edited_sds;
	uint8_t last_m_sds;
	uint8_t edited_lmb;
	uint8_t last_m_lmb;
}mem_status_t;

/* Global variables for device operation */
extern volatile spi_work_t spi_work;
#define __SPI_STATE ((volatile spi_work_t*)&spi_work)
/****/
extern uint16_t valves_open;
#define __valves_open ((const uint16_t)valves_open)
/****/
extern drain_sys_t drain;
#define DRAIN_EXIT_TIM (100UL)
/****/
extern mem_status_t mem_status;
/**********************************/

/**** Functions prototypes ********/
uint8_t spi_tx_word(int16_t, uint16_t);
uint8_t get_dtcs_status(void);						/* Get DCTs flags pending for alarm in LIN reply frame */
uint8_t get_valve_diagnostics(int16_t, uint8_t);	/* Return specially compiled byte for Report (valve diagnostics) based on SPI return data. */
uint8_t pneumatic_drain(int16_t);
uint8_t pressure_hoarder(uint8_t, int16_t);
uint8_t operation_mode(int16_t, uint8_t);		 	/* TODO: in progress */
/**********************************/

void test_spi_tx(uint16_t);

#endif /* __VALVE__ */
