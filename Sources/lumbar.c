/*
 * lumbar.c
 *
 *  Created on: Jan 25, 2023
 *      Author: Nick
 */

/**
  @file lumbar.c
  @version 1.0
  @brief Содержит функции управления поясничной поддержкой блока MLSM      
*/  

/**
  @addtogroup Поясничная_поддержка
  @{
*/

/****/
#define DEFLAT_P_TH(P) ((uint8_t)((((uint16_t)(P))*3U)/4U))
/****/

#define LMB_INLETS__ ((uint8_t)0x2AU) /* Lumbar valves inlets mask */

enum{
	_NO_Bt_,
	_UP_Bt_,
	_DN_Bt_,
	_IN_Bt_,
	_DE_Bt_
};

#define LOW_PRESSURE ((uint8_t)3U)
#define DEFLAT_ADDER(P) (((P)>AEM_BAG_PRESS)?(AEM_BAG_PRESS):(LOW_PRESSURE))

#include "lumbar.h"

/**
 * @Brief Lumbar bit-valves structure
 */
lumbar_t lumbar;

/**
 * @Brief Lumbar bags pressure holder & target pressure holder for diagnostics
 */
lmb_press_t lmb_press;
lmb_press_t lmb_memory;

/**
  @brief Correct air pressure measurements on adjust||apply exit
  @param Function uses global structures
  @return Nothing - modifies global structures
  @details Сова летает по кругу на кордовой ручке
           и никак не может остановиться.
  @callgraph
 */
void lmb_correct_p(void){
	if((lmb_press.middl>=lmb_press.upper)&&
	   (lmb_press.middl>=lmb_press.lower)){
		if(lmb_press.upper>lmb_press.lower){
			lmb_press.lower=(uint8_t)0U;
		}
		else if(lmb_press.lower>lmb_press.upper){
			lmb_press.upper=(uint8_t)0U;
		}
		else{
			lmb_press.upper=(uint8_t)0U;
			lmb_press.lower=(uint8_t)0U;
		}
	}
	else if((lmb_press.upper>=lmb_press.middl)&&
			(lmb_press.middl>=lmb_press.lower)){
		lmb_press.lower=(uint8_t)0U;
	}
	else if((lmb_press.lower>=lmb_press.middl)&&
			(lmb_press.middl>=lmb_press.upper)){
		lmb_press.upper=(uint8_t)0U;
	}
	else{
		lmb_press.upper=(uint8_t)0U;
		lmb_press.lower=(uint8_t)0U;	
	}
	return;
}

#if(0)
uint16_t lumb_up_press;
uint16_t lumb_md_press;
uint16_t lumb_dn_press;
uint16_t lmb_press_max;
#endif

/**
  @brief Correct air pressure measurements on adjust||apply exit
  @param Function uses global structures
  @return Nothing - modifies global structures
  @details Сова летает по кругу на кордовой ручке
           и никак не может остановиться.
  @callgraph
 */
void read_lumbar(void){
	/* Get upper bag pressure */
	lumbar.val.upp_fill=(uint8_t)1U;
	__SPI_STATE->busy=spi_tx_word(_V_LMB,SPI_CMD_VALVE|(uint16_t)lumbar.feed_cmd);
	for(;__SPI_STATE->busy;){};
	ms_halt(ISOLATE_P_SENS);
	lmb_press.upper=air_pressure();
	lumbar.feed_cmd=(uint8_t)0U;
	__SPI_STATE->busy=spi_tx_word(_V_LMB,SPI_CMD_VALVE|(uint16_t)lumbar.feed_cmd);
	for(;__SPI_STATE->busy;){};
	ms_halt(ISOLATE_P_SENS);
	/* Get middle bag pressure */
	lumbar.val.mid_fill=(uint8_t)1U;
	__SPI_STATE->busy=spi_tx_word(_V_LMB,SPI_CMD_VALVE|(uint16_t)lumbar.feed_cmd);
	for(;__SPI_STATE->busy;){};
	ms_halt(ISOLATE_P_SENS);
	lmb_press.middl=air_pressure();
	lumbar.feed_cmd=(uint8_t)0U;
	__SPI_STATE->busy=spi_tx_word(_V_LMB,SPI_CMD_VALVE|(uint16_t)lumbar.feed_cmd);
	for(;__SPI_STATE->busy;){};
	ms_halt(ISOLATE_P_SENS);
	/* Get lower bag pressure */
	lumbar.val.low_fill=(uint8_t)1U;
	__SPI_STATE->busy=spi_tx_word(_V_LMB,SPI_CMD_VALVE|(uint16_t)lumbar.feed_cmd);
	for(;__SPI_STATE->busy;){};
	ms_halt(ISOLATE_P_SENS);
	lmb_press.lower=air_pressure();
	lumbar.feed_cmd=(uint8_t)0U;
	__SPI_STATE->busy=spi_tx_word(_V_LMB,SPI_CMD_VALVE|(uint16_t)lumbar.feed_cmd);
	for(;__SPI_STATE->busy;){};
	ms_halt(ISOLATE_P_SENS);
	
	lmb_correct_p();
	lmb_press.max_p=AEM_BAG_PRESS;
	lmb_press.max_p=lmb_press.max_p<lmb_press.upper?lmb_press.upper:lmb_press.max_p;
	lmb_press.max_p=lmb_press.max_p<lmb_press.middl?lmb_press.middl:lmb_press.max_p;
	lmb_press.max_p=lmb_press.max_p<lmb_press.lower?lmb_press.lower:lmb_press.max_p;
	if(lmb_press.max_p>MAX_BAG_PRESS)lmb_press.max_p=MAX_BAG_PRESS;
	
#if(0) /* Feed uint16_t for debug monitor only */
	lumb_up_press=lmb_press.upper;
	lumb_md_press=lmb_press.middl;
	lumb_dn_press=lmb_press.lower;
#endif
	
	return;
}

/**
  @brief Save current memory position
 */
void save_lumbar(uint8_t memory){
	uint16_t buffer;
	/****/
#if(0)
	if((memory>MEM_SETINGS_MAX)||
	   (!mem_status.edited_lmb))return;
#else
	if((!memory)||(memory>MEM_SETINGS_MAX))return;
#endif
	memory=memory*MEM_LUMBAR_STEP;
	buffer=((uint16_t)lmb_press.max_p)<<8U;
	buffer|=(uint16_t)lmb_press.upper;
	write_16w(memory,buffer);
	memory+=(uint8_t)2U;
	buffer=((uint16_t)lmb_press.middl)<<8U;
	buffer|=(uint16_t)lmb_press.lower;
	write_16w(memory,buffer);
	/*****/
#if(1)
	memory=(memory-(uint8_t)2U)/MEM_LUMBAR_STEP;
	if(memory==mem_status.last_m_lmb)mem_status.edited_lmb=(uint8_t)0U;
#else
	mem_status.edited_lmb=(uint8_t)0U;
#endif
	/*****/
	return;
}

/**
  @brief Initial solver to define first bag to pump on memory change
 */
static int16_t fill_init_solver(lmb_press_t* memory){
	if(!memory->upper){
		lumbar.val.upp_flat=(uint8_t)1U;
		if((memory->lower>lmb_press.lower)&&
		   (memory->lower>AEM_BAG_PRESS)){
			if((memory->lower<lmb_press.middl)&&
			   (memory->middl>lmb_press.middl)){
				lumbar.val.mid_fill=(uint8_t)1U;
				return _MD_;
			}
			else{
				lumbar.val.low_fill=(uint8_t)1U;
				return _LO_;		
			}
		}
		else if((memory->middl>lmb_press.middl)&&
				(memory->middl>AEM_BAG_PRESS)){
			lumbar.val.mid_fill=(uint8_t)1U;
			return _MD_;
		}
	}
	else{
		lumbar.val.low_flat=(uint8_t)1U;
		if((memory->upper>lmb_press.upper)&&
		   (memory->upper>AEM_BAG_PRESS)){				
			if((memory->upper<lmb_press.middl)&&
			   (memory->middl>lmb_press.middl)){
				lumbar.val.mid_fill=(uint8_t)1U;
				return _MD_;
			}
			else{
				lumbar.val.upp_fill=(uint8_t)1U;
				return _UP_;
			}	
		}
		else if((memory->middl>lmb_press.middl)&&
				(memory->middl>AEM_BAG_PRESS)){
			lumbar.val.mid_fill=(uint8_t)1U;
			return _MD_;
		}
	}
	return _NA_;
}

/**
 * @Brief Initial solver to define first bag to flat on memory change
 */
static int16_t flat_init_solver(lmb_press_t* memory){
	if(!memory->upper){
		if((memory->lower+AEM_BAG_PRESS)<lmb_press.lower){
			if(((memory->middl+AEM_BAG_PRESS)<lmb_press.middl)&&
				(memory->middl<memory->lower)){
				lumbar.val.mid_fill=(uint8_t)1U;
				lumbar.val.mid_flat=(uint8_t)1U;
				return _MD_;
			}
			else{
				lumbar.val.low_fill=(uint8_t)1U;
				lumbar.val.low_flat=(uint8_t)1U;
				return _LO_;
			}
		}
		else if((memory->middl+AEM_BAG_PRESS)<lmb_press.middl){
			lumbar.val.mid_fill=(uint8_t)1U;
			lumbar.val.mid_flat=(uint8_t)1U;
			return _MD_;
		}
	}
	else{	
		if((memory->upper+AEM_BAG_PRESS)<lmb_press.upper){
			if(((memory->middl+AEM_BAG_PRESS)<lmb_press.middl)&&
				(memory->middl<memory->upper)){
				lumbar.val.mid_fill=(uint8_t)1U;
				lumbar.val.mid_flat=(uint8_t)1U;
				return _MD_;
			}
			else{
				lumbar.val.upp_fill=(uint8_t)1U;
				lumbar.val.upp_flat=(uint8_t)1U;
				return _UP_;
			}
		}
		else if((memory->middl+AEM_BAG_PRESS)<lmb_press.middl){
			lumbar.val.mid_fill=(uint8_t)1U;
			lumbar.val.mid_flat=(uint8_t)1U;
			return _MD_;
		}	
	}	
	return _NA_;
}

/* Global variables for debug monitor */
#if(0)
uint16_t mem_plug;
uint16_t counter_apply;
uint16_t run_state;
uint16_t previous_run;
uint16_t operate_on;
uint16_t valves_st;
#endif
/*************************************/

/**
  @brief Apply lumbar memory setting 
 */
uint8_t apply_lumbar(uint8_t mem_set, uint8_t invasion){
	/****/
	static uint8_t memory=(uint8_t)0U;
	static uint8_t inv_act=(uint8_t)0U;
	static uint8_t run=(uint8_t)0U;
	static uint8_t exit=(uint8_t)0U;
	static uint8_t p_run=(uint8_t)0U;
	static uint8_t pumping=(uint8_t)0U;
	static uint8_t pressure=(uint8_t)0U;
	static uint8_t valid=(uint8_t)0U;
	static uint8_t lmb_feed=(uint8_t)0U;
	static int16_t operate=_NA_;
	static int32_t notch=0UL;
#if(0)
	static lmb_press_t lmb_memory={
		(uint8_t)0U,
		(uint8_t)0U,
		(uint8_t)0U,
		(uint8_t)0U,
	};
#endif
	uint16_t buffer;
	/****/
	if(mem_set>MEM_SETINGS_MAX)return run;
    if(drain.open|drain.exit){
    	if(!drain.exit){
			if(mem_set^mem_status.last_m_lmb){
				if(!pneumatic_drain(_OFF_)){
					drain.exit=(uint8_t)1U;
					drain.notch=__get_millis;
				}		
			}
    	}
    	else if(ms_from(drain.notch)>DRAIN_EXIT_TIM){
    		drain.exit=(uint8_t)0U;
    	}
    }
	else if(!run){
		if(mem_set==mem_status.last_m_lmb)return (uint8_t)0U;
		else if(invasion){
			mem_status.last_m_lmb=mem_set;
			mem_status.edited_lmb=(uint8_t)1U;
			return (uint8_t)0U;
		}
		/***/
		/* Debug */
		//mem_plug=mem_set;
		/* ----- */
		memory=mem_set*MEM_LUMBAR_STEP;
		read_16w(memory,&buffer);
		lmb_memory.max_p=(uint8_t)((buffer>>8U)&0xFFU);
		lmb_memory.upper=(uint8_t)(buffer&0xFFU);
		memory+=(uint8_t)2U;
		read_16w(memory,&buffer);
		lmb_memory.middl=(uint8_t)((buffer>>8U)&0xFFU);
		lmb_memory.lower=(uint8_t)(buffer&0xFFU);
		if((lmb_memory.max_p>MAX_BAG_PRESS)||
		   (lmb_memory.upper>MAX_BAG_PRESS)||
		   (lmb_memory.middl>MAX_BAG_PRESS)||
		   (lmb_memory.lower>MAX_BAG_PRESS)){
			mem_status.last_m_lmb=memory/MEM_LUMBAR_STEP;	/* Some Epic Fail occurred */
			return (uint8_t)0U;
		}
		run=(uint8_t)1U;
		/* Debug */
		//counter_apply++; // Debug variable
		/* ----- */
	}
	else if(!(__SPI_STATE->busy)){
		if(invasion|inv_act){
			if(inv_act){
				if(valid){
					if(exit){
						switch(exit){
							case (uint8_t)1U:
								msg_1R.valve.drain=(uint8_t)1U;
								__SPI_STATE->busy=spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
								msg_1R.valve.drain=(uint8_t)0U;
								notch=__get_millis;
								exit++;
							break;
							case (uint8_t)2U:
								if(ms_from(notch)>=ISOLATE_P_SENS){
									if(!lmb_memory.upper)lumbar.val.upp_fill=(uint8_t)1U;
									else lumbar.val.low_fill=(uint8_t)1U;
									exit++;
								}
							break;
							case (uint8_t)3U:
								if(!lmb_memory.upper){
									lmb_press.upper=pressure;
									lumbar.val.upp_fill=(uint8_t)0U;
								}
								else{
									lmb_press.lower=pressure;
									lumbar.val.low_fill=(uint8_t)0U;
								}
								exit++;
							break;
							case (uint8_t)4U:
								__SPI_STATE->busy=spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
								notch=__get_millis;
								exit++;
							break;
							case (uint8_t)5U:
								if(ms_from(notch)>__ONLY_P_SENSE){
									lmb_correct_p();
									mem_status.last_m_lmb=memory/MEM_LUMBAR_STEP;
									mem_status.edited_lmb=(uint8_t)1U;
									run=exit=inv_act=(uint8_t)0U;	
								}		
							break;
						}
					}
					else{
						switch(operate){
							case _UP_: lmb_press.upper=pressure; break;
							case _MD_: lmb_press.middl=pressure; break;
							case _LO_: lmb_press.lower=pressure; break;
						}
						p_run=lumbar.feed_cmd=(uint8_t)0U;
						if(lmb_feed==lumbar.feed_cmd){
							valid=(uint8_t)0U;
							notch=__get_millis;
						}
						exit=(uint8_t)1U;
					}
				}
			}
			else if(run==(uint8_t)1U){
				mem_status.last_m_lmb=memory/MEM_LUMBAR_STEP;
				mem_status.edited_lmb=(uint8_t)1U;
				run=(uint8_t)0U;
			}
			else inv_act=invasion;
			/* Debug */
			invasion=invasion; // Debug point
			/* ----- */
		}
		else{
			switch(run){
				case (uint8_t)1U:
					operate=fill_init_solver(&lmb_memory);
					if(operate^_NA_){
						p_run=(uint8_t)1U;
						run++;
					}
					else{
						if(!lmb_memory.upper){
							operate=_UP_;
							lumbar.val.upp_fill=(uint8_t)1U;
						}
						else{
							operate=_LO_;
							lumbar.val.low_fill=(uint8_t)1U;
						}
						run=(uint8_t)5U;
					}
				break;
				case (uint8_t)2U:
					if(valid){
						switch(operate){
							case _UP_:
								if((lmb_press.upper=pressure)>lmb_memory.upper){
									if(lmb_memory.middl>(lmb_press.middl+AEM_BAG_PRESS)){
										operate=_MD_;
										lumbar.val.upp_fill=(uint8_t)0U;
										lumbar.val.mid_fill=(uint8_t)1U;
										run++;
									}
									else{
										operate=_NA_;
										lumbar.val.upp_fill=(uint8_t)0U;
										p_run=(uint8_t)0U;
										run=(uint8_t)4U;
									}
								}
							break;
							case _MD_:
								if((lmb_press.middl=pressure)>lmb_memory.middl){
									if(lmb_memory.upper>(lmb_press.upper+AEM_BAG_PRESS)){						
										operate=_UP_;
										lumbar.val.mid_fill=(uint8_t)0U;
										lumbar.val.upp_fill=(uint8_t)1U;					
										run++;	
									}
									else if(lmb_memory.lower>(lmb_press.lower+AEM_BAG_PRESS)){
										operate=_LO_;
										lumbar.val.mid_fill=(uint8_t)0U;
										lumbar.val.low_fill=(uint8_t)1U;					
										run++;	
									}
									else{
										operate=_NA_;
										lumbar.val.mid_fill=(uint8_t)0U;
										p_run=(uint8_t)0U;
										run=(uint8_t)4U;
									}
								}
							break;
							case _LO_:
								if((lmb_press.lower=pressure)>lmb_memory.lower){  /* Some small update */
									if(lmb_memory.middl>(lmb_press.middl+AEM_BAG_PRESS)){
										operate=_MD_;
										lumbar.val.low_fill=(uint8_t)0U;
										lumbar.val.mid_fill=(uint8_t)1U;
										run++;
									}
									else{
										operate=_NA_;
										lumbar.val.low_fill=(uint8_t)0U;
										p_run=(uint8_t)0U;
										run=(uint8_t)4U;
									}
								}	
							break;
						}
					}		
				break;
				case (uint8_t)3U:
					if(valid){
						switch(operate){
							case _UP_:
								if((lmb_press.upper=pressure)>lmb_memory.upper){
									operate=_NA_;
									lumbar.val.upp_fill=(uint8_t)0U;
									p_run=(uint8_t)0U;
									run++;
								}	
							break;
							case _MD_:
								if((lmb_press.middl=pressure)>lmb_memory.middl){
									operate=_NA_;
									lumbar.val.mid_fill=(uint8_t)0U;
									p_run=(uint8_t)0U;
									run++;
								}		
							break;
							case _LO_:
								if((lmb_press.lower=pressure)>lmb_memory.lower){
									operate=_NA_;
									lumbar.val.low_fill=(uint8_t)0U;
									p_run=(uint8_t)0U;
									run++;
								}		
							break;
						}
					}					
				break;
				case (uint8_t)4U:
					if(valid){
						if(!lmb_memory.upper){
							operate=_UP_;
							lumbar.val.upp_fill=(uint8_t)1U;
						}
						else{
							operate=_LO_;
							lumbar.val.low_fill=(uint8_t)1U;
						}
						run++;
					}			
				break;
				case (uint8_t)5U:
					if(valid){
						if(operate==_UP_){
							if((lmb_press.upper=pressure)<LOW_PRESSURE){
								operate=_NA_;
								lumbar.val.upp_fill=(uint8_t)0U;
								lumbar.val.upp_flat=(uint8_t)0U;
								lmb_press.upper=(uint8_t)0U;
								run++;
							}			
						}
						else{
							if((lmb_press.lower=pressure)<LOW_PRESSURE){
								operate=_NA_;
								lumbar.val.low_fill=(uint8_t)0U;
								lumbar.val.low_flat=(uint8_t)0U;
								lmb_press.lower=(uint8_t)0U;
								run++;
							}		
						}
					}			
				break;
				case (uint8_t)6U:
					if(valid){
						operate=flat_init_solver(&lmb_memory);
						if(operate^_NA_)run++;
						else run=(uint8_t)9U;
					}		
				break;
				case (uint8_t)7U:
					if(valid){
						switch(operate){
							case _UP_:
								if((lmb_press.upper=pressure)<
								   (lmb_memory.upper+DEFLAT_ADDER(lmb_memory.upper))){
									if((lmb_memory.middl+AEM_BAG_PRESS)<lmb_press.middl){
										lumbar.val.mid_fill=(uint8_t)1U;
										lumbar.val.mid_flat=(uint8_t)1U;
										operate=_MD_;
										run++;
									}
									else{
										operate=_NA_;
										run=(uint8_t)9U;
									}
									lumbar.val.upp_fill=(uint8_t)0U;
									lumbar.val.upp_flat=(uint8_t)0U;
								}
							break;
							case _MD_:
								if((lmb_press.middl=pressure)<
								   (lmb_memory.middl+DEFLAT_ADDER(lmb_memory.middl))){
									if((lmb_memory.upper+AEM_BAG_PRESS)<lmb_press.upper){						
										operate=_UP_;
										lumbar.val.upp_fill=(uint8_t)1U;
										lumbar.val.upp_flat=(uint8_t)1U;					
										run++;	
									}
									else if((lmb_memory.lower+AEM_BAG_PRESS)<lmb_press.lower){
										operate=_LO_;
										lumbar.val.low_fill=(uint8_t)1U;
										lumbar.val.low_flat=(uint8_t)1U;					
										run++;	
									}
									else{
										operate=_NA_;
										run=(uint8_t)9U;
									}
									lumbar.val.mid_flat=(uint8_t)0U;
									lumbar.val.mid_fill=(uint8_t)0U;
								}	
							break;
							case _LO_:
								if((lmb_press.lower=pressure)<
								   (lmb_memory.lower+DEFLAT_ADDER(lmb_memory.lower))){
									if((lmb_memory.middl+AEM_BAG_PRESS)<lmb_press.middl){
										lumbar.val.mid_fill=(uint8_t)1U;
										lumbar.val.mid_flat=(uint8_t)1U;
										operate=_MD_;
										run++;
									}
									else{
										operate=_NA_;
										run=(uint8_t)9U;
									}
									lumbar.val.low_fill=(uint8_t)0U;
									lumbar.val.low_flat=(uint8_t)0U;
								}
							break;	
						}
					}
				break;
				case (uint8_t)8U:
					if(valid){
						switch(operate){
							case _UP_:
								if((lmb_press.upper=pressure)<
								   (lmb_memory.upper+DEFLAT_ADDER(lmb_memory.upper))){
									operate=_NA_;
									lumbar.val.upp_fill=(uint8_t)0U;
									lumbar.val.upp_flat=(uint8_t)0U;
									run++;
								}	
							break;
							case _MD_:
								if((lmb_press.middl=pressure)<
								   (lmb_memory.middl+DEFLAT_ADDER(lmb_memory.middl))){
									operate=_NA_;
									lumbar.val.mid_fill=(uint8_t)0U;
									lumbar.val.mid_flat=(uint8_t)0U;
									run++;
								}		
							break;
							case _LO_:
								if((lmb_press.lower=pressure)<
								   (lmb_memory.lower+DEFLAT_ADDER(lmb_memory.lower))){
									operate=_NA_;
									lumbar.val.low_fill=(uint8_t)0U;
									lumbar.val.low_flat=(uint8_t)0U;
									run++;
								}		
							break;
						}
					}		
				break;
				case (uint8_t)9U:
					if(valid){
						lmb_correct_p();
						lmb_press.max_p=lmb_memory.max_p;
						mem_status.last_m_lmb=memory/MEM_LUMBAR_STEP;
						mem_status.edited_lmb=(uint8_t)0U;
						notch=__get_millis;
						run++;
					}
				break;
				case (uint8_t)10U:
					if(ms_from(notch)>ISOLATE_P_SENS)run=(uint8_t)0U;
				break;
			}
		}
		/***/
		if(lmb_feed^lumbar.feed_cmd){
			lmb_feed=lumbar.feed_cmd;
			__SPI_STATE->busy=spi_tx_word(_V_LMB,SPI_CMD_VALVE|(uint16_t)lmb_feed);
			pressure_hoarder((uint8_t)0U,_ON_);
			valid=(uint8_t)0U;
			notch=__get_millis;
		}
		else if(ms_from(notch)>ISOLATE_PUMP_W){
			if(p_run^pumping){
				if(p_run)pump_force(MAX_DUTY);
				else pump_force(MIN_DUTY);
				pumping=p_run;
			}
			pressure=pressure_hoarder((uint8_t)0U,_OFF_);
			if(ms_from(notch)>(ISOLATE_P_SENS-ISOLATE_PUMP_W))valid=(uint8_t)1U;
		}
	}
    
	/* Debug */
#if(0)
    if(run_state^((uint16_t)run)){
    	previous_run=run_state;
    	run_state=run;
    }
	operate_on=operate;
	valves_st=lmb_feed;
#endif
	/* ----- */
     
	return run; //((run^(uint8_t)1U)?run:(uint8_t)0U);
}

static uint8_t adaptive_hyst(uint8_t silencer){
	int16_t hysteresis;
	hysteresis=((int16_t)lmb_press.max_p)-(((int16_t)PR_HYSTERESIS)/2);
	hysteresis-=(10-((((int16_t)lmb_press.max_p)-((int16_t)FLT_BAG_PRESS))/10));
	hysteresis-=(int16_t)silencer;
	return ((uint8_t)((hysteresis>0)?hysteresis:0));
}

/* Debug */
#if(0)
static uint16_t watch_silence; /* Variables for debug monitor */
static uint16_t watch_shift;
#endif
/*********/

static void adaptive_silence(uint8_t* silence, uint8_t max_press){ // TODO: Replace all numeric values with macro definitions, better LUT
	static uint8_t post_shift=(uint8_t)0U;
	if(*silence)*silence+=post_shift;
	else{
		if(max_press<=(uint8_t)25){
			*silence+=(uint8_t)15U;	/* Correct to be 14 */
			post_shift=(uint8_t)4U;
		}
		else if(max_press<=(uint8_t)30){ //25
			*silence+=(uint8_t)9U;
			post_shift=(uint8_t)3U;
		}
		else if(max_press<=(uint8_t)35){
			*silence+=(uint8_t)5U;
			post_shift=(uint8_t)2U;
		}
		else if(max_press<=(uint8_t)45){
			*silence+=(uint8_t)2U;
			post_shift=(uint8_t)1U;
		}
		else{
			(*silence)++;
			post_shift=(uint8_t)1U;
		}
	}
	if((*silence)>MAX_BAG_PRESS)*silence=MAX_BAG_PRESS;	
	/***/
#if(0)
	watch_silence=*silence;
	watch_shift=post_shift;
#endif
	/***/
	return;
}

/**
 * 
 */
static void lmb_scan_p(int16_t* exclude, uint8_t* measure){ 	/* Some updates */
	lumbar.feed_cmd=(uint8_t)0U;
	if(*measure)(*measure)++;
	switch(*exclude){
		case _UP_:
			switch(*measure){
				case (uint8_t)3U: 
					lumbar.val.mid_fill=(uint8_t)1U;
				break;
				case (uint8_t)4U:
					lmb_press.middl=air_pressure();
				break;
				case (uint8_t)5U:
					lumbar.val.low_fill=(uint8_t)1U;
				break;
				case (uint8_t)6U:
					lmb_press.lower=air_pressure();
				break;
				case (uint8_t)8U:
					*measure=(uint8_t)0U;
				break;
				default: break;	
			}
		break;
		case _MD_:
			switch(*measure){
				case (uint8_t)3U: 
					lumbar.val.upp_fill=(uint8_t)1U;
				break;
				case (uint8_t)4U:
					lmb_press.upper=air_pressure();
				break;
				case (uint8_t)5U:
					lumbar.val.low_fill=(uint8_t)1U;
				break;
				case (uint8_t)6U:
					lmb_press.lower=air_pressure();
				break;
				case (uint8_t)8U:
					*measure=(uint8_t)0U;
				break;
				default: break;	
			}
		break;
		case _LO_:
			switch(*measure){
				case (uint8_t)3U: 
					lumbar.val.mid_fill=(uint8_t)1U;
				break;
				case (uint8_t)4U:
					lmb_press.middl=air_pressure();
				break;
				case (uint8_t)5U:
					lumbar.val.upp_fill=(uint8_t)1U;
				break;
				case (uint8_t)6U:
					lmb_press.upper=air_pressure();
				break;
				case (uint8_t)8U:
					*measure=(uint8_t)0U;
				break;
				default: break;	
			}
		break;
		case _NA_:
			switch(*measure){
				case (uint8_t)3U: 
					lumbar.val.upp_fill=(uint8_t)1U;
				break;
				case (uint8_t)4U:
					lmb_press.upper=air_pressure();
				break;
				case (uint8_t)5U:
					lumbar.val.mid_fill=(uint8_t)1U;
				break;
				case (uint8_t)6U:
					lmb_press.middl=air_pressure();
				break;
				case (uint8_t)7U:
					lumbar.val.low_fill=(uint8_t)1U;
				break;
				case (uint8_t)8U:
					lmb_press.lower=air_pressure();
				break;
				case (uint8_t)10U:
					*measure=(uint8_t)0U;
				break;
				default: break;	
			}
		break;
	}
	return;
}

/* Debug variables */
uint16_t lumb_up_press=0U;
uint16_t lumb_md_press=0U;
uint16_t lumb_dn_press=0U;


/**
 * @Brief Adjust lumbar support depending on buttons received
 */
uint8_t adjust_lumbar /* TODO: Requires earlier planned updates */
	(const uint8_t up,
	 const uint8_t down,
	 const uint8_t increase,
	 const uint8_t decrease){
	/***/
	static uint8_t p_run=(uint8_t)0U;
	static uint8_t p_lock=(uint8_t)0U;
	static uint8_t latch_max=(uint8_t)0U;
	static uint8_t latch_up_lvl=(uint8_t)0U; /* TODO: Move to global structure and discard when memory change */
	static uint8_t latch_dn_lvl=(uint8_t)0U; /* TODO: Move to global structure and discard when memory change */
	static int8_t lockl_pr=(int8_t)0;
	static int8_t delta_pr=(int8_t)0;
	static uint8_t lmb_feed=(uint8_t)0U;
	static uint8_t lmb_inlt=(uint8_t)0U;
	static uint8_t measure=(uint8_t)0U;
	static uint8_t norm_f=(uint8_t)0U;
	static uint8_t slide=(uint8_t)0U;
	static uint8_t silence=(uint8_t)0U;		 /* TODO: Move to global structure and discard when memory change */
	static uint8_t dempfer=(uint8_t)0U;		 /* TODO: Move to global structure and discard when memory change */
	static uint8_t pr_head=(uint8_t)0U; 	 /* TODO: Remove on revision */
	static uint8_t inc_feed=(uint8_t)0U;
	static uint32_t notch=0UL;
	static int16_t operate=_NA_;
	static int16_t exclude=_NA_;
	static int16_t correct=_NA_;
	static int16_t last_bt=_NO_Bt_;
	static int16_t corr_bt=_NO_Bt_;
	/***/
	uint8_t set_max=(uint8_t)0U;
	/***/
	uint8_t in_up=(uint8_t)0U;
	uint8_t in_down=(uint8_t)0U;
    uint8_t in_increase=(uint8_t)0U;
    uint8_t in_decrease=(uint8_t)0U;
	/***/
    uint8_t inlets;
    /***/
	if(!measure){
		lumbar.feed_cmd=(uint8_t)0U;
		in_up=up;
		in_down=down;
	    in_increase=increase;
	    in_decrease=decrease;
	}
	/***/
    if(drain.open|drain.exit){
    	if(!drain.exit){
			if(in_up|in_down|in_increase|in_decrease){
				if(!pneumatic_drain(_OFF_)){
					drain.exit=(uint8_t)1U;
					drain.notch=__get_millis;
				}		
			}
    	}
    	else if(ms_from(drain.notch)>DRAIN_EXIT_TIM){
    		drain.exit=(uint8_t)0U;
    	}
    }
    else if(!(__SPI_STATE->busy)){
		if(((up|down|increase|decrease)&((uint8_t)0xFEU))||
		   ((up+down+increase+decrease)>(uint8_t)1U)){
			p_lock=(uint8_t)0U;	
			operate=_NA_;	
			if(last_bt^_NO_Bt_){
				corr_bt=last_bt;
				last_bt=_NO_Bt_;
				norm_f=measure=(uint8_t)1U;
				/*
				if((delta_pr/=(int8_t)2)){
					if(lmb_press.max_p>(uint8_t)delta_pr){
						lmb_press.max_p-=(uint8_t)delta_pr;
						delta_pr=(int8_t)0;
					}
				}
				*/	
			}
		}
		else if(in_up){
			if(last_bt^_UP_Bt_){
				mem_status.edited_lmb=(uint8_t)1U;
				latch_dn_lvl=(uint8_t)0U;
				correct=exclude=_NA_;
				if((corr_bt==_DN_Bt_)||(last_bt==_DN_Bt_)){
					silence=(uint8_t)0U;
				}
				last_bt=_UP_Bt_;		
			}
			/***/
			p_lock=(uint8_t)0U;
			/***/
			if(lmb_press.max_p>FLT_BAG_PRESS){
				if(lmb_press.upper>AEM_BAG_PRESS){ 
					operate=_UP_;
					lumbar.val.upp_fill=(uint8_t)1U;
					lumbar.val.low_flat=(uint8_t)1U;
					if((lmb_press.upper>
						DEFLAT_P_TH(lmb_press.max_p))||silence){ //lmb_press.middl
						lumbar.val.mid_flat=(uint8_t)1U;
					}
					if(latch_up_lvl){
						p_lock=(uint8_t)1U;	
						if(lmb_press.upper<adaptive_hyst(silence)){	
							latch_up_lvl=p_lock=(uint8_t)0U;
						}	
					}
					else if(lmb_press.upper>lmb_press.max_p){
						latch_up_lvl=p_lock=(uint8_t)1U;
						adaptive_silence(&silence,lmb_press.max_p);
					}
				}
				else{
					if(lmb_press.middl<(lmb_press.max_p+((uint8_t)1U))){
						operate=_MD_;
						lumbar.val.mid_fill=(uint8_t)1U;
						if(lmb_press.middl>
							DEFLAT_P_TH(lmb_press.max_p)){ //lmb_press.lowe
							lumbar.val.low_flat=(uint8_t)1U;
						}	
					}
					else{
						lmb_press.upper=AEM_BAG_PRESS;
						lmb_press.upper++;
						operate=_UP_;
						lumbar.val.upp_fill=(uint8_t)1U;
					}
				}
			}	
			/***/				
		}
		else if(in_down){
			if(last_bt^_DN_Bt_){
				mem_status.edited_lmb=(uint8_t)1U;
				latch_up_lvl=(uint8_t)0U;
				correct=exclude=_NA_;
				if((corr_bt==_UP_Bt_)||(last_bt==_UP_Bt_)){
					silence=(uint8_t)0U;
				}
				last_bt=_DN_Bt_;		
			}
			/***/
			p_lock=(uint8_t)0U;
			/***/
			if(lmb_press.max_p>FLT_BAG_PRESS){
				if(lmb_press.lower>AEM_BAG_PRESS){
					operate=_LO_;
					lumbar.val.low_fill=(uint8_t)1U;
					lumbar.val.upp_flat=(uint8_t)1U;
					if((lmb_press.lower>
						DEFLAT_P_TH(lmb_press.max_p))||silence){
						lumbar.val.mid_flat=(uint8_t)1U;
					}
					if(latch_dn_lvl){
						p_lock=(uint8_t)1U;
						if(lmb_press.lower<adaptive_hyst(silence)){
							latch_dn_lvl=p_lock=(uint8_t)0U;
						}	
					}
					else if(lmb_press.lower>lmb_press.max_p){
						latch_dn_lvl=p_lock=(uint8_t)1U;
						adaptive_silence(&silence,lmb_press.max_p);
					}
				}
				else{
					if(lmb_press.middl<(lmb_press.max_p+((uint8_t)1U))){
						operate=_MD_;
						lumbar.val.mid_fill=(uint8_t)1U;
						if(lmb_press.middl>
							DEFLAT_P_TH(lmb_press.max_p)){
							lumbar.val.upp_flat=(uint8_t)1U;
						}	
					}
					else{
						lmb_press.lower=AEM_BAG_PRESS;
						lmb_press.lower++;
						operate=_LO_;
						lumbar.val.low_fill=(uint8_t)1U;
					}
				}
			}	
			/***/		
		}
		else if(in_increase){
			if(last_bt^_IN_Bt_){
				mem_status.edited_lmb=(uint8_t)1U;
				latch_dn_lvl=latch_up_lvl=(uint8_t)0U;
				last_bt=_IN_Bt_;
				correct=exclude=_NA_;
				silence=(uint8_t)0U;
				/***/
				if((lmb_press.upper<=AEM_BAG_PRESS)&&
				   (lmb_press.lower<=AEM_BAG_PRESS)){
					operate=_MD_;
					lumbar.val.mid_fill=(uint8_t)1U;	
					lumbar.val.upp_flat=(uint8_t)1U;
					lumbar.val.low_flat=(uint8_t)1U;				
				}
				else if(lmb_press.upper>lmb_press.middl){
					operate=_UP_;
					lumbar.val.upp_fill=(uint8_t)1U;	
					lumbar.val.mid_flat=(uint8_t)1U;
					lumbar.val.low_flat=(uint8_t)1U;
				}
				else if(lmb_press.lower>lmb_press.middl){
					operate=_LO_;
					lumbar.val.low_fill=(uint8_t)1U;	
					lumbar.val.mid_flat=(uint8_t)1U;
					lumbar.val.upp_flat=(uint8_t)1U;
				}
				else{
					operate=_MD_;
					lumbar.val.mid_fill=(uint8_t)1U;	
					lumbar.val.upp_flat=(uint8_t)1U;
					lumbar.val.low_flat=(uint8_t)1U;			
				}
				inc_feed=lumbar.feed_cmd;
			}
			else lumbar.feed_cmd=inc_feed;
			/***/	
			p_lock=(uint8_t)0U;
			set_max=(uint8_t)2U;
			/***/
			switch(operate){
				case _UP_:
					if(latch_max){
						p_lock=(uint8_t)1U;
						if(lmb_press.upper<
						  ((MAX_BAG_PRESS-PR_HYSTERESIS)-dempfer)){
							latch_max=p_lock=(uint8_t)0U;
						}
					}
					else if(lmb_press.upper>MAX_BAG_PRESS){
						latch_max=p_lock=(uint8_t)1U;
						if(++dempfer>FLT_BAG_PRESS)dempfer=FLT_BAG_PRESS;
					}	
				break;
				case _MD_:
					if(latch_max){
						p_lock=(uint8_t)1U;
						if(lmb_press.middl<
						  ((MAX_BAG_PRESS-PR_HYSTERESIS)-dempfer)){
							latch_max=p_lock=(uint8_t)0U;
						}
					}
					else if(lmb_press.middl>MAX_BAG_PRESS){
						latch_max=p_lock=(uint8_t)1U;
						if(++dempfer>FLT_BAG_PRESS)dempfer=FLT_BAG_PRESS;
					}			
				break;
				case _LO_:
					if(latch_max){
						p_lock=(uint8_t)1U;
						if(lmb_press.lower<
						  ((MAX_BAG_PRESS-PR_HYSTERESIS)-dempfer)){
							latch_max=p_lock=(uint8_t)0U;
						}
					}
					else if(lmb_press.lower>MAX_BAG_PRESS){
						latch_max=p_lock=(uint8_t)1U;
						if(++dempfer>FLT_BAG_PRESS)dempfer=FLT_BAG_PRESS;
					}
				break;
			}	
		}
		else if(in_decrease){
			if(last_bt^_DE_Bt_){
				if(lmb_press.max_p){
					if(!silence){
						adaptive_silence(&silence,lmb_press.max_p); /* TODO: Update to LUT array - actually, remove this block */ 
					}
					pr_head=lmb_press.max_p-adaptive_hyst(silence);
				}
				else pr_head=(uint8_t)0U;
		        /***/
				mem_status.edited_lmb=(uint8_t)1U;
				latch_dn_lvl=latch_up_lvl=
							 latch_max=(uint8_t)0U;
				last_bt=_DE_Bt_;
				correct=exclude=_NA_;
				dempfer=silence=(uint8_t)0U;
			}
			/***/
			p_lock=(uint8_t)1U;
			/***/
			if((lmb_press.upper<=AEM_BAG_PRESS)&&
			   (lmb_press.lower<=AEM_BAG_PRESS)){
					set_max=(uint8_t)1U;
					operate=_MD_;
					lumbar.val.mid_fill=(uint8_t)1U;
					lumbar.val.mid_flat=(uint8_t)1U;
					/***/
					lumbar.val.upp_flat=(uint8_t)1U;
					lumbar.val.low_flat=(uint8_t)1U;
			}
			else if(lmb_press.upper<=AEM_BAG_PRESS){
				if((lmb_press.lower>=lmb_press.middl)&&
				   (lmb_press.middl>AEM_BAG_PRESS)){
					operate=_MD_;
					lumbar.val.mid_fill=(uint8_t)1U;
					lumbar.val.mid_flat=(uint8_t)1U;
				}
				else{
					if(lmb_press.middl<=AEM_BAG_PRESS){
						set_max=(uint8_t)1U;
						lumbar.val.mid_flat=(uint8_t)1U;
					}
					operate=_LO_;
					lumbar.val.low_fill=(uint8_t)1U;
					lumbar.val.low_flat=(uint8_t)1U;
				}
				lumbar.val.upp_flat=(uint8_t)1U;
			}
			else if(lmb_press.lower<=AEM_BAG_PRESS){
				if((lmb_press.upper>=lmb_press.middl)&&
				   (lmb_press.middl>AEM_BAG_PRESS)){
					operate=_MD_;
					lumbar.val.mid_fill=(uint8_t)1U;
					lumbar.val.mid_flat=(uint8_t)1U;						
				}
				else{
					if(lmb_press.middl<=AEM_BAG_PRESS){
						set_max=(uint8_t)1U;
						lumbar.val.mid_flat=(uint8_t)1U;
					}
					operate=_UP_;
					lumbar.val.upp_fill=(uint8_t)1U;
					lumbar.val.upp_flat=(uint8_t)1U;
				}
				lumbar.val.low_flat=(uint8_t)1U;
			}
			/***/
			/*
			if(!set_max){
				if(correct^operate){
					switch(operate){
						case _UP_: lockl_pr=(int8_t)lmb_press.upper; break;
						case _MD_: lockl_pr=(int8_t)lmb_press.middl; break;
						case _LO_: lockl_pr=(int8_t)lmb_press.lower; break;
					}
					correct=operate;
				}
				else if(exclude==correct){
					switch(correct){
						case _UP_: delta_pr=lockl_pr-(int8_t)lmb_press.upper; break;
						case _MD_: delta_pr=lockl_pr-(int8_t)lmb_press.middl; break;
						case _LO_: delta_pr=lockl_pr-(int8_t)lmb_press.lower; break;				
					}
					if(delta_pr<(int8_t)0)delta_pr=(int8_t)0;
				}
			}
			else{
				delta_pr=(int8_t)0;
				correct=_NA_;
			}
			*/
		}
		else{
			p_lock=(uint8_t)0U;	
			operate=_NA_;	
			if(last_bt^_NO_Bt_){
				corr_bt=last_bt;
				last_bt=_NO_Bt_;
				norm_f=measure=(uint8_t)1U;
				/*
				if((delta_pr/=(int8_t)2)){
					if(lmb_press.max_p>(uint8_t)delta_pr){
						lmb_press.max_p-=(uint8_t)delta_pr;
						delta_pr=(int8_t)0;
					}
				}
				*/	
			}
		}	
		if((p_run)&&(p_lock)){
			pump_force(MIN_DUTY);
			p_run=(uint8_t)0U;
		}
		if(lmb_feed^lumbar.feed_cmd){
			/*
			inlets=(lmb_feed=lumbar.feed_cmd)&LMB_INLETS__;
			*/
			lmb_feed=lumbar.feed_cmd;

			{
				 inlets=(uint8_t)lumbar.val.upp_fill;
				inlets<<=(uint8_t)1U;
				inlets+=(uint8_t)lumbar.val.mid_fill;
				inlets<<=(uint8_t)1U;
				inlets+=(uint8_t)lumbar.val.low_fill;				
			};
			if(lmb_inlt^inlets){
				pressure_hoarder(lmb_press.max_p,_TRIG_);
				notch=__get_millis;
				lmb_inlt=inlets;
			}
			//notch=__get_millis;
			__SPI_STATE->busy=spi_tx_word(_V_LMB,SPI_CMD_VALVE|(uint16_t)lmb_feed);
		}
		else if(ms_from(notch)>ISOLATE_PUMP_W){
			 switch(operate){
			 	 case _NA_:
					if(p_run){
						pump_force(MIN_DUTY);
						p_run=(uint8_t)0U;
					}
					if(measure==(uint8_t)1U){
						msg_1R.valve.drain=(uint8_t)1U;
						__SPI_STATE->busy=spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
						msg_1R.valve.drain=(uint8_t)0U;
						measure++;
					}
					else if(exclude){
						if(measure==(uint8_t)7U){
							__SPI_STATE->busy=spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
							measure++;
						}
					}
					else if(measure==(uint8_t)9U){
						__SPI_STATE->busy=spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
						measure++;	
					}
					//slide=pressure_hoarder(((uint8_t)0U),_RUN_); // Update 06.06.2023
			 	 break;
				 case _UP_:
				 case _MD_:
				 case _LO_:
				 if((!p_lock)&&(!p_run)){
					 pump_force(MAX_DUTY);
					 p_run=(uint8_t)1U;
				 }
				 slide=pressure_hoarder(lmb_press.max_p,_RUN_);
				 break;
				 default: break;		 
			 }	
			 if(ms_from(notch)>(ISOLATE_P_SENS-
			   ((measure>(uint8_t)2U)?ISOLATE_PUMP_W:0UL))){ // Some update 07.06.2023
			    switch(operate){
					case _UP_:
						lmb_press.upper=slide;
						if(set_max==(uint8_t)2U)lmb_press.max_p=lmb_press.upper;
						else if(set_max){
							if(lmb_press.upper){
								lmb_press.max_p=lmb_press.upper+pr_head; /* TODO: update to LUT array x3 next down*/
							}
							else lmb_press.max_p=(uint8_t)0U;
						}
						exclude=_UP_;
					break;
					case _MD_: 
						lmb_press.middl=slide;
						if(set_max==(uint8_t)2U)lmb_press.max_p=lmb_press.middl;
						else if(set_max){
							if(lmb_press.middl){
								lmb_press.max_p=lmb_press.middl+pr_head;
							}
							else lmb_press.max_p=(uint8_t)0U;
						}
						exclude=_MD_;
					break;
					case _LO_:
						lmb_press.lower=slide;
						if(set_max==(uint8_t)2U)lmb_press.max_p=lmb_press.lower;
						else if(set_max){
							if(lmb_press.lower){
								lmb_press.max_p=lmb_press.lower+pr_head;
							}
							else lmb_press.max_p=(uint8_t)0U;
						}
						exclude=_LO_;
					break;
					case _TS_: 
						/* 
						 * (^.^)___/ 
						 *     /   \
						 */ 
					break;
					default:
						lmb_scan_p(&exclude,&measure);
						if((!measure)&&(norm_f)){
							norm_f=(uint8_t)0U;
							lmb_correct_p();				
						}
						// TODO: Silencer drop after some timeout
					break; 	
				}
			}
		}
	}
	
	#if(0) /* Feed uint16_t for debug monitor only */
		lumb_up_press=lmb_press.upper;
		lumb_md_press=lmb_press.middl;
		lumb_dn_press=lmb_press.lower;
		//lmb_press_max=lmb_press.max_p;
	#endif
	
	
	return (uint8_t)(!(!(lmb_feed|p_run|measure))); /* TODO: p_run seems useless extra compute load, remove booleans - extra load */
}

/**
  @}
*/
