/*
 * support.c
 *
 *  Created on: Feb 13, 2023
 *      Author: Nick
 */

#define ATTEMPTS_BAL (3) /* Permissible attempts count to balance bags */
#define ACCELERATION_DEFLATE_TIMEOUT (10000UL)

#include "support.h"

sides_pr_t sides_pr;
extern bool side_mem_lin_enable;

static struct{
	uint8_t b_latch;
	uint8_t b_dempfer;
	uint8_t q_latch;
	uint8_t q_dempfer;
}limit;

enum{
	_NO_O_,
	_BACK_,
	_CUSH_
};

enum{
	_NO_ACT_,
	_BR_INF_,
	_BR_DEF_,
	_CS_INF_,
	_CS_DEF_
};

/* Global variables for debug monitor */
#if(0)
uint16_t mem_plug_2;
uint16_t counter_apply_2;
uint16_t run_state_2;
uint16_t previous_run_2;
uint16_t operate_on_2;
uint16_t backrest_lp;
uint16_t backrest_rp;
uint16_t cushion_lp;
uint16_t cushion_rp;
uint16_t press_mon;
uint16_t moment_pr;
#endif
/*************************************/

/**
 * @Brief Balance backrest or cushion bags if a significant mismatch found
 */
static void balance_ssb(int16_t target, uint8_t duration){
	switch(target){
		case _BACK_:
			msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)1U;
			spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
			for(;__SPI_STATE->busy;){};
			ms_halt((uint32_t)duration*BASE_PNUMO_TIM);
			msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)0U;
			spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
			for(;__SPI_STATE->busy;){};
			ms_halt(ISOLATE_P_SENS);
			
		break;
		case _CUSH_:
			msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
			spi_tx_word(_L_SIDE,SPI_CMD_VALVE|(uint16_t)msg_2L.feed_cmd);
			for(;__SPI_STATE->busy;){};
			ms_halt((uint32_t)duration*BASE_PNUMO_TIM);
			msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)0U;
			spi_tx_word(_L_SIDE,SPI_CMD_VALVE|(uint16_t)msg_2L.feed_cmd);
			for(;__SPI_STATE->busy;){};
			ms_halt(ISOLATE_P_SENS);
		break;
		default: break;	
	}
	return;
}

/**
 * @Brief Get initial air pressure in side support bags after power on
 */
void read_side_s(void){
	/****/
	int16_t i;
	struct{
		uint8_t back_l;
		uint8_t back_r;
		uint8_t cush_l;
		uint8_t cush_r;
	}pressure;
	/****/
	/* Backrest check and balance */ 
	i=0;
	for(;0x01U;){
		if(++i>ATTEMPTS_BAL)break;
		/* Get backrest left bag pressure */
		msg_1R.valve.backrest_l=(uint8_t)1U;
		spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
		for(;__SPI_STATE->busy;){};
		ms_halt(ISOLATE_P_SENS);
		pressure.back_l=air_pressure();
		msg_1R.feed_cmd=(uint8_t)0U;
		spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
		for(;__SPI_STATE->busy;){};
		ms_halt(ISOLATE_P_SENS);
		/* Get backrest right bag pressure */
		msg_1R.valve.backrest_r=(uint8_t)1U;
		spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
		for(;__SPI_STATE->busy;){};
		ms_halt(ISOLATE_P_SENS);
		pressure.back_r=air_pressure();
		msg_1R.feed_cmd=(uint8_t)0U;
		spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
		for(;__SPI_STATE->busy;){};
		ms_halt(ISOLATE_P_SENS);
		/* Compare backrest pressures, balance bags if significant error */
		if(pressure.back_l>pressure.back_r){
			if(pressure.back_l-pressure.back_r>AEM_BAG_PRESS){
				balance_ssb(_BACK_,(pressure.back_l-pressure.back_r));
				continue;
			}
			else break;
		}
		else if(pressure.back_r-pressure.back_l>AEM_BAG_PRESS){
			balance_ssb(_BACK_,(pressure.back_r-pressure.back_l));
			continue;
		}
		else break;
		/****/
	}
	sides_pr.backrest=(pressure.back_l+pressure.back_r)/(uint8_t)2U;
	/* Cushion check and balance */
	i=0;
	for(;0x01U;){
		if(++i>ATTEMPTS_BAL)break;
		/* Get cushion left bag pressure */
		msg_2L.valve.cushion_l=(uint8_t)1U;
		spi_tx_word(_L_SIDE,SPI_CMD_VALVE|(uint16_t)msg_2L.feed_cmd);
		for(;__SPI_STATE->busy;){};
		ms_halt(ISOLATE_P_SENS);
		pressure.cush_l=air_pressure();
		msg_2L.feed_cmd=(uint8_t)0U;
		spi_tx_word(_L_SIDE,SPI_CMD_VALVE|(uint16_t)msg_2L.feed_cmd);
		for(;__SPI_STATE->busy;){};
		ms_halt(ISOLATE_P_SENS);
		/* Get cushion right bag pressure */
		msg_2L.valve.cushion_r=(uint8_t)1U;
		spi_tx_word(_L_SIDE,SPI_CMD_VALVE|(uint16_t)msg_2L.feed_cmd);
		for(;__SPI_STATE->busy;){};
		ms_halt(ISOLATE_P_SENS);
		pressure.cush_r=air_pressure();
		msg_2L.feed_cmd=(uint8_t)0U;
		spi_tx_word(_L_SIDE,SPI_CMD_VALVE|(uint16_t)msg_2L.feed_cmd);
		for(;__SPI_STATE->busy;){};
		ms_halt(ISOLATE_P_SENS);
		/* Compare backrest pressures, balance bags if significant error */
		if(pressure.cush_l>pressure.cush_r){
			if(pressure.cush_l-pressure.cush_r>AEM_BAG_PRESS){
				balance_ssb(_CUSH_,(pressure.cush_l-pressure.cush_r));
				continue;
			}
			else break;
		}
		else if(pressure.cush_r-pressure.cush_l>AEM_BAG_PRESS){
			balance_ssb(_CUSH_,(pressure.cush_r-pressure.cush_l));
			continue;
		}
		else break;
		/****/
	}
	sides_pr.cushion=(pressure.cush_l+pressure.cush_r)/(uint8_t)2U;
	/* Deflat system */
	msg_1R.valve.drain=(uint8_t)1U;
	spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
	for(;__SPI_STATE->busy;){};
	ms_halt(ISOLATE_P_SENS);
	msg_1R.feed_cmd=(uint8_t)0U;
	spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
	for(;__SPI_STATE->busy;){};
	ms_halt(ISOLATE_P_SENS);
	
	
#if(0) /* Feed uint16_t for debug monitor only */
	backrest_lp=pressure.back_l;
	backrest_rp=pressure.back_r;
	cushion_lp=pressure.cush_l;
	cushion_rp=pressure.cush_r;
#endif
	
	return;
}

/**
 * @Brief Save current Side Support position
 */
void save_side_s(uint8_t memory){
	uint16_t buffer;
	/****/
#if(0)
	if((memory>MEM_SETINGS_MAX)||
	   (!mem_status.edited_sds))return;
#else
	if((!memory)||(memory>MEM_SETINGS_MAX))return;
#endif
	memory=MEM_SIDE_S_BASE+(memory-(uint8_t)1U)*MEM_SIDE_S_STEP;
	buffer=((uint16_t)sides_pr.backrest)<<8U;
	buffer|=(uint16_t)sides_pr.cushion;
	write_16w(memory,buffer);
#if(1)
	memory=((memory-MEM_SIDE_S_BASE)/MEM_SIDE_S_STEP)+(uint8_t)1U;
	if(memory==mem_status.last_m_sds)mem_status.edited_sds=(uint8_t)0U;
#else
	mem_status.edited_sds=(uint8_t)0U;
#endif
	return;
}

static void drop_limits(void){
	limit.b_dempfer=limit.q_dempfer=(uint8_t)0U;
	limit.b_latch=limit.q_latch=(uint8_t)0U;
	return;
}


#define ENHANCED_PUMPING (1)

uint8_t apply_side_support(uint8_t mem_set, uint8_t invasion){
	/****/
	static uint8_t memory=(uint8_t)0U;
	static uint8_t inv_act=(uint8_t)0U;
	static uint8_t run=(uint8_t)0U;		   
	static uint8_t exit=(uint8_t)0U;
	static uint8_t p_run=(uint8_t)0U;
	static uint8_t pumping=(uint8_t)0U;
	static uint8_t pressure=(uint8_t)0U;
	static uint8_t valid=(uint8_t)0U;
	static uint8_t con_burst=(uint8_t)0U;
	static uint8_t feed_cmd_r=(uint8_t)0U;
	static uint8_t feed_cmd_l=(uint8_t)0U;
	static uint8_t modified=(uint8_t)0U;
	static int16_t operate=_NO_O_;
	static int32_t notch=0UL;
	static sides_pr_t sides_mem={
		(uint8_t)0U,
		(uint8_t)0U,
		(uint8_t)0U,
	};
#if(LEGACY_OFF)
	static uint8_t from_edit=(uint8_t)0U;
	static uint8_t from_memp=(uint8_t)0U;
	static uint8_t trip_zero=(uint8_t)0U;
#endif
	uint8_t flip_order=(uint8_t)0U;
	uint16_t buffer;
	/****/
	
	if (!side_mem_lin_enable)
				return 0;
	
	
	if(mem_set>MEM_SETINGS_MAX)return run;
	if(drain.open|drain.exit){
		if(!drain.exit){
			if(mem_set^mem_status.last_m_sds){
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
		if(mem_set==mem_status.last_m_sds)return (uint8_t)0U;
		else if(invasion){
			mem_status.last_m_sds=mem_set;
			mem_status.edited_sds=(uint8_t)1U;
			return (uint8_t)0U;
		}
		/* Debug */
		//mem_plug_2=mem_set;
		/* ----- */
		if(mem_set){
#if(LEGACY_OFF)
			read_16w(MEM_SIDE_S_SERV,&buffer);
			from_edit=(uint8_t)((buffer>>8U)&0xFFU);
			from_memp=(uint8_t)(buffer&0xFFU);
			if((from_edit==((uint8_t)1U))&&(mem_set==from_memp)){
				memory=MEM_SIDE_S_BASE+(mem_set-(uint8_t)1U)*MEM_SIDE_S_STEP;
				read_16w(MEM_SIDE_S_SERV+((uint8_t)2U),&buffer);
				sides_mem.backrest=(uint8_t)((buffer>>8U)&0xFFU);
				sides_mem.cushion=(uint8_t)(buffer&0xFFU);
			}
			else{
				if(from_edit==((uint8_t)1U)){
					write_16w(MEM_SIDE_S_SERV,0U);
					from_edit=(uint8_t)0U;
				}
#endif				
				memory=MEM_SIDE_S_BASE+(mem_set-(uint8_t)1U)*MEM_SIDE_S_STEP;
				read_16w(memory,&buffer);
				sides_mem.backrest=(uint8_t)((buffer>>8U)&0xFFU);
				sides_mem.cushion=(uint8_t)(buffer&0xFFU);
				if((sides_mem.backrest>MAX_BAG_PRESS)||
				   (sides_mem.cushion>MAX_BAG_PRESS)){
					mem_status.last_m_sds=(memory-MEM_SIDE_S_BASE)/MEM_SIDE_S_STEP+(uint8_t)1U;	/* Some Epic Fail occurred */
					drop_limits();
					return (uint8_t)0U;
				}
#if(LEGACY_OFF)
			}			
			if(from_edit==((uint8_t)1U)){
				write_16w(MEM_SIDE_S_SERV,0U);
			}
			if(!mem_status.last_m_sds){
				trip_zero=(uint8_t)1U;
				#if(ENHANCED_PUMPING) /* Enhanced pumping for zero trip mode 02.06.2023*/
				sides_mem.backrest+=AEM_BAG_PRESS;
				if(sides_mem.backrest>MAX_BAG_PRESS){
				   sides_mem.backrest=MAX_BAG_PRESS;
				}
				sides_mem.cushion+=AEM_BAG_PRESS;
				if(sides_mem.cushion>MAX_BAG_PRESS){
				   sides_mem.cushion=MAX_BAG_PRESS;
				}
				#endif
			}
#endif
		}
		else{
#if(LEGACY_OFF)
			memory=(uint8_t)0U;
			trip_zero=(uint8_t)1U;
#endif
			sides_mem.backrest=sides_mem.cushion=(uint8_t)0U;
#if(LEGACY_OFF)
			if(mem_status.edited_sds){
				buffer=0x0100U|((uint16_t)mem_status.last_m_sds);
				write_16w(MEM_SIDE_S_SERV,buffer);
				buffer=((uint16_t)sides_pr.backrest)<<8U;
				buffer|=(uint16_t)sides_pr.cushion;
				write_16w(MEM_SIDE_S_SERV+((uint8_t)2U),buffer);
			}
#endif
		}
		run=(uint8_t)1U;
		/* Debug */
		//counter_apply_2++; // Debug variable
		/* ----- */
	}
	else if(!(__SPI_STATE->busy)){
    	if(!con_burst){
    		con_burst=(uint8_t)1U;
			if((invasion||inv_act)&&(!trip_zero)){ /* TODO: Additionally to LEGACY_OFF added fuse to avoid user influence on transient from valid to 0 the same as from 0 to valid memory */
				if(inv_act){
					if(valid){
						if(exit){
							switch(exit){
								case (uint8_t)1U:
									msg_1R.valve.drain=(uint8_t)1U;
									spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
									msg_1R.valve.drain=(uint8_t)0U;
									notch=__get_millis;
									exit++;
								break;
								case (uint8_t)2U:
									if(ms_from(notch)>=(__ONLY_P_SENSE*2UL)){
										spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)msg_1R.feed_cmd);
										notch=__get_millis;
										exit++;
									}
								break;
								case (uint8_t)3U:
									if(ms_from(notch)>__ONLY_P_SENSE){
										mem_status.last_m_sds=(memory-MEM_SIDE_S_BASE)/MEM_SIDE_S_STEP+(uint8_t)1U;
										mem_status.edited_sds=(uint8_t)1U;
										run=exit=inv_act=(uint8_t)0U;
										drop_limits();
									}		
								break;
							}
						}
						else{
							switch(operate){
								case _BACK_:
									if(run>(uint8_t)5U)sides_pr.backrest=__get_drop_pressure(pressure); /* TODO: Make the same thing in lumbar module */
									else sides_pr.backrest=pressure;
								break;
								case _CUSH_:
									if(run>(uint8_t)5U)sides_pr.cushion=__get_drop_pressure(pressure);
									else sides_pr.cushion=pressure;
								break;
								default: break;
							}
							p_run=msg_2L.feed_cmd=msg_1R.feed_cmd=(uint8_t)0U;
							if((feed_cmd_l==msg_2L.feed_cmd)&&(feed_cmd_r==msg_1R.feed_cmd)){
								valid=(uint8_t)0U;
								notch=__get_millis;
							}
							exit=(uint8_t)1U;
						}
					}
				}
				else if(run==(uint8_t)1U){
					mem_status.last_m_sds=(memory-MEM_SIDE_S_BASE)/MEM_SIDE_S_STEP+(uint8_t)1U;
					mem_status.edited_sds=(uint8_t)1U;
					run=(uint8_t)0U;
					drop_limits();
				}
				else inv_act=invasion;
				/* Debug */
				//invasion=invasion; // Debug point
				/* ----- */
			}
			else{	
				switch(run){
					case (uint8_t)1U:
						p_run=(uint8_t)1U;
						run++;	
						if((sides_pr.backrest<=sides_mem.backrest)&&
						   (sides_pr.cushion<=sides_mem.cushion)){
							if(trip_zero){
								operate=_CUSH_;
								msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
							}
							else if(sides_pr.backrest>=sides_pr.cushion){ // if(sides_mem.backrest>=sides_mem.cushion){
								operate=_CUSH_;
								msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
							}
							else{
								operate=_BACK_;
								msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)1U;
							}
						}
						else if(sides_pr.backrest<=sides_mem.backrest){
							operate=_BACK_;
							msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)1U;
						}
						else if (sides_pr.cushion<=sides_mem.cushion){
							operate=_CUSH_;
							msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
						}
						else{
							msg_1R.valve.drain=(uint8_t)1U;
							p_run=(uint8_t)0U; 
							run=(uint8_t)5U;    /* TODO: This is inherited value, check on next steps !!!!! */
						}
					break;
					case (uint8_t)2U:	
						if(valid){
							switch(operate){
								case _BACK_:
									if((sides_pr.backrest=pressure)>
									   (sides_mem.backrest+AEM_BAG_PRESS)){
										if(sides_mem.cushion>(sides_pr.cushion+AEM_BAG_PRESS)){										
											operate=_CUSH_;
											msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
											run++;
										}
										else{
											operate=_NO_O_;
											p_run=(uint8_t)0U;
											run=(uint8_t)4U;	/* TODO: This is inherited value, check on next steps !!!!! */
										}
										msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)0U;
#if(ENHANCED_PUMPING)
										sides_pr.backrest=sides_mem.backrest;
#endif	
									}
								break;
								case _CUSH_:
									if((sides_pr.cushion=pressure)>
									   (sides_mem.cushion+AEM_BAG_PRESS)){	
										if(sides_mem.backrest>(sides_pr.backrest+AEM_BAG_PRESS)){			
											flip_order=(uint8_t)1U;
											operate=_BACK_;
											msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)1U;
											run++;
										}
										else{
											operate=_NO_O_;
											p_run=(uint8_t)0U;
											run=(uint8_t)4U;	/* TODO: This is inherited value, check on next steps !!!!! */
										}
										msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)0U;
#if(ENHANCED_PUMPING)
										sides_pr.cushion=sides_mem.cushion;
#endif	
									}
								break;
							}
						}	
					break;
					case (uint8_t)3U:
						if(valid){
							switch(operate){
								case _BACK_:
									if((sides_pr.backrest=pressure)>
									   (sides_mem.backrest+AEM_BAG_PRESS)){
#if(ENHANCED_PUMPING)
										sides_pr.backrest=sides_mem.backrest;
#endif	
										operate=_NO_O_;	
										msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)0U;
										p_run=(uint8_t)0U;
										run++;
									}	
								break;
								case _CUSH_:
									if((sides_pr.cushion=pressure)>
									   (sides_mem.cushion+AEM_BAG_PRESS)){
#if(ENHANCED_PUMPING)
										sides_pr.cushion=sides_mem.cushion;
#endif
										operate=_NO_O_;	
										msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)0U;
										p_run=(uint8_t)0U;
										run++;
									}		
								break;
							}
						}					
					break;
					case (uint8_t)4U:
						if(valid){
							msg_1R.valve.drain=(uint8_t)1U;
							run++;
						}			
					break;
					case (uint8_t)5U:
						if(valid){	
							run++;
							if(((sides_mem.backrest+AEM_BAG_PRESS)<sides_pr.backrest)&&
								((sides_mem.cushion+AEM_BAG_PRESS)<sides_pr.cushion)){
								if(!(sides_mem.backrest||sides_mem.cushion)){
									operate=_CUSH_;
									msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
								}
								else if(sides_mem.backrest>=sides_mem.cushion){
									operate=_BACK_;
									msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)1U;
								}
								else{
									operate=_CUSH_;
									msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
								}
							}
							else if((sides_mem.backrest+AEM_BAG_PRESS)<sides_pr.backrest){
								operate=_BACK_;
								msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)1U;
							}
							else if((sides_mem.cushion+AEM_BAG_PRESS)<sides_pr.cushion){
								operate=_CUSH_;
								msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
							}
							else{
								msg_1R.valve.drain=(uint8_t)0U;
								run=(uint8_t)9U;    /* TODO: This is inherited value, check on next steps !!!!! */
							}
						}		
					break;
					case (uint8_t)6U:
						if(valid){
							switch(operate){
								case _BACK_:
									if((sides_pr.backrest=pressure)<
									  (sides_mem.backrest+AEM_BAG_PRESS)){
										if((sides_mem.cushion+AEM_BAG_PRESS)<sides_pr.cushion){
											operate=_CUSH_;
											msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
											run++;
										}
										else{
											operate=_NO_O_;
											run=(uint8_t)8U; /* TODO: This is inherited value, check on next steps !!!!! */
										}
										msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)0U;
									}
								break;
								case _CUSH_:
									if((sides_pr.cushion=pressure)<
									  (sides_mem.cushion+AEM_BAG_PRESS)){
										if((sides_mem.backrest+AEM_BAG_PRESS)<sides_pr.backrest){
											
											flip_order=(uint8_t)1U;
											
											operate=_BACK_;
											msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)1U;
											run++;
										}
										else{
											operate=_NO_O_;
											run=(uint8_t)8U; /* TODO: This is inherited value, check on next steps !!!!! */
										}
										msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)0U;
									}
								break;	
							}
						}
					break;		
					case (uint8_t)7U:
						if(valid){
							switch(operate){
								case _BACK_:
									if((sides_pr.backrest=pressure)<
									  (sides_mem.backrest+AEM_BAG_PRESS)){
										operate=_NO_O_;
										msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)0U;
										run++;
									}
								break;
								case _CUSH_:
									if((sides_pr.cushion=pressure)<
									  (sides_mem.cushion+AEM_BAG_PRESS)){
										operate=_NO_O_;
										msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)0U;
										run++;
									}
								break;
							}
						}		
					break;
					case (uint8_t)8U:
						if(valid){
							msg_1R.valve.drain=(uint8_t)0U;
							run++;
						}			
					break;
					case (uint8_t)9U:
						if(valid){
#if(LEGACY_OFF)
							if(memory)mem_status.last_m_sds=(memory-MEM_SIDE_S_BASE)/MEM_SIDE_S_STEP+(uint8_t)1U;
							else mem_status.last_m_sds=(uint8_t)0U;
							if(from_edit==((uint8_t)1U)){
								 mem_status.edited_sds=(uint8_t)1U;
								 from_edit=(uint8_t)0U;
							}
							else mem_status.edited_sds=(uint8_t)0U;
#else
							mem_status.last_m_sds=(memory-MEM_SIDE_S_BASE)/MEM_SIDE_S_STEP+(uint8_t)1U;
							mem_status.edited_sds=(uint8_t)0U;
#endif
							notch=__get_millis;
							run++;
						}
					break;
					case (uint8_t)10U:
						if(ms_from(notch)>__ONLY_P_SENSE){
							run=(uint8_t)0U;
							drop_limits(); /* TODO: Implement the same thing in lumbar support */
#if(LEGACY_OFF)
							trip_zero=(uint8_t)0U;
#endif
						}
					break;
					default: break; /* TODO: Add the same trap to lumbar module */
				}
			}
    	}
		/***/
		if((feed_cmd_r^msg_1R.feed_cmd)&&(!flip_order)){		
			feed_cmd_r=msg_1R.feed_cmd;
			spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)feed_cmd_r);
			notch=__get_millis;
			modified=(uint8_t)1U;
			valid=(uint8_t)0U;
		}
		else if(feed_cmd_l^msg_2L.feed_cmd){
			feed_cmd_l=msg_2L.feed_cmd;
			spi_tx_word(_L_SIDE,SPI_CMD_VALVE|(uint16_t)feed_cmd_l);
			notch=__get_millis;
			modified=(uint8_t)1U;
			valid=(uint8_t)0U;
		}
		else{
			con_burst=(uint8_t)0U;
			if(ms_from(notch)>ISOLATE_PUMP_W){
				if(p_run^pumping){
					if(p_run)pump_force(MAX_DUTY);
					else pump_force(MIN_DUTY);
					pumping=p_run;
				}
				if(modified){
					pressure_hoarder((uint8_t)0U,_TRIG_);
					modified=(uint8_t)0U;
				}
				else pressure=pressure_hoarder((uint8_t)0U,_RUN_); /* TODO: Replace _OFF_ in lumbar_apply() in the same way like here */
				if(ms_from(notch)>__ONLY_P_SENSE)valid=(uint8_t)1U;
			}
		}
	}
	
	/* Debug */
#if(0)
	if(run_state_2^((uint16_t)run)){
		previous_run_2=run_state_2;
		run_state_2=run;
	}
	operate_on_2=operate;
	press_mon=pressure;
	moment_pr=air_pressure();
#endif	
	/* ----- */
	 
	return run; //((run^(uint8_t)1U)?run:(uint8_t)0U
}

uint8_t adjust_side_support(uint8_t backrest, uint8_t cushion){
	/****/
	static uint8_t backrest_in=(uint8_t)0U;
	static uint8_t cushion_in=(uint8_t)0U;
	/****/
	static uint8_t p_run=(uint8_t)0U;
	static uint8_t p_lock=(uint8_t)0U;
	static uint8_t pumping=(uint8_t)0U;
	static uint8_t feed_cmd_r=(uint8_t)0U;
	static uint8_t feed_cmd_l=(uint8_t)0U;
	static uint8_t con_burst=(uint8_t)0U;
	static uint8_t modified=(uint8_t)0U;
	static uint8_t slide=(uint8_t)0U;
	static uint8_t valid=(uint8_t)1U;
	static uint8_t exaust=(uint8_t)0U;	
	static int16_t action=_NO_ACT_;
	static int16_t process=_NO_O_;
	static int16_t operate=_NO_O_;
	static uint32_t notch=0UL;
	/****/
	if(valid){
		backrest_in=backrest;
		cushion_in=cushion;
	}
	/****/
    if(drain.open|drain.exit){
    	if(!drain.exit){
			if(backrest_in|cushion_in){
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
    	if(!con_burst){
    		con_burst=(uint8_t)1U;
			msg_1R.feed_cmd=msg_2L.feed_cmd=(uint8_t)0U;
			p_run=p_lock=(uint8_t)0U;
			/****/
			if((backrest_in)&&(cushion_in))cushion_in=backrest_in=(uint8_t)0U;
			else if(backrest_in==(uint8_t)3U)backrest_in=(uint8_t)0U;
			else if(cushion_in==(uint8_t)3U)cushion_in=(uint8_t)0U;
			/****/
			if(backrest_in){
				switch(backrest_in){
					case (uint8_t)1U:
						if(action^_BR_INF_){
							valid=(uint8_t)0U;
							mem_status.edited_sds=(uint8_t)1U;
							operate=_BACK_;
							action=_BR_INF_;	
						}
						p_run=(uint8_t)1U;
						msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)1U;
						if(valid)sides_pr.backrest=slide;
						if(limit.b_latch){
							if(sides_pr.backrest<
							  ((MAX_BAG_PRESS-PR_HYSTERESIS)-limit.b_dempfer)){
								limit.b_latch=p_lock=(uint8_t)0U;
							}
							else p_lock=(uint8_t)1U; /* TODO: Update in lumbar support!!! */
						}
						else{
							if(sides_pr.backrest>MAX_BAG_PRESS){
								limit.b_latch=p_lock=(uint8_t)1U;
								if(++limit.b_dempfer>FLT_BAG_PRESS)limit.b_dempfer=FLT_BAG_PRESS;
							}	
						}
					break;
					case (uint8_t)2U:
						if(action^_BR_DEF_){
							valid=(uint8_t)0U;
							mem_status.edited_sds=(uint8_t)1U;
							limit.b_latch=limit.b_dempfer=(uint8_t)0U;
							operate=_BACK_;
							action=_BR_DEF_;
						}
						msg_1R.valve.backrest_l=msg_1R.valve.backrest_r=(uint8_t)1U;
						msg_1R.valve.drain=(uint8_t)1U;
						if(valid)sides_pr.backrest=__get_drop_pressure(slide);
					break;
					default: break;
				}
			}
			else if (cushion_in){
				switch(cushion_in){
					case (uint8_t)1U:
						if(action^_CS_INF_){
							valid=(uint8_t)0U;
							mem_status.edited_sds=(uint8_t)1U;
							operate=_CUSH_;
							action=_CS_INF_;	
						}
						p_run=(uint8_t)1U;
						msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
						if(valid)sides_pr.cushion=slide;
						if(limit.q_latch){
							if(sides_pr.cushion<
							  ((MAX_BAG_PRESS-PR_HYSTERESIS)-limit.q_dempfer)){
								limit.q_latch=p_lock=(uint8_t)0U;
							}
							else p_lock=(uint8_t)1U; /* TODO: Update in lumbar support!!! */
						}
						else{
							if(sides_pr.cushion>MAX_BAG_PRESS){
								limit.q_latch=p_lock=(uint8_t)1U;
								if(++limit.q_dempfer>FLT_BAG_PRESS)limit.q_dempfer=FLT_BAG_PRESS;
							}	
						}	
					break;
					case (uint8_t)2U:
						if(action^_CS_DEF_){
							valid=(uint8_t)0U;
							mem_status.edited_sds=(uint8_t)1U;
							limit.q_latch=limit.q_dempfer=(uint8_t)0U;
							operate=_CUSH_;	
							action=_CS_DEF_;
						}
						msg_2L.valve.cushion_l=msg_2L.valve.cushion_r=(uint8_t)1U;
						msg_1R.valve.drain=(uint8_t)1U;
						if(valid)sides_pr.cushion=__get_drop_pressure(slide);
					break;
					default: break;
				}	
			}
			else{
				if(action^_NO_ACT_){
					valid=(uint8_t)0U;
					action=_NO_ACT_;
					operate=_NO_O_;
				}
				if(exaust){
					msg_1R.valve.drain=(uint8_t)1U;
					process=_NO_O_;
				}	
			}
	    	/****/
	    	if((process&&operate)&&(process^operate)){
				valid=(uint8_t)0U;
				action=_NO_ACT_;
				operate=_NO_O_;
				backrest_in=cushion_in=(uint8_t)0U;
				msg_1R.feed_cmd=msg_2L.feed_cmd=(uint8_t)0U;
				p_run=(uint8_t)0U;
	    	}
    	}
    	/****/
		if((p_run)&&(p_lock)){
			if(pumping){
				pump_force(MIN_DUTY);
				pumping=(uint8_t)0U;
			}
		}
		if(feed_cmd_r^msg_1R.feed_cmd){
			feed_cmd_r=msg_1R.feed_cmd;
			spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)feed_cmd_r);
			notch=__get_millis;
			modified=(uint8_t)1U;
		}
		else if(feed_cmd_l^msg_2L.feed_cmd){
			feed_cmd_l=msg_2L.feed_cmd;
			spi_tx_word(_L_SIDE,SPI_CMD_VALVE|(uint16_t)feed_cmd_l);
			notch=__get_millis;
			modified=(uint8_t)1U;
		}
		else{
			con_burst=(uint8_t)0U;
			if(ms_from(notch)>ISOLATE_PUMP_W){
				if((pumping^p_run)&&(!p_lock)){
					pumping=p_run;
					pump_force(pumping?MAX_DUTY:MIN_DUTY);
				}
				if(modified){
					pressure_hoarder((uint8_t)0U,_TRIG_);
					modified=(uint8_t)0U;
				}
				else slide=pressure_hoarder(((operate%2)?sides_pr.backrest:sides_pr.cushion),_RUN_);
				if(ms_from(notch)>((exaust?__ONLY_P_SENSE:ISOLATE_PUMP_W)+__ONLY_P_SENSE)){
					if(operate^_NO_O_){
						valid=(uint8_t)1U;
						process=operate;
					}
					else if(process^_NO_O_)exaust=(uint8_t)1U;
					else if(exaust)exaust=(uint8_t)0U;	
					else valid=(uint8_t)1U;		
				}
			}
		}		
    }	
	return (uint8_t)((!valid)||(feed_cmd_r|feed_cmd_l));
}

/*
 * Active side support controlled by the LIN Side_Acceleration signal.
 * The common pressure sensor can measure only one circuit at a time, so
 * backrest and cushion are processed one after another.
 */
uint8_t acceleration_side_support(uint8_t request){
	enum{
		_ACC_IDLE_,
		_ACC_PREPARE_,
		_ACC_DRIVE_,
		_ACC_STOP_,
		_ACC_HOLD_
	};
	static uint8_t state=(uint8_t)_ACC_IDLE_;
	static uint8_t inflate=(uint8_t)0U;
	static uint8_t circuit=(uint8_t)_BACK_;
	static uint8_t edited_sds=(uint8_t)0U;
	static uint32_t deflate_notch=0UL;
	uint8_t pressure;
	uint8_t command;

	request=(request==(uint8_t)1U)?(uint8_t)1U:(uint8_t)0U;

	if(state==(uint8_t)_ACC_IDLE_){
		if(!request)return (uint8_t)0U;
		edited_sds=mem_status.edited_sds;
		inflate=(uint8_t)1U;
		circuit=(uint8_t)_BACK_;
		state=(uint8_t)_ACC_PREPARE_;
	}
	else if(request^inflate){
		/* First stop the current action before reversing air flow. */
		inflate=request;
		if(!inflate)deflate_notch=__get_millis;
		state=(uint8_t)_ACC_PREPARE_;
	}

	/* Stop deflation after 10 seconds even if the pressure target was not reached. */
	if((!inflate)&&(state!=(uint8_t)_ACC_IDLE_)&&
	   (ms_from(deflate_notch)>=ACCELERATION_DEFLATE_TIMEOUT)){
		if(!adjust_side_support((uint8_t)0U,(uint8_t)0U)){
			drop_limits();
			mem_status.edited_sds=edited_sds;
			state=(uint8_t)_ACC_IDLE_;
		}
		return (state==(uint8_t)_ACC_IDLE_)?(uint8_t)0U:(uint8_t)1U;
	}

	switch(state){
		case (uint8_t)_ACC_PREPARE_:
			if(!adjust_side_support((uint8_t)0U,(uint8_t)0U)){
				circuit=(uint8_t)_BACK_;
				state=(uint8_t)_ACC_DRIVE_;
			}
		break;

		case (uint8_t)_ACC_DRIVE_:
			pressure=(circuit==(uint8_t)_BACK_)?
					 sides_pr.backrest:sides_pr.cushion;
			if((inflate&&(pressure>=MAX_BAG_PRESS))||
			   ((!inflate)&&(pressure<=AEM_BAG_PRESS))){
				state=(uint8_t)_ACC_STOP_;
			}
			else{
				command=inflate?(uint8_t)1U:(uint8_t)2U;
				if(circuit==(uint8_t)_BACK_){
					(void)adjust_side_support(command,(uint8_t)0U);
				}
				else{
					(void)adjust_side_support((uint8_t)0U,command);
				}
			}
		break;

		case (uint8_t)_ACC_STOP_:
			if(!adjust_side_support((uint8_t)0U,(uint8_t)0U)){
				if(!inflate){
					if(circuit==(uint8_t)_BACK_)sides_pr.backrest=(uint8_t)0U;
					else sides_pr.cushion=(uint8_t)0U;
				}
				if(circuit==(uint8_t)_BACK_){
					circuit=(uint8_t)_CUSH_;
					state=(uint8_t)_ACC_DRIVE_;
				}
				else if(inflate)state=(uint8_t)_ACC_HOLD_;
				else{
					drop_limits();
					mem_status.edited_sds=edited_sds;
					state=(uint8_t)_ACC_IDLE_;
				}
			}
		break;

		case (uint8_t)_ACC_HOLD_:
		default:
		break;
	}

	return (state==(uint8_t)_ACC_IDLE_)?(uint8_t)0U:(uint8_t)1U;
}
