/*
 * alalog.c
 *
 *  Created on: Nov 14, 2022
 *      Author: Nick
 */

/* ADC commands for CMD_SEL [7:6] of ADCxCMD_0 register  */
#define ADC_CMD_JNC (0x00000000UL) /* Just a normal conversion command 	 */
#define ADC_CMD_AEL (0x80000000UL) /* End of list with automatic rewind  */

/* Group Interrupts to seed in ADCxCONIF register        */
#define ADC_INT_FL1 (0x01000000UL) /* First group interrupt				 */
#define ADC_INT_FL2 (0x00000000UL) /* Second group interrupt will be EOL */
#define ADC_INT_NOI (0x00000000UL) /* No interrupt on this conversion	 */

/* Reference voltage for ADC [7:6] of ADCxCMD_1 register */
#define ADC_REF_HL0 (0x00C00000UL) /* ADC select VRH_1/VRL_1 VREF 		 */

/* Required ADC channels for ADCxCMD_1 register          */
#define ADC_CHI1_BAND_VREF (0x00090000UL) /* Internal Ch 1 - Bandgap Voltage reference 		*/
#define ADC_CHE2_VSUP_MONI (0x00120000UL) /* External Ch 2 - Voltage Supply monitor  		*/
#define ADC_CHE7_CURR_SENS (0x00170000UL) /* External Ch 7 - Current Sensor for Pump driver */
#define ADC_CHE8_PRES_SENS (0x00180000UL) /* External Ch 8 - Pressure Sensor Vout monitor   */

/* ADC sample time SMPS [7:6] for ADCxCMD_2 register	 */
#define ADC_SMP_MAX (0x0000A000UL) /* Max Sample time for conversion	 */

#define RING_BUFFER (0x08U)	/* Ring buffer for 8 ADC samples */

/*---- Fix point calculations -------*/
#define FIX_MATS_R (1000000UL)	                  /* Resolution for fix-point math operations */
#define BANGAP_REF (12UL)   	                  /* Bandgap voltage reference - 1.2V for fix-point math representation */
#define TO_V16b_2D(V) ((uint16_t)((V)/=100000UL)) /* Supplementary for fix math to match VVV format where V.VV volts */
#define CAR_SUPP_V(V) ((uint8_t)(((V)*57U)/100U)) /* Supply divider voltage to real car voltage in VVV where VV.V volts,
 	 	 	 	 	 	 	 	 	 	 	 	 	 this is based on hardware divider 47k/10k as input */
/*---- Schottky diode correction ----*/
#define S1J_FV_DROP ((uint8_t)8)				  /* S1J Forward Voltage drop - 0.8V at 0.1A current */

/*---- BATS voltage thresholds ------*/
#define LBI4_MIN_ASSERT   ((uint8_t)80U)		  /* Low Voltage Warning  #4 assert   - 8V     */
#define LBI4_MAX_DEASSERT ((uint8_t)105U)	  	  /* Low Voltage Warning  #4 deassert - 10.5V  */
#define HBI1_MAX_ASSERT   ((uint8_t)180U)	  	  /* High Voltage Warning #1 assert   - 18V    */
#define HBI1_MIN_DEASSERT ((uint8_t)140U)	  	  /* High Voltage Warning #1 deassert - 14V    */

/*---- Pressure sensor system -------*/
#define MAX_DIFF_PRESS  (100U)					  /* Maximum differential pressure 100kPa - sensor resolution */
#define TO_P_kPa_16B(V) (((V)*10U)/45U)			  /* Voltage to MPXV5100DP pressure in kPa according to differential span of 4.5V */
#define PNUMO_BASIC_TIM (50UL)					  /* Basic timer for pneumatic intermediate operations */
#define PNUMO_VALVE_TIM (150UL)					  /* Pneumatic system valve toggle time */
#define PNUMO_EXTRA_TIM	(500UL)					  /* Pneumatic system short drain timer for initial measurements */
#define PNUMO_REF_SAMPL (10U)			  		  /* Number of pressure samples to collect and average for offset plug on Power ON or Wake Up */
#define PNUMO_WAIT_STAB (5000UL)				  /* Stabilisation delay - ms to measure pressure offset after all
 	 	 	 	 	 	 	 	 	 	 	 	 	 operations: valves toggling or pump work */
#define PNUMO_DRAIN_TIM (2000UL)				  /* Time to drain the whole pneumatic system */
#define PNUMO_CLOSE_DRN (1000UL)				  /* Interval to start offset measurements after drain valve is closed */
#define PNUMO_TAKE_POFF (250UL)					  /* Interval to take pressure offset measurements */
#define REDRAIN_PNUMO_S (300000UL)				  /* Interval to redrain pneumatic system */                
#define OFFSET_DISTURBE (3U)					  /* ADC critical shift while constant offset monitoring */
/*---- Pump current sensing ---------*/
#define PUMP_FAULT_HI   (1020U)					  /* FAULT voltage is 10.8V on 1200 Ohm CS resistor, will be forced
													 to 7.5V by BZX84C7V5LT1G, anyway this is 10-bit ADC max with small safe offset */
#define PUMP_FAULT_LO   (5U)					  /* Zero current sensing, positive safe offset applied */
#define VCS_TO_I16b_2D(V) (((V)*455U)/1200U)      /* Voltage on CS resistor to current in format III where I.II Amperes */
#define PUMP_TON_FAULT  (50U)					  /* Delay to start CS output current test for Open Load after pump Turn ON */

#include "analog.h"

/* ADC CSL assembly */
uint32_t command[] __attribute__((aligned (4)))={ //
	/* First group conversion  */
	ADC_CMD_JNC|ADC_INT_NOI|ADC_REF_HL0|ADC_CHI1_BAND_VREF|ADC_SMP_MAX,
	ADC_CMD_JNC|ADC_INT_NOI|ADC_REF_HL0|ADC_CHE2_VSUP_MONI|ADC_SMP_MAX,
	ADC_CMD_JNC|ADC_INT_NOI|ADC_REF_HL0|ADC_CHE7_CURR_SENS|ADC_SMP_MAX,
	ADC_CMD_JNC|ADC_INT_FL1|ADC_REF_HL0|ADC_CHE8_PRES_SENS|ADC_SMP_MAX,
	/* Second group conversion */
	ADC_CMD_JNC|ADC_INT_NOI|ADC_REF_HL0|ADC_CHI1_BAND_VREF|ADC_SMP_MAX,
	ADC_CMD_JNC|ADC_INT_NOI|ADC_REF_HL0|ADC_CHE2_VSUP_MONI|ADC_SMP_MAX,
	ADC_CMD_JNC|ADC_INT_NOI|ADC_REF_HL0|ADC_CHE7_CURR_SENS|ADC_SMP_MAX,
	ADC_CMD_AEL|ADC_INT_FL2|ADC_REF_HL0|ADC_CHE8_PRES_SENS|ADC_SMP_MAX,
	/* Automatic wrap to top */
};

/* Conversion results */
#define GROUP_1_ADDRESS (0)
#define GROUP_2_ADDRESS (4)
uint16_t result[RING_BUFFER] __attribute__((aligned (4))); //
uint16_t* data_carriage;

static volatile struct{
	uint8_t j_heat: 2;
	uint8_t over_v: 2;
	uint8_t undr_v: 2;
	uint8_t _dummy: 2;
}health;

static volatile struct{
	uint16_t bandgap_vref;
	uint16_t supply_voltage;
	uint16_t pump_current;
	uint16_t air_pressure;
}analog;

static uint8_t identify_err; /* Error catching variable for debugger */
static uint16_t adc_impact;
#define __adc_impact (*(volatile uint16_t*)&adc_impact)

#define __pneumo_static (!(__valves_open||__pump_active))
uint16_t pressure_offset;

/**
 * @Brief 
 */
interrupt VectorNumber_Vhti core_heat(void){
	if(CPMUHTCTL&((uint8_t)0x04U))health.j_heat=1U;
	else health.j_heat=0U;
	CPMUHTCTL&=(uint8_t)0x2BU;
	return;
}

/**
 * @Brief Battery sensor events
 */
interrupt VectorNumber_Vbats bat_sensor(void){
	switch(BATSR){
		case (uint8_t)1U: health.undr_v=1U; break;
		case (uint8_t)2U: health.over_v=1U; break;
		default:
			health.undr_v=0U;
			health.over_v=0U;
		break;
	}
	BATIF=BATIF;
	return;
}

/**
 * @Brief Identify ADC errors by flag
 */
interrupt VectorNumber_Vadc0err adc_error(void){
	identify_err=ADC0EIF;
	identify_err=identify_err; /* Dummy line for debugger breakpoint */
	return;
}

/**
 * @Brief Identify sequence problems. 
 */
interrupt VectorNumber_Vadc0conv_seq_abrt abort(void){ 
	identify_err=ADC0IF;
	identify_err=identify_err; /* Dummy line for debugger breakpoint */
	return;
}

/**
 * @Brief ADC Conversion complete interrupt.
 */
interrupt VectorNumber_Vadc0conv_compl conversion(void){
	/* Set read pointer */	
	if(ADC0CONIF%2){
		data_carriage=&result[GROUP_2_ADDRESS];
	}
	else{
		data_carriage=&result[GROUP_1_ADDRESS];
	}
	/* Get group data */
#if(1)
	analog.bandgap_vref=*data_carriage++;
	analog.supply_voltage=*data_carriage++;
	analog.pump_current=*data_carriage++;
	analog.air_pressure=*data_carriage;
#endif
	/* Clear ADC interrupt */
	ADC0CONIF=ADC0CONIF;
	adc_impact=1U;
	return;
}

/**
 * @Brief Program ADC and start continuous conversions loop.
 */
void programm_adc(void){
	ADC0CBP=(void*)command;		/* Plug command array pointer */
	ADC0RBP=(void*)result;		/* Plug result array pointer  */
	ADC0CTL|=0x8800U;			/* Enable ADC peripheral, Flow control access through data bus only */
	ADC0FLWCTL|=(uint8_t)0x20U;	/* Assert Restart event */
	for(;ADC0FLWCTL!=0x00U;){}; /* Wait Restart event to complete */
	return;
}

/**
 * @Brief Stabilisation and holding filter for Supply Voltage
 */
static uint16_t slide_filter_supp(int16_t val){
	static int16_t hold=0;
	if((val-hold>1)||(hold-val>1))hold=val;
	return (uint16_t)hold;
}

/**
 * @Brief Stabilisation and holding filter for Air Pressure sensor
 */
static uint16_t slide_filter_airp(int16_t val){
	static int16_t hold=0;
	if((val-hold>1)||(hold-val>1))hold=val;
	return (uint16_t)hold;
}

/**
 * @Brief Stabilisation and holding filter for Pump Current
 */
static uint16_t slide_filter_pump(int16_t val){
	static int16_t hold=0;
	if((val-hold>1)||(hold-val>1))hold=val;
	return (uint16_t)hold;
}

/**
 * @Brief Convert ADC values to actual voltage in format 500 as 5.00V
 */
static uint16_t adc_to_voltage(uint16_t v,uint16_t ref){
	uint32_t ref_v;
	uint32_t val=((uint32_t)v)*FIX_MATS_R;
	ref_v=((uint32_t)ref)>=BANGAP_REF?(uint32_t)ref:BANGAP_REF;
	val=(val/ref_v)*BANGAP_REF;
	return TO_V16b_2D(val);
}

/**
 * @Brief Get car supply voltage as VVV where VV.V Volts
 */
int16_t supply_voltage(void){
	uint16_t ref, supply;
	disturb:
	__adc_impact=0U;
	ref=analog.bandgap_vref;
	supply=analog.supply_voltage;
	if(__adc_impact)goto disturb;		
	supply=slide_filter_supp((int16_t)supply);
	supply=adc_to_voltage(supply,ref);
	supply=CAR_SUPP_V(supply);
	/*---- Analog error diagnostic ----*/
	if(((supply<HBI1_MIN_DEASSERT)&&(supply>LBI4_MAX_DEASSERT))&&
		(health.over_v||health.undr_v))return BATS_IN_N_RANGE;
	else if((supply>HBI1_MAX_ASSERT)&&(!health.over_v))return BATS_NO_OV_FLAG;
	else if((supply<LBI4_MIN_ASSERT)&&(!health.undr_v))return BATS_NO_UV_FLAG;
	/*---------------------------------*/
	supply+=S1J_FV_DROP;
	return (int16_t)supply;
}

/**
 * @Brief Get pressure offset after Power ON or Wake up
 */
uint8_t plug_p_offset(int16_t trigger){ // TODO: Make whole sequence with drain
	static uint32_t notch=0UL;
	static uint8_t progress=(uint8_t)0U;
	static uint16_t samples=0U;
	/***/
	if(trigger&&(!progress)){
		progress=(uint8_t)1U;
	}
	else{
		switch(progress){
			case (uint8_t)1U:
				samples=0U;
				notch=__get_millis;
				progress++;
			case (uint8_t)2U:
				if(ms_from(notch)>=PNUMO_BASIC_TIM)progress++;
			break;	
			case (uint8_t)3U:
				for(;pneumatic_drain(_ON_);){};
				notch=__get_millis;
				progress++;
			case (uint8_t)4U:
				if(ms_from(notch)>=
				  (PNUMO_VALVE_TIM+PNUMO_EXTRA_TIM)){
					notch=__get_millis;
					pressure_offset=0U;
					progress++;
					--notch;
				}
			break;
			case (uint8_t)5U:
				if(samples<PNUMO_REF_SAMPL){
					if(notch^__get_millis){
						pressure_offset+=analog.air_pressure;
						notch=__get_millis;
						samples++;
					}
				}
				else{
					pressure_offset/=samples;
					notch=__get_millis;
					progress++;
				}
			break;
			case (uint8_t)6U:
				if(ms_from(notch)>=PNUMO_BASIC_TIM)progress++;
			break;
			case (uint8_t)7U:
				for(;pneumatic_drain(_OFF_);){};
				notch=__get_millis;
				progress++;
			case (uint8_t)8U:
				if(ms_from(notch)>=
				  (PNUMO_VALVE_TIM+PNUMO_BASIC_TIM)){
					progress=(uint8_t)0U;
				}
			break;
		}
	}
	return progress;
}

uint16_t tick_offset;

/**
 * @Brief Periodic pass-through task to maintain air pressure offset
 */
void scan_p_offset(int16_t trigger){
	static uint32_t letup;
	static uint32_t drain;
	static uint16_t inner_offset;
	static uint8_t active;
	static uint8_t res=(uint8_t)1U;
	/***/
	if(trigger){
		res=active=(uint8_t)1U;
		letup=__get_millis;
		return;
	}
	/***/
	if(__pneumo_static){		
		switch(active){
			case (uint8_t)1U:
				if(ms_from(letup)<PNUMO_WAIT_STAB)return;
				for(;pneumatic_drain(_ON_);){};
				letup=__get_millis;
				active++;
			case (uint8_t)2U:
				if(ms_from(letup)<PNUMO_DRAIN_TIM)return;
				for(;pneumatic_drain(_OFF_);){};
				letup=__get_millis;
				active++;
			case (uint8_t)3U:
				if(ms_from(letup)<PNUMO_CLOSE_DRN)return;
				drain=__get_millis;
			    active=(uint8_t)0U;
			break;
			default:
				if((ms_from(letup)>=PNUMO_TAKE_POFF)||res){
					pressure_offset=analog.air_pressure;
					
					tick_offset++; /* leave for debug purposes */
					
					if(res){
						inner_offset=pressure_offset;
						res=(uint8_t)0U;
					}
					if(pressure_offset>=inner_offset){
						/* TODO: I want to add extra feature here, no time now!!!! */
						if((pressure_offset-inner_offset)>OFFSET_DISTURBE){
							pressure_offset=inner_offset;
							active=(uint8_t)1U;
							letup=__get_millis;			
						}
						else inner_offset=pressure_offset;
					}
					else{
						/* TODO: I want to add extra feature here, no time now!!!! */
						if((inner_offset-pressure_offset)>OFFSET_DISTURBE){
							pressure_offset=inner_offset;
							active=(uint8_t)1U;
							letup=__get_millis;			
						}
						else inner_offset=pressure_offset;
					}
					letup=__get_millis;
				}
				else if(ms_from(drain)>=REDRAIN_PNUMO_S){
					res=active=(uint8_t)1U;
					letup=__get_millis;
				}
			break;
		}	
	}
	else{
		active=(uint8_t)1U;
		letup=__get_millis;
	}	
	return;
}

/**
 * @Brief Get differential pneumatic system pressure in kPa
 */
int8_t air_pressure(void){
	uint16_t ref, pressure;
	disturb:
	__adc_impact=0U;
	ref=analog.bandgap_vref;
	pressure=analog.air_pressure;
	if(__adc_impact)goto disturb;
	pressure=pressure>=pressure_offset?
			 pressure-pressure_offset:0U;
	pressure=slide_filter_airp((int16_t)pressure);
	pressure=adc_to_voltage(pressure,ref);
	pressure=TO_P_kPa_16B(pressure);
	pressure=pressure>MAX_DIFF_PRESS?
			 MAX_DIFF_PRESS:pressure;
	return (int8_t)pressure;
}

/**
 * @Brief Get Pump current in format 050 as 0.5A
 */
int16_t pump_current(void){
	uint16_t ref, current;
	static uint32_t letup;
	static uint8_t latch;
	disturb:
	__adc_impact=0U;
	ref=analog.bandgap_vref;
	current=analog.pump_current;
	if(__adc_impact)goto disturb;
	current=slide_filter_pump((int16_t)current);
	if(__pump_active){	
		if(latch){
			if(ms_from(letup)>=PUMP_TON_FAULT)latch=(uint8_t)0U;
		}
		else{
			if(current>PUMP_FAULT_HI)return PUMP_FAIL_SHORT;
			if(current<PUMP_FAULT_LO)return PUMP_FAIL_OPENL;
		}
	}
	else{
		latch=(uint8_t)1U;
		letup=__get_millis;
	}
	current=adc_to_voltage(current,ref);
	current=VCS_TO_I16b_2D(current);
	return (int16_t)current;
}

