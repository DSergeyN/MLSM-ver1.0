/*
 * massage.c
 *
 *  Created on: Jan 24, 2023
 *      Author: Nick
 */

/**************************************************************************//**
* @addtogroup Система_массажа
* @{
******************************************************************************/
#include "massage.h"

/* Macros block */
#define MSG_PROG_SIZE (50U) ///< Размер массива массажной программы 
#define MSG_PROG_Side (MSG_PROG_SIZE/2U) /**< Размер стороны программы массажа */
#define MAX_BASE_PERI (20000U)
#define MSG_MAX_PROGR ((uint8_t)7U)
#define MSG_MAX_INTEN ((uint8_t)3U)
#define MSG_VAL_ARRAY (5U)
#define END_P_VALVE	  (0xEEEEU)
#define CON_P_VALVE	  (0xCCCCU)
#define LOAD_MPROGRAM(P,E) ((P)?(msg_pattern[((P)>6)?(6U):((P)-1)][E]):END_P_VALVE)
#define USE_P_ELEMENT(S,V,E) ((uint16_t const)msg_program[(MSG_PROG_Side*((uint16_t)(S)))+ \
                                             (MSG_VAL_ARRAY*(uint16_t)(V))+(uint16_t)(E)])
#define MSG_UNFOLD_TM (600U)

enum{
	_1_VALVE,
	_2_VALVE,
	_3_VALVE,
	_4_VALVE,
	_5_VALVE
};

#define VAL_U16_MAX (~(uint16_t)0) ///< Максимальное значение беззнакового 16-битного целого числа
/****************/

/**
 * @Brief Left and Right side bit-valves structures
 */
msg_1R_t msg_1R;
msg_2L_t msg_2L;

/**
 * @Brief Massage programmes full set
 */
static uint16_t const msg_pattern[MSG_MAX_PROGR][MSG_PROG_SIZE]={
		{   /* Predefined pattern #1 */
			     0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Right Bag #1  */
			   600U,   1200U,   4200U,   4800U, 0xEEEEU, /*     Right Bag #2  */
			  1200U,   1800U,   3600U,   4200U, 0xEEEEU, /*     Right Bag #3  */
			  1800U,   2400U,   3000U,   3600U, 0xEEEEU, /*     Right Bag #4  */
			  2400U,   3000U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #5  */
			/**/
		         0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Left  Bag #6  */
			   600U,   1200U,   4200U,   4800U, 0xEEEEU, /*     Left  Bag #7  */
			  1200U,   1800U,   3600U,   4200U, 0xEEEEU, /*     Left  Bag #8  */
			  1800U,   2400U,   3000U,   3600U, 0xEEEEU, /*     Left  Bag #9  */
			  2400U,   3000U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #10 */	
		},
		{   /* Predefined pattern #2 */
			     0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Right Bag #1  */
			   600U,   1200U,   4200U,   4800U, 0xEEEEU, /*     Right Bag #2  */
			  1200U,   1800U,   3600U,   4200U, 0xEEEEU, /*     Right Bag #3  */
			  1800U,   2400U,   3000U,   3600U, 0xEEEEU, /*     Right Bag #4  */
			  2400U,   3000U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #5  */
			/**/
			  2400U,   3000U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Left  Bag #6  */
			  1800U,   2400U,   3000U,   3600U, 0xEEEEU, /*     Left  Bag #7  */
			  1200U,   1800U,   3600U,   4200U, 0xEEEEU, /*     Left  Bag #8  */
			   600U,   1200U,   4200U,   4800U, 0xEEEEU, /*     Left  Bag #9  */
		         0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #10 */
		},
		{   /* Predefined pattern #3 */
			     0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Right Bag #1  */
			   600U,   1200U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #2  */
			  1200U,   1800U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #3  */
			  1800U,   2400U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #4  */
			  2400U,   3000U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #5  */
			/**/
			     0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Left  Bag #6  */
			   600U,   1200U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #7  */
			  1200U,   1800U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #8  */
			  1800U,   2400U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #9  */
			  2400U,   3000U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #10 */
		},
#if(0)
		{	/* Predefined pattern #4 */
			     0U,    600U,   4800U,   5400U, 0xEEEEU, /* Top Right Bag #1  */ /* Program with concatenation of periods for soft transient */
			   600U,   1200U,   4200U,   4800U, 0xEEEEU, /*     Right Bag #2  */ /* Initially there will be a single strike to bag #0 */
			  1200U,   1800U,   3600U,   4200U, 0xEEEEU, /*     Right Bag #3  */
			  1800U,   2400U,   3000U,   3600U, 0xEEEEU, /*     Right Bag #4  */
			  2400U,   3000U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #5  */
			/**/ 
		       300U,    900U,   5100U, 0xEEEEU, 0xEEEEU, /* Top Left  Bag #6  */
			   900U,   1500U,   4500U,   5100U, 0xEEEEU, /*     Left  Bag #7  */
			  1500U,   2100U,   3900U,   4500U, 0xEEEEU, /*     Left  Bag #8  */ 
			  2100U,   2700U,   3300U,   3900U, 0xEEEEU, /*     Left  Bag #9  */
			  2700U,   3300U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #10 */	
		},
#else
		{	/* Predefined pattern #4 */
			  2400U,   3000U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Right Bag #1  */ /* Mixed periods programme: 600 - dual bag inflation / 400 - single bag inflation*/			     
			  1800U,   2400U,   3400U,   3800U, 0xEEEEU, /*     Right Bag #2  */	   
			  1200U,   1800U,   3800U,   4200U, 0xEEEEU, /*     Right Bag #3  */	  
			   600U,   1200U,   5000U,   5400U, 0xEEEEU, /*     Right Bag #4  */	  
			     0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #5  */
			/**/ 
		      2400U,   3000U,   5100U, 0xEEEEU, 0xEEEEU, /* Top Left  Bag #6  */		       
			  1800U,   2400U,   3000U,   3400U, 0xEEEEU, /*     Left  Bag #7  */		   
			  1200U,   1800U,   4200U,   4600U, 0xEEEEU, /*     Left  Bag #8  */ 	  
			   600U,   1200U,   4600U,   5000U, 0xEEEEU, /*     Left  Bag #9  */	  
			     0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #10 */	
		},		
#endif
		{   /* Predefined pattern #5 */
			     0U,    400U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Right Bag #1  */ /* Single bag at a time program: smaller time base - 400 ms */
			  1200U,   1600U,   6000U,   6400U, 0xEEEEU, /*     Right Bag #2  */
			  1600U,   2000U,   4800U,   5200U, 0xEEEEU, /*     Right Bag #3  */
			  2800U,   3200U,   4400U,   4800U, 0xEEEEU, /*     Right Bag #4  */
			  3200U,   3600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #5  */
			/**/  
			   400U,    800U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Left  Bag #6  */
			   800U,   1200U,   5600U,   6000U, 0xEEEEU, /*     Left  Bag #7  */
			  2000U,   2400U, 	5200U,   5600U, 0xEEEEU, /*     Left  Bag #8  */
			  2400U,   2800U,   4000U,   4400U, 0xEEEEU, /*     Left  Bag #9  */
			  3600U,   4000U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #10 */
		},
		{   /* Predefined pattern #6 */
			     0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Right Bag #1  */
			   600U,   1200U,   1800U,   2400U, 0xEEEEU, /*     Right Bag #2  */
			  1200U,   1800U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #3  */
			0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #4  */
			0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #5  */ 
			/**/	   
			     0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Left  Bag #6  */
			   600U,   1200U,   1800U,   2400U, 0xEEEEU, /*     Left  Bag #7  */
			  1200U,   1800U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #8  */
			0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #9  */
			0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #10 */ 
		},
		{   /* Predefined pattern #7 */
			0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Right Bag #1  */
			0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #2  */
			0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #3  */
			     0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #4  */
			   600U,   1200U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Right Bag #5  */ 
			/**/	   
			0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, /* Top Left  Bag #6  */
			0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #7  */
			0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #8  */
			     0U,    600U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #9  */
			   600U,   1200U, 0xEEEEU, 0xEEEEU, 0xEEEEU, /*     Left  Bag #10 */ 
		},		
};

static uint16_t msg_program[MSG_PROG_SIZE]; /* Executable massage program */

/**
 * @Brief Structure to share data between msg_run() and
 */
static struct{
	uint8_t program;
	uint8_t intensity;
   uint16_t period;
   uint16_t timer;
   uint32_t notch;
}msg_state;

/**
 * @Brief Type and union to control bag sequence execution state
 */
typedef struct{
	/* R sequence active */
	uint16_t r_1: 2;
	uint16_t r_2: 2;
	uint16_t r_3: 2;
	uint16_t r_4: 1;
	uint16_t r_5: 1;
	/* L sequence active */
	uint16_t l_1: 2;
	uint16_t l_2: 2;
	uint16_t l_3: 2;
	uint16_t l_4: 1;
	uint16_t l_5: 1;	
}EOS_f_t;

static union{
	EOS_f_t flag;
	uint16_t arm; /* sequence */
}sequence;

/**
 * @Brief Type and unions to control bag sequence execution progress
 */
typedef struct{
	uint16_t seq_1: 3;
	uint16_t seq_2: 3;
	uint16_t seq_3: 3;
	uint16_t seq_4: 3;
	uint16_t seq_5: 3;
	uint16_t _tail: 1;
}carriage_t;

static union{
	carriage_t val;
	  uint16_t drain;
}carrier_1R;

static union{
	carriage_t val;
	  uint16_t drain;
}carrier_2L;

/**
 * @Brief Upload actual massage programme to executable array
 */
void arm_msg_program(uint8_t prog, uint8_t intensity){
	uint16_t i=0U;
/********************************/	
#if LEGACY_OFF
	static uint8_t prog_latch=(uint8_t)0U;
	static uint8_t inte_latch=(uint8_t)0U;
	uint16_t buffer;
	/****/
	if((!prog)||(!intensity)){
		read_16w((uint8_t)0U,&buffer);
		if(!prog){
			prog=(buffer>>((uint8_t)8U))&((uint8_t)0xFFU);
			prog=prog>MSG_MAX_PROGR?((uint8_t)1U):prog;
			prog=prog?prog:((uint8_t)1U);
		}
		if(!intensity){
			intensity=(uint8_t)(buffer&0xFFU);
			intensity=intensity>MSG_MAX_INTEN?((uint8_t)1U):intensity;
			intensity=intensity?intensity:(uint8_t)1U;
		}
		if(buffer^((((uint16_t)prog)<<8U)|((uint16_t)intensity))){
			buffer=(((uint16_t)prog)<<8U)|((uint16_t)intensity);
			write_16w((uint8_t)0U,buffer);
			prog_latch=prog;
			inte_latch=intensity;
		}
	}
	else if((prog_latch^prog)||(inte_latch^intensity)){
		buffer=(((uint16_t)prog)<<8U)|((uint16_t)intensity);
		write_16w((uint8_t)0U,buffer);
		prog_latch=prog;
		inte_latch=intensity;
	}
#endif
/********************************/		
	if(!prog)prog=msg_state.program;
	else{
		prog=prog>MSG_MAX_PROGR?MSG_MAX_PROGR:prog;
		msg_state.program=prog;
	}
	if(!intensity){
		intensity=msg_state.intensity?msg_state.intensity:(uint8_t)1U;
	}
	else{
		intensity=intensity>MSG_MAX_INTEN?MSG_MAX_INTEN:intensity;
		intensity=intensity?intensity:(uint8_t)1U;
		msg_state.intensity=intensity;		
	}
	/* Array copy & search max value */
	msg_state.period=msg_state.timer=0U;
	for(;i<MSG_PROG_SIZE;i++){
		if((msg_program[i]=LOAD_MPROGRAM(prog,i))<MAX_BASE_PERI){
			msg_program[i]*=(uint16_t)intensity;
			msg_state.period=msg_state.period<msg_program[i]?
							 msg_program[i]:msg_state.period;
		}
	}
	msg_state.period=msg_state.period?
			         msg_state.period:MAX_BASE_PERI;
	return;
}

/* Debug variables */
#if(0)
uint16_t right_valves, left_valves;
#endif
/*******************/

/**
 * @Brief Run massage programme
 */
uint8_t msg_run(uint8_t process){
	static uint8_t latch=(uint8_t)0U;
	static uint8_t feed_righ=(uint8_t)0U;
	static uint8_t feed_left=(uint8_t)0U;
	static uint8_t unfold=(uint8_t)0U;
	static uint8_t suppress=(uint8_t)0U;  /* Suppress early toggle */
	static uint16_t compare=0U;
	/***/
	if(suppress)process=(uint8_t)0U;
	/***/
    if(drain.open|drain.exit){
    	if(!drain.exit){
			if(process){
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
    else if(process^latch){
		if(!(__SPI_STATE->busy)){
			if(process){
				msg_state.notch=__get_millis;
				carrier_1R.drain=carrier_2L.drain=0U;
				msg_1R.feed_cmd=msg_2L.feed_cmd=(uint8_t)0U;
				feed_righ=feed_left=(uint8_t)0U;
				sequence.arm=VAL_U16_MAX;
				unfold=(uint8_t)1U;
				latch=process;
				/* Start pump from here */
				if(msg_state.program)pump_force(MAX_DUTY);
				/************************/
			}
			else{
				suppress=(uint8_t)1U;
				if(unfold){
					if(feed_righ){
						__SPI_STATE->busy=spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)(feed_righ=(uint8_t)0U));
					}
					else if(feed_left){
						__SPI_STATE->busy=spi_tx_word(_L_SIDE,SPI_CMD_VALVE|(uint16_t)(feed_left=(uint8_t)0U));
					}
					else{
						/* Stop pump from here */
						pump_force(MIN_DUTY);
						/***********************/
						msg_1R.feed_cmd=msg_2L.feed_cmd=(uint8_t)0U;
						msg_state.notch=__get_millis;
						unfold=(uint8_t)0U;	
					}
				}
				else if(ms_from(msg_state.notch)>(uint32_t)MSG_UNFOLD_TM){
					suppress=latch=process;
				}
			}	
		}	
	}
	else if(latch){
		msg_state.timer=(uint16_t)ms_from(msg_state.notch);
		if(!(__SPI_STATE->busy)){
			if(feed_righ^(msg_1R.feed_cmd)){
#if(0)
				right_valves=(uint16_t)(feed_righ=msg_1R.feed_cmd); /* right_valves is for debug only */
#else
				feed_righ=msg_1R.feed_cmd;
#endif
				__SPI_STATE->busy=spi_tx_word(_R_SIDE,SPI_CMD_VALVE|(uint16_t)feed_righ);
			}
			else if(feed_left^(msg_2L.feed_cmd)){
#if(0)
				left_valves=(uint16_t)(feed_left=msg_2L.feed_cmd); /* left_valves is for debug only  */
#else
				feed_left=msg_2L.feed_cmd;
#endif			
				__SPI_STATE->busy=spi_tx_word(_L_SIDE,SPI_CMD_VALVE|(uint16_t)feed_left);				
			}
			else if(sequence.arm||(msg_state.timer<=msg_state.period)){
				{	/* R side */
					/* valve #1 processing */
					compare=USE_P_ELEMENT(_R_SIDE,_1_VALVE,carrier_1R.val.seq_1);
					if(msg_state.timer>=compare){
						msg_1R.valve.bag_1=~msg_1R.valve.bag_1;
						carrier_1R.val.seq_1++;
					}
					else if(compare>msg_state.period)sequence.flag.r_1=0U;
					/* valve #2 processing */
					compare=USE_P_ELEMENT(_R_SIDE,_2_VALVE,carrier_1R.val.seq_2);
					if(msg_state.timer>=compare){
						msg_1R.valve.bag_2=~msg_1R.valve.bag_2;
						carrier_1R.val.seq_2++;
					}
					else if(compare>msg_state.period)sequence.flag.r_2=0U;
					/* valve #3 processing */
					compare=USE_P_ELEMENT(_R_SIDE,_3_VALVE,carrier_1R.val.seq_3);
					if(msg_state.timer>=compare){
						msg_1R.valve.bag_3=~msg_1R.valve.bag_3;
						carrier_1R.val.seq_3++;
					}
					else if(compare>msg_state.period)sequence.flag.r_3=0U;
					/* valve #4 processing */
					compare=USE_P_ELEMENT(_R_SIDE,_4_VALVE,carrier_1R.val.seq_4);
					if(msg_state.timer>=compare){
						msg_1R.valve.bag_4=~msg_1R.valve.bag_4;
						carrier_1R.val.seq_4++;
					}
					else if(compare>msg_state.period)sequence.flag.r_4=0U;
					/* valve #5 processing */
					compare=USE_P_ELEMENT(_R_SIDE,_5_VALVE,carrier_1R.val.seq_5);
					if(msg_state.timer>=compare){
						msg_1R.valve.bag_5=~msg_1R.valve.bag_5;
						carrier_1R.val.seq_5++;
					}
					else if(compare>msg_state.period)sequence.flag.r_5=0U;
				};
				{   /* L side */
					/* L side valve #1 processing */
					compare=USE_P_ELEMENT(_L_SIDE,_1_VALVE,carrier_2L.val.seq_1);
					if(msg_state.timer>=compare){
						msg_2L.valve.bag_1=~msg_2L.valve.bag_1;
						carrier_2L.val.seq_1++;
					}
					else if(compare>msg_state.period)sequence.flag.l_1=0U;
					/* L side valve #2 processing */
					compare=USE_P_ELEMENT(_L_SIDE,_2_VALVE,carrier_2L.val.seq_2);
					if(msg_state.timer>=compare){
						msg_2L.valve.bag_2=~msg_2L.valve.bag_2;
						carrier_2L.val.seq_2++;
					}
					else if(compare>msg_state.period)sequence.flag.l_2=0U;
					/* L side valve #3 processing */
					compare=USE_P_ELEMENT(_L_SIDE,_3_VALVE,carrier_2L.val.seq_3);
					if(msg_state.timer>=compare){
						msg_2L.valve.bag_3=~msg_2L.valve.bag_3;
						carrier_2L.val.seq_3++;
					}
					else if(compare>msg_state.period)sequence.flag.l_3=0U;
					/* L side valve #4 processing */
					compare=USE_P_ELEMENT(_L_SIDE,_4_VALVE,carrier_2L.val.seq_4);
					if(msg_state.timer>=compare){
						msg_2L.valve.bag_4=~msg_2L.valve.bag_4;
						carrier_2L.val.seq_4++;
					}
					else if(compare>msg_state.period)sequence.flag.l_4=0U;
					/* L side valve #5 processing */
					compare=USE_P_ELEMENT(_L_SIDE,_5_VALVE,carrier_2L.val.seq_5);
					if(msg_state.timer>=compare){
						msg_2L.valve.bag_5=~msg_2L.valve.bag_5;
						carrier_2L.val.seq_5++;
					}
					else if(compare>msg_state.period)sequence.flag.l_5=0U;
				};	
			}
			else{
				msg_state.notch=__get_millis;
				carrier_1R.drain=carrier_2L.drain=0U;
				sequence.arm=VAL_U16_MAX;
			}
		}
	}
	return latch;
}

/**
* @}
*/
