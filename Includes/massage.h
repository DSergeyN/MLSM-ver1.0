/*
 * massage.h
 *
 *  Created on: Jan 24, 2023
 *      Author: Nick
 */

#ifndef __MASSAGE__
#define __MASSAGE__

/*---- Required system headers ---------*/
#include <mc9s12zvl32.h>
#include <types.h>

/*---- Required application headers ----*/
#include "timer.h"
#include "pump.h"
/****/
#include "valve.h" /* Root pneumatic control */
/*--------------------------------------*/

/**
 * @Refers   Datatypes 
 * @Specific Right side valves datatype
 */	
#if(0)
typedef struct{
	uint8_t all_bag: 5;
	uint8_t        : 3;	
}drain_m_side_t;
#endif

#if(0)
typedef struct{
	uint8_t bag_1: 1;
	uint8_t bag_2: 1;
	uint8_t bag_3: 1;
	uint8_t bag_4: 1;
	uint8_t bag_5: 1;
	uint8_t drain: 1;
	uint8_t backrest_l: 1;
	uint8_t backrest_r: 1;
}valve_R_side_t;
#else
typedef struct{
	uint8_t bag_1: 1;
	uint8_t bag_2: 1;
	uint8_t bag_3: 1;
	uint8_t bag_4: 1;
	uint8_t bag_5: 1;
	uint8_t drain: 1;
	uint8_t backrest_l: 1; // This is backrest left Corrected
	uint8_t backrest_r: 1; // Tested
}valve_R_side_t;
#endif

#define MSR_VALVES_MASK (0xFFFFU) /* Mask for SPI diagnostic reply with valves 
									 positions used in: massage right; backrest sds. and drain.  */

typedef union{
	valve_R_side_t valve;
	       uint8_t feed_cmd;	
}msg_1R_t;

extern msg_1R_t msg_1R;

/**** Try bit manipulation to avoid data flip in unions, MISRA C requirements ****/
#define MSG_1R_BAG_1(X) ((X)?(msg_1R.feed_cmd|=(uint8_t)0x01U):(msg_1R.feed_cmd&=(uint8_t)0xFEU))
#define MSG_1R_BAG_2(X) ((X)?(msg_1R.feed_cmd|=(uint8_t)0x02U):(msg_1R.feed_cmd&=(uint8_t)0xFDU))
#define MSG_1R_BAG_3(X) ((X)?(msg_1R.feed_cmd|=(uint8_t)0x04U):(msg_1R.feed_cmd&=(uint8_t)0xFBU))
#define MSG_1R_BAG_4(X) ((X)?(msg_1R.feed_cmd|=(uint8_t)0x08U):(msg_1R.feed_cmd&=(uint8_t)0xF7U))
#define MSG_1R_BAG_5(X) ((X)?(msg_1R.feed_cmd|=(uint8_t)0x10U):(msg_1R.feed_cmd&=(uint8_t)0xEFU))
#define MSG_1R_DRAIN(X) ((X)?(msg_1R.feed_cmd|=(uint8_t)0x20U):(msg_1R.feed_cmd&=(uint8_t)0xDFU))

/**
 * @Refers   Datatypes 
 * @Specific Left side valves datatype
 */	
#if(0)
typedef struct{
	uint8_t bag_5: 1;
	uint8_t bag_4: 1;
	uint8_t bag_3: 1;
	uint8_t bag_2: 1;
	uint8_t bag_1: 1;
	uint8_t qushion_l: 1;
	uint8_t qushion_r: 1;
	uint8_t 	 : 1;
}valve_L_side_t;
#else
typedef struct{
	uint8_t bag_5: 1;
	uint8_t bag_4: 1;
	uint8_t bag_3: 1;
	uint8_t bag_2: 1;
	uint8_t bag_1: 1;
	uint8_t cushion_l:  1; // This is cushion left!!!! corrected!!!!
	uint8_t cushion_r:  1; // Todo: not tested
	uint8_t 		 :  1;
}valve_L_side_t;
#endif

#define MSL_VALVES_MASK (0x3FFFU) /* Mask for SPI diagnostic reply with valves 
									 positions used in: massage left and cushion sds. */

typedef union{
	valve_L_side_t valve;
	       uint8_t feed_cmd;	
}msg_2L_t;

extern msg_2L_t msg_2L;

/**** Try bit manipulation to avoid data flip in unions, MISRA C requirements ****/
#define MSG_2L_BAG_5(X) ((X)?(msg_2L.feed_cmd|=(uint8_t)0x01U):(msg_2L.feed_cmd&=(uint8_t)0xFEU))
#define MSG_2L_BAG_4(X) ((X)?(msg_2L.feed_cmd|=(uint8_t)0x02U):(msg_2L.feed_cmd&=(uint8_t)0xFDU))
#define MSG_2L_BAG_3(X) ((X)?(msg_2L.feed_cmd|=(uint8_t)0x04U):(msg_2L.feed_cmd&=(uint8_t)0xFBU))
#define MSG_2L_BAG_2(X) ((X)?(msg_2L.feed_cmd|=(uint8_t)0x08U):(msg_2L.feed_cmd&=(uint8_t)0xF7U))
#define MSG_2L_BAG_1(X) ((X)?(msg_2L.feed_cmd|=(uint8_t)0x10U):(msg_2L.feed_cmd&=(uint8_t)0xEFU))

/*---- Functions prototypes ------------*/
void arm_msg_program(uint8_t, uint8_t);
uint8_t msg_run(uint8_t);
/*--------------------------------------*/

#endif /* __MASSAGE__ */
