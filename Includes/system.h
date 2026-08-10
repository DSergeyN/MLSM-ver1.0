/*
 * system.h
 *
 *  Created on: Nov 8, 2022
 *      Author: Nick
 */

#ifndef __SYSTEM__
#define __SYSTEM__

#include <mc9s12zvl32.h>
#include "types.h"		/* Common types and register access macros set */
#include "projDefines.h"

typedef enum{
	Seat_inflation_pressure_sensor_input      =0,
	Seat_lumbar_support_valve_status          =1,
	Seat_air_pump_status                      =2,
	Seat_massage_control_valve_status         =3,
	FIU_seat_massage_request_status	          =4, /* Not used from my side */
	Seat_side_support_valve_status            =5,
	CCU_seat_massage_request 		   		  =6,
	Seat_control_switch_status_lumbar_control =7,
	Seat_lumbar_support_position_actual       =8,
	Seat_lumbar_support_position_target       =9,
	Seat_side_support_request_status		  =10,
	DTCs_pump_and_pressure_sensor             =11,
	DCTs_lumbar_valves_errors                 =12,
	DCTs_side_support_valves_errors           =13,
	DCTs_massage_left_valve_errors            =14,
	DCTs_massage_right_valve_errors           =15
}Int_Diag_Req_t;

/* Functions prototypes */
void cpu_init(void);		 	/* Initialise CPU core to run 12.5 MHz with 6.25 MHz bus clock. */
void set_irq_priority(void);	/* Configure device interrupt priorities. */
void rti_init(void);			/* Brief Configure mS System Timer. */
void port_init(void);			/* Configure Pin routing and other Port features. */
void lin_init(void);			/* Initialise LIN Physical interface. */
void pwm_init(void);			/* Initialise PWM for air pump control. */
void gpio_init(void);   		/* Initialise GPIO pins peripheral. */
void spi_init(void);    		/* Initialise SPI interface for valve control. */
void timer_init(void);
void bats_init(void);			/* Battery sense initialisation for ADC assistance. */
void vreg_conf(void);			/* Configure VREG to operate in internal mode with 5V output. */
void adc_init(void);			/* ADC initialisation for analogue inputs sensing. */
void block_lin_PT2(void);       /* Blocks TIM0 output compare on PT2, 
										  should be called after LIN stack initialisation */

#endif /* __SYSTEM__ */
	
