/*
 * analog.h
 *
 *  Created on: Nov 14, 2022
 *      Author: Nick
 */

#ifndef __ANALOG__
#define __ANALOG__

/* Required headers */
#include <mc9s12zvl32.h>
#include <types.h>
#include "timer.h"
#include "pump.h"
#include "valve.h"
#include "gpio.h"


#define VREG_STABLE (50) /* Voltage regulator stabilisation time */

/** 
 * @Refers   Datatypes 
 * @Specific ERRORS for supply_voltage()
 */
typedef enum{
	BATS_NO_OV_FLAG=-3,
	BATS_IN_N_RANGE,
	BATS_NO_UV_FLAG
}AD_Fail_t;

/**
 * @Refers   Datatypes
 * @Specific ERRORS for pump_current()
 */
typedef enum{
	PUMP_FAIL_SHORT=-2,
	PUMP_FAIL_OPENL
}CS_Fail_t;

/* Functions prototypes */
void programm_adc(void); 		/* Program ADC and start continuous conversions loop          */
int16_t supply_voltage(void);	/* Get car supply voltage as VVV where VV.V Volts             */
uint8_t plug_p_offset(int16_t);	/* Get pressure offset after power ON or Wake up              */
void scan_p_offset(int16_t);	/* Periodic pass-through task to maintain air pressure offset */
int8_t air_pressure(void);		/* Get differential pneumatic system pressure in kPa	 	  */
int16_t pump_current(void);		/* Get Pump current in format III as I.II Amperes */

#endif /* __ANALOG__ */
