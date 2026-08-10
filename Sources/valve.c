/*
 * valve.c
 *
 *  Created on: Nov 10, 2022
 *      Author: Nick
 */

/* Soft SPI CS block */
#define __FETCH_MSG1_R (PTT&=~((uint8_t)0x02U))
#define __FETCH_LUMBAR (PTT&=~((uint8_t)0x04U))
#define __FETCH_MSG2_L (PTT&=~((uint8_t)0x08U)) 

#define __RELEASE_ALLS (PTT|=(uint8_t)0x0EU)

/* SPI commands */
#define SPTEF_FLAG ((uint8_t)0x20U)
#define __ARM_SPI_t (TIM1TCNT=0x00U)
/***************************/

#include "valve.h"

#if(0)
uint16_t const p_drop_corr[]={
	  /*  1   2   3   4   5   6   7   8   9	 10	*/					
	  0, 20, 20, 21, 21, 22, 22, 22, 23, 23, 24,
	  	 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 
	  	 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 
	  	 35, 36, 37, 38, 38, 39, 40, 41, 42, 43, 
	  	 43, 44, 45, 46, 47, 48, 48, 49, 50, 51, 
	  	 52, 53, 54, 55, 56, 56, 57, 58, 59, 60,
};

uint16_t const p_drop_corr[]={
	  /*  1   2   3   4   5   6   7   8   9	 10	*/					
	  0,  2,  4, 10, 12, 13, 13, 14, 15, 16, 16,	  
	  	 17, 18, 19, 19, 20, 21, 22, 23, 23, 24, 	  	 
	  	 25, 26, 27, 27, 28, 29, 30, 31, 32, 32, 	 
	  	 33, 34, 35, 36, 37, 38, 39, 39, 40, 41,   	 
	  	 42, 43, 44, 45, 46, 47, 47, 48, 49, 50,  	 
	  	 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
};
#else
uint16_t const p_drop_corr[]={
	  /*  1   2   3   4   5   6   7   8   9	 10	*/					
	  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10,	  
	  	 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 	  	 
	  	 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 	 
	  	 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,   	 
	  	 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,  	 
	  	 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
};
#endif

/************************************************/
volatile spi_work_t spi_work;
uint16_t valves_open; /* External interaction for pressure sensor */
drain_sys_t drain;
mem_status_t mem_status;
/***********************************************/

volatile uint8_t spi_lock;

interrupt VectorNumber_Vtim1ch0 spi_service(void){
	TIM1TFLG1=(uint8_t)0x01U;
	if(spi_work.busy){
		spi_work.busy=(uint8_t)0U;
		__RELEASE_ALLS;
		/****/
		switch(spi_work.request){
			case _R_SIDE: spi_work.r_side_st|=SPI0DR&MSR_VALVES_MASK; break;
			case _L_SIDE: spi_work.l_side_st|=SPI0DR&MSL_VALVES_MASK; break;
			case _V_LMB:  spi_work.lumb_v_st|=SPI0DR&LMB_VALVES_MASK; break;
			default: break;
		}
		spi_work.drd=(uint8_t)1U;
	}
	return;	
}

#if(0)
/**
 * @Brief SPI Interrupt handler
 */
interrupt VectorNumber_Vspi0 spi_rx_evt(){
	__RELEASE_ALLS;
	SPI0SR=SPI0SR;
	if(spi_work.req){
		spi_work.spi_return=SPI0DR;
		//spi_work.drd=spi_work.req;
		spi_work.req=(uint8_t)0U;
	}
	else SPI0DR;
	spi_work.busy=(uint8_t)0U;
	return;
}
#endif

/******************************************/

// TODO: Test spi transmission
void test_spi_tx(uint16_t data){ /* Remove this garbage */
	if(SPI0SR&SPTEF_FLAG){
		
		PTT&=~((uint8_t)0x02U);
					
		//SPI0DR=data;
		SPI0DRH=(uint8_t)(data>>8);
		SPI0DRL=(uint8_t)(data&0x00FFU);

		__ARM_SPI_t;
		__SPI_STATE->busy=(uint8_t)1U;
		//for(;SPI0SR&0x20U;){};
		//SPI0CR1|=0x20U;
	}
	return;
}

uint8_t spi_tx_word(int16_t target, uint16_t data){
	static uint8_t msg_1r_valves=(uint8_t)0U;
	static uint8_t lmb_ns_valves=(uint8_t)0U;
	static uint8_t msg_2l_valves=(uint8_t)0U;
	for(;!(SPI0SR&SPTEF_FLAG);){};	
	switch((spi_work.request=target)){
		case _R_SIDE:
			__FETCH_MSG1_R;
			if((data&0xFF00U)==SPI_CMD_VALVE)msg_1r_valves=(uint8_t)(data&0x00FFU);
		break;
		case _V_LMB:
			__FETCH_LUMBAR;
			if((data&0xFF00U)==SPI_CMD_VALVE)lmb_ns_valves=(uint8_t)(data&0x00FFU);
		break;
		case _L_SIDE:
			__FETCH_MSG2_L;
			if((data&0xFF00U)==SPI_CMD_VALVE)msg_2l_valves=(uint8_t)(data&0x00FFU);
		break;
		default: return (uint8_t)0U;
	}
	if(msg_1r_valves||lmb_ns_valves||msg_2l_valves){
		 valves_open=1U;
	}
	else valves_open=0U;		
	SPI0DR=data;
	__ARM_SPI_t;
	return (uint8_t)1U;
}

/**
 * @Brief Get isolated diagnostic value for specified subsystem IC
 */
static uint16_t read_valves_reply(int16_t subsys, uint8_t erase){
	/****/
	uint16_t reply=(uint8_t)0U;
	/****/
	refresh:
	__SPI_STATE->drd=(uint8_t)0U;
	if(erase){
		switch(subsys){
			case _R_SIDE: spi_work.r_side_st=0U; break;
			case _L_SIDE: spi_work.l_side_st=0U; break;
			case _V_LMB:  spi_work.lumb_v_st=0U; break;
			default: break;
		}
	}
	else{
		switch(subsys){
			case _R_SIDE: reply=spi_work.r_side_st; break;
			case _L_SIDE: reply=spi_work.l_side_st; break;
			case _V_LMB:  reply=spi_work.lumb_v_st; break;
			default: break;
		}
	}
	if(__SPI_STATE->drd)goto refresh;
	return reply;
}

/**
 * @Brief Get DCTs status pending 
 */
uint8_t get_dtcs_status(void){
	/****/
	uint16_t reply;
	/* Valves test ****/
	dct_update:
	__SPI_STATE->drd=(uint8_t)0U;
	reply=spi_work.r_side_st|spi_work.l_side_st|spi_work.lumb_v_st;
	if(__SPI_STATE->drd)goto dct_update;
	/* Pump test ******/
	reply|=get_pump_dtcs((uint8_t)0U);	
	return (uint8_t)(!(!reply));
}

/**
 * @Brief Return specially compiled byte for Report (valve diagnostics) based on SPI return data.
 */
uint8_t get_valve_diagnostics(int16_t request_id, uint8_t counter){
	/****/
	static uint16_t raw_reply=(uint8_t)0U;
	/****/
	uint8_t reply=(uint8_t)0U;
	uint8_t processing=(uint8_t)0U;
	/****/
	switch(request_id){
		case Seat_lumbar_support_valve_status:		
			switch(counter){
				case (uint8_t)1U:
					raw_reply=read_valves_reply(_V_LMB,(uint8_t)0U);
					 reply=(raw_reply&0x0300U)?((uint8_t)0x20U):((uint8_t)0+lumbar.val.upp_flat);
					reply|=(raw_reply&0x0030U)?((uint8_t)0x08U):((uint8_t)0+lumbar.val.mid_flat);
					reply|=(raw_reply&0x0003U)?((uint8_t)0x02U):((uint8_t)0+lumbar.val.low_flat);
				break;
				default:
					 reply=(raw_reply&0x0C00U)?((uint8_t)0x20U):((uint8_t)0+lumbar.val.upp_fill);					 
					reply|=(raw_reply&0x00C0U)?((uint8_t)0x08U):((uint8_t)0+lumbar.val.mid_fill);					
					reply|=(raw_reply&0x000CU)?((uint8_t)0x02U):((uint8_t)0+lumbar.val.low_fill);
				break;
			}
		break;
		case Seat_massage_control_valve_status:
			switch(counter){
				case (uint8_t)3U:
					raw_reply=read_valves_reply(_R_SIDE,(uint8_t)0U);
					 reply=(raw_reply&0x0300U)?((uint8_t)0x02U):((uint8_t)0+msg_1R.valve.bag_5);
				break;
				case (uint8_t)2U:
					 reply=(raw_reply&0x00C0U)?((uint8_t)0x80U):((uint8_t)0+msg_1R.valve.bag_4);
					reply|=(raw_reply&0x0030U)?((uint8_t)0x20U):((uint8_t)0+msg_1R.valve.bag_3);
					reply|=(raw_reply&0x000CU)?((uint8_t)0x08U):((uint8_t)0+msg_1R.valve.bag_2);
					reply|=(raw_reply&0x0003U)?((uint8_t)0x02U):((uint8_t)0+msg_1R.valve.bag_1);
				break;
				case (uint8_t)1U:
					raw_reply=read_valves_reply(_L_SIDE,(uint8_t)0U);
					 reply=(raw_reply&0x0003U)?((uint8_t)0x02U):((uint8_t)0+msg_2L.valve.bag_5);
				break;
				default:
					 reply=(raw_reply&0x000CU)?((uint8_t)0x80U):((uint8_t)0+msg_2L.valve.bag_4);		 
					reply|=(raw_reply&0x0030U)?((uint8_t)0x20U):((uint8_t)0+msg_2L.valve.bag_3);
					reply|=(raw_reply&0x00C0U)?((uint8_t)0x08U):((uint8_t)0+msg_2L.valve.bag_2);	
					reply|=(raw_reply&0x0300U)?((uint8_t)0x02U):((uint8_t)0+msg_2L.valve.bag_1);	
				break;
			}
		break;
		case Seat_side_support_valve_status:
			switch(counter){
				case (uint8_t)2U:
					raw_reply=read_valves_reply(_R_SIDE,(uint8_t)0U);
					 reply=(raw_reply&0x0C00U)?((uint8_t)0x02U):((uint8_t)0+msg_1R.valve.drain);
				break;
				case (uint8_t)1U:
					 reply=(raw_reply&0xC000U)?((uint8_t)0x08U):((uint8_t)0+msg_1R.valve.backrest_r);
					raw_reply=read_valves_reply(_L_SIDE,(uint8_t)0U);
					reply|=(raw_reply&0x3000U)?((uint8_t)0x02U):((uint8_t)0+msg_2L.valve.cushion_r);			
				break;
				default:
					raw_reply=read_valves_reply(_R_SIDE,(uint8_t)0U);
					 reply=(raw_reply&0x3000U)?((uint8_t)0x08U):((uint8_t)0+msg_1R.valve.backrest_l);
					raw_reply=read_valves_reply(_L_SIDE,(uint8_t)0U);
					reply|=(raw_reply&0x0C00U)?((uint8_t)0x02U):((uint8_t)0+msg_2L.valve.cushion_l);
				break;	
			}
		break;
		case DCTs_lumbar_valves_errors:
			switch(counter){
				case (uint8_t)3U:
					raw_reply=read_valves_reply(_V_LMB,(uint8_t)0U);
					if(raw_reply&0x0300U){
						processing=(uint8_t)((raw_reply&0x0300U)>>8U);
						if(processing==(uint8_t)3U)reply=(uint8_t)0x10U;
						else if(processing==(uint8_t)2U)reply=(uint8_t)0x02U;
						else reply=(uint8_t)0x08U;
					}
				break;
				case (uint8_t)2U:
					if(raw_reply&0x0030U){
						processing=(uint8_t)((raw_reply&0x0030U)>>4U);
						if(processing==(uint8_t)3U)reply=(uint8_t)0x80U;
						else if(processing==(uint8_t)2U)reply=(uint8_t)0x10U;
						else reply=(uint8_t)0x40U;
					}
					if(raw_reply&0x0003U){
						processing=(uint8_t)(raw_reply&0x0003U);
						if(processing==(uint8_t)3U)reply|=(uint8_t)0x04U;
						else if(processing==(uint8_t)2U)reply|=(uint8_t)0x00U; /* Dummy operation LSB will go to MSB of next byte */
						else reply|=(uint8_t)0x02U;
					}
				break;
				case (uint8_t)1U:
					if(raw_reply&0x0003U){
						processing=(uint8_t)(raw_reply&0x0003U);
						if(processing==(uint8_t)2U)reply=(uint8_t)0x80U;
					}
					if(raw_reply&0x0C00U){
						processing=(uint8_t)((raw_reply&0x0C00U)>>10U);
						if(processing==(uint8_t)3U)reply|=(uint8_t)0x20U;
						else if(processing==(uint8_t)2U)reply|=(uint8_t)0x04U;	
						else reply|=(uint8_t)0x10U;	
					}
					if(raw_reply&0x00C0U){
						processing=(uint8_t)((raw_reply&0x00C0U)>>6U);
						if(processing==(uint8_t)3U)reply|=(uint8_t)0x01U;
					}
				break;
				default:
					if(raw_reply&0x00C0U){
						processing=(uint8_t)((raw_reply&0x00C0U)>>6U);
						if(processing==(uint8_t)2U)reply=(uint8_t)0x20U;	
						else reply=(uint8_t)0x80U;	
					}
					if(raw_reply&0x000CU){
						if(processing==(uint8_t)3U)reply|=(uint8_t)0x08U;
						else if(processing==(uint8_t)2U)reply|=(uint8_t)0x01U;
						else reply|=(uint8_t)0x04U;
					}
				break;
			}	
		break;
			
		default: break;
	
	
	
	}
	
	
	return reply;
}

/**
 * @Brief Open/Close pneumatic system drain valve
 */
uint8_t pneumatic_drain(int16_t toggle){
	if(__SPI_STATE->busy)return (uint8_t)1U;
	drain.open=(uint16_t)(!(!toggle));
	msg_1R.valve.drain=(uint8_t)drain.open;
	/***/
	for(;!(SPI0SR&SPTEF_FLAG);){};	
	__FETCH_MSG1_R;
	SPI0DR=(SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
	__ARM_SPI_t;
	__SPI_STATE->busy=(uint8_t)1U;
	/***/
	msg_1R.valve.drain=(uint8_t)0U;
	return (uint8_t)0U;
}

/**
 * @Brief Adaptive Slide filter for pressure measurement
 */
uint8_t pressure_hoarder(uint8_t max_press, int16_t res){ // TODO: Replace all numeric values with macro definitions
	static uint8_t vector[10]={0};
	static uint32_t notch=(uint32_t)0;
	uint16_t pressure=(uint16_t)0U;
	int16_t i,j;
	/***/
	if(res){
		for(i=0;i<10;){vector[i++]=(uint8_t)0U;};
		return (uint8_t)0U;
	}
	/***/
	if(notch^__get_millis){
		for(i=9;i;i--){vector[i]=vector[i-1];};
		vector[i]=air_pressure();
		notch=__get_millis;
	}
	j=10-((((int16_t)max_press)-((int16_t)FLT_BAG_PRESS))/10);
	j=(j>10)?(10):j;
	if(j){
		for(i=0;i<j;){pressure+=(uint16_t)vector[i++];};
		pressure/=(uint16_t)j;
	}
	else pressure=(uint16_t)vector[0];
	return (uint8_t)pressure;
}

/**
 * Demo function - in final version this should be total energy save solver
 */
uint8_t operation_mode(int16_t toggle, uint8_t memory){ 	// TODO: Add Side support drain and restore
	static uint8_t pass=(uint8_t)0U;
	static uint8_t next=(uint8_t)0U;
	static uint8_t feed=(uint8_t)0U;
	static int16_t state=_OFF_;
	static int16_t operate=_NA_;
	static int16_t valid_b=_NA_;
	static uint32_t notch=0UL;
	static uint8_t err_p=(uint8_t)0U;
	static lmb_press_t hold={
		(uint8_t)0U,
		(uint8_t)0U,
		(uint8_t)0U,
		(uint8_t)0U,
	};
	static int16_t calc=0; 
	uint16_t buffer;
	/***/
	if(state^toggle){
		if(!pass){
			state=toggle;
			pass=(uint8_t)1U;
		}	
	}
	else if(pass){
		if(!(__SPI_STATE->busy)){
			if(state&&(!err_p)){
				switch(pass){
					case (uint8_t)1U:
#if(!LEGACY_OFF)	
						if(lmb_press.max_p>AEM_BAG_PRESS){
							err_p=(uint8_t)1U;
						}
						else{
#endif							
							if(next){
								VDD_MON_TOGGLE(_ON_);
								VDD_DRV_TOGGLE(_ON_);
								VDD_PSN_TOGGLE(_ON_);
							}
							if(memory){
								if(!((memory==mem_status.last_m_lmb)&&
									  mem_status.edited_lmb)){
									memory*=MEM_LUMBAR_STEP;
									read_16w(memory,&buffer);
									hold.max_p=(uint8_t)((buffer>>8U)&0xFFU);
									hold.upper=(uint8_t)(buffer&0xFFU);
									memory+=(uint8_t)2U;
									read_16w(memory,&buffer);
									hold.middl=(uint8_t)((buffer>>8U)&0xFFU);
									hold.lower=(uint8_t)(buffer&0xFFU);
									
									// TODO: Add protection against default 0xFFFF writes in EEPROM 
								}	
							}
							notch=__get_millis;
							pass++;
#if(!LEGACY_OFF)
						}
#endif
					break;
					case (uint8_t)2U:
						if(ms_from(notch)>=BASE_PNUMO_TIM){
							if(next)plug_p_offset(_TRIG_);
							notch=__get_millis;
							pass++;
						}
					break;
					case (uint8_t)3U:
						if(next){
							if(!plug_p_offset(_RUN_)){
								notch=__get_millis;
								pass++;
							}
						}
						else{
							notch=__get_millis;
							pass++;
						}
					break;
					case (uint8_t)4U:
						if(ms_from(notch)>=BASE_PNUMO_TIM){
							notch=__get_millis;
							pass++;
						}
					break;
					case (uint8_t)5U:
						if(hold.upper){	
							if(hold.upper>AEM_BAG_PRESS){
								lumbar.val.upp_fill=(uint8_t)1U;
								calc++;
							}
							if(hold.middl>AEM_BAG_PRESS){
								lumbar.val.mid_fill=(uint8_t)1U;
								calc++;
							}
							if(calc/2){
								if(hold.upper<hold.middl)operate=_UP_;
								else operate=_MD_;
							}
							else if(calc){
								if(hold.middl>AEM_BAG_PRESS)operate=_MD_;
								else operate=_UP_;
							}
							else operate=_NA_;
						}
						else{
							if(hold.lower>AEM_BAG_PRESS){
								lumbar.val.low_fill=(uint8_t)1U;
								calc++;
							}
							if(hold.middl>AEM_BAG_PRESS){
								lumbar.val.mid_fill=(uint8_t)1U;
								calc++;
							}
							if(calc/2){
								if(hold.lower<hold.middl)operate=_LO_;
								else operate=_MD_;
							}
							else if(calc){
								if(hold.middl>AEM_BAG_PRESS)operate=_MD_;
								else operate=_UP_;
							}
							else operate=_NA_;
						}
						if((!calc)&&(hold.max_p>FLT_BAG_PRESS)){
							hold.middl=hold.max_p;
							lumbar.val.mid_fill=(uint8_t)1U;
							operate=_MD_;
							calc=1;
						}
						notch=__get_millis;
						pass++;	
					break;
					case (uint8_t)6U:
						if(ms_from(notch)>=ISOLATE_PUMP_W){
							if(operate){
								pump_force(MAX_DUTY);
								notch=__get_millis;
								pass++;
							}
							else pass=(uint8_t)11U; /// Apply setting, this should be natural number		
						}
					break;
					case (uint8_t)7U:
						if(ms_from(notch)>=__ONLY_P_SENSE){
							switch(operate){
								case _UP_:
									if((lmb_press.upper=air_pressure())
										>(hold.upper+AEM_BAG_PRESS)){
										lumbar.val.upp_fill=(uint8_t)0U;
									}
								break;
								case _MD_:
									if((lmb_press.middl=air_pressure())
										>(hold.middl+AEM_BAG_PRESS)){
										lumbar.val.mid_fill=(uint8_t)0U;
									}	
								break;
								case _LO_:
									if((lmb_press.lower=air_pressure())
										>(hold.lower+AEM_BAG_PRESS)){
										lumbar.val.low_fill=(uint8_t)0U;
									}	
								break;
							}
							notch=__get_millis;
							pass++;
							calc--;
						}
					break;
					case (uint8_t)8U:
						if(ms_from(notch)>=ISOLATE_PUMP_W){
							if(calc){
								valid_b=operate;
								if(operate^_MD_)operate=_MD_;
								else{
									if(hold.upper)operate=_UP_;
									else operate=_LO_;
								}
								calc=(uint8_t)0U;
								pass++;	
							}
							else{
								pump_force(MIN_DUTY);
								pass=(uint8_t)12U; /// Apply setting, this should be natural number	
							}
							notch=__get_millis;
						}	
					break;
					case (uint8_t)9U:
						if(ms_from(notch)>=ISOLATE_PUMP_W){
							switch(operate){
								case _UP_:
									if((lmb_press.upper=air_pressure())
										>(hold.upper+AEM_BAG_PRESS)){
										lumbar.val.upp_fill=(uint8_t)0U;
										pass++;
									}
								break;
								case _MD_:
									if((lmb_press.middl=air_pressure())
										>(hold.middl+AEM_BAG_PRESS)){
										lumbar.val.mid_fill=(uint8_t)0U;
										pass++;
									}
								break;
								case _LO_:
									if((lmb_press.lower=air_pressure())
										>(hold.lower+AEM_BAG_PRESS)){
										lumbar.val.low_fill=(uint8_t)0U;
										pass++;
									}
								break;
							}
							notch=__get_millis;
						}
					break;
					case (uint8_t)10U:
						if(ms_from(notch)>=ISOLATE_PUMP_W){
							pump_force(MIN_DUTY);
							notch=__get_millis;
							pass=(uint8_t)13U; /// Apply setting, this should be a natural number	
						}
					break;	
					case (uint8_t)11U:
						if(next)lmb_press=hold;
						else next=(uint8_t)1U;
						pass=(uint8_t)14U; ///// One other!
					break;	
					case (uint8_t)12U:
						if(ms_from(notch)>=__ONLY_P_SENSE){
							lmb_press.max_p=hold.max_p;
							if(operate^_UP_)lmb_press.upper=hold.upper;
							if(operate^_MD_)lmb_press.middl=hold.middl;
							if(operate^_LO_)lmb_press.lower=hold.lower;
							pass=(uint8_t)14U; //// One other!
						}
					break;
					case (uint8_t)13U:
						if(ms_from(notch)>=__ONLY_P_SENSE){
							lmb_press.max_p=hold.max_p;
							if((valid_b^_UP_)&&(operate^_UP_))lmb_press.upper=hold.upper;
							if((valid_b^_MD_)&&(operate^_MD_))lmb_press.middl=hold.middl;
							if((valid_b^_LO_)&&(operate^_LO_))lmb_press.lower=hold.lower;
							pass=(uint8_t)14U; //// One other!
						}
					break;
					case (uint8_t)14U:
						mem_status.last_m_lmb=memory/MEM_LUMBAR_STEP; // TODO: Revise after apply_lumbar tests, probably will require division on MEM_LUMBAR_STEP
						lmb_correct_p();
						scan_p_offset(_TRIG_);
						pass=(uint8_t)0U;
					break;
				}	
			}
			else{
				switch(pass){
					case (uint8_t)1U:
						if(!err_p){
							hold.max_p=lmb_press.max_p;
							hold.upper=lmb_press.upper;
							hold.middl=lmb_press.middl;
							hold.lower=lmb_press.lower;			
						}
						/***/
						if((lmb_press.upper>=lmb_press.middl)&&
						   (lmb_press.middl>=lmb_press.lower)){
							lumbar.val.upp_fill=(uint8_t)1U;
						}
						else if((lmb_press.lower>=lmb_press.middl)&&
								(lmb_press.middl>=lmb_press.upper)){
							lumbar.val.low_fill=(uint8_t)1U;
						}
						else{
							lumbar.val.mid_fill=(uint8_t)1U;
						}
						lumbar.val.upp_flat=lumbar.val.mid_flat=
											lumbar.val.low_flat=(uint8_t)1U;
						notch=__get_millis;
						pass++;
					break;
					case (uint8_t)2U:
						if(ms_from(notch)>=__ONLY_P_SENSE)pass++;
					break;
					case (uint8_t)3U:
						if(air_pressure()<AEM_BAG_PRESS){  // Changed to AEM... because of more deep drain required, was FLT_BAG_PRESS
							lumbar.val.upp_fill=(uint8_t)0U;
							lumbar.val.mid_fill=(uint8_t)0U;
							lumbar.val.low_fill=(uint8_t)0U;
							notch=__get_millis;
							pass++;
						}
					break;
					case (uint8_t)4U:
						if(ms_from(notch)>=500UL){
							lumbar.val.upp_flat=(uint8_t)0U;
							lumbar.val.mid_flat=(uint8_t)0U;
							lumbar.val.low_flat=(uint8_t)0U;
							notch=__get_millis;
							pass++;
						}
					break;
					case (uint8_t)5U:
						if(ms_from(notch)>=BASE_PNUMO_TIM){
							lmb_press.upper=lmb_press.middl=
							lmb_press.lower=(uint8_t)0U;
							lmb_press.max_p=(uint8_t)0U;
							notch=__get_millis;
							/***/
							if(err_p){
								err_p=(uint8_t)0U;
								pass=(uint8_t)1U;
								break;
							}
							pass++;
						}
					break;
					case (uint8_t)6U:
						if(ms_from(notch)>=__ONLY_P_SENSE){
							VDD_MON_TOGGLE(_OFF_);
							VDD_PSN_TOGGLE(_OFF_);
							notch=__get_millis;
							pass++;
						}
					break;
					case (uint8_t)7U:
						if(ms_from(notch)>=BASE_PNUMO_TIM){
							
							// TODO: Toggle MCU to sleep state in this "case", better - next down 
							
							VDD_DRV_TOGGLE(_OFF_);
							notch=__get_millis;
							pass++;
						}
					break;
					case (uint8_t)8U:
						if(ms_from(notch)>=BASE_PNUMO_TIM)pass=(uint8_t)0U;
					break;
				}			
			}
			if(feed^lumbar.feed_cmd){
				feed=lumbar.feed_cmd;
				__SPI_STATE->busy=spi_tx_word(_V_LMB,SPI_CMD_VALVE|(uint16_t)feed);
			}	 
		}
	}	
	return pass;
}

#define DEF_TASK_LMB (1)
#define DEF_TASK_SSA (2)

/**
 * @Brief Schedule task to check pressures in pneumatic system
 */
uint8_t schedule_check(uint8_t lumbar_activity,
					   uint8_t side_s_activity, 
					   uint8_t ambient_temp,
					   uint8_t bcm_eem,
					   uint8_t impact){
	/****/
	static uint8_t temp_hold=(uint8_t)0U;
	static uint8_t impact_act=(uint8_t)0U;
	static uint8_t deferred=(uint8_t)0U;
	static uint8_t was_out=(uint8_t)0U;
	static uint32_t notch=0UL;
	static uint32_t def_tim=0UL;
	uint8_t run=(uint8_t)0U;
	/****/
	if(!drain.open){
		if(impact|impact_act){
			
		}
		else{
			if(run){
				
			}
			else{
				if(lumbar_activity){
					if((deferred)&&(was_out)){
						was_out=(uint8_t)0U;
						def_tim-=DECR_P_TIM;
					}
					else if(!def_tim)def_tim=DEFERRED_P_TIM;
					deferred=DEF_TASK_SSA;
				}
				else if(side_s_activity){
					if((deferred)&&(was_out)){
						was_out=(uint8_t)0U;
						def_tim-=DECR_P_TIM;
					}
					else if(!def_tim)def_tim=DEFERRED_P_TIM;
					deferred=DEF_TASK_LMB;
					
				}
				else if(deferred){
					was_out=(uint8_t)1U;
					
				}
				else if(temp_hold^ambient_temp){
					
				}
				else if(ms_from(notch)>=SCHEDULE_P_TIM){
					
				}
			}
		}
	}


	
	
	return (uint8_t)1U;
}
