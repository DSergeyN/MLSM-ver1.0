/*
 * gpio.h
 *
 *  Created on: Nov 22, 2022
 *      Author: Nick
 */

#ifndef __GPIO__
#define __GPIO__

typedef enum{
	_OFF_,
	_ON_
}TOGGLE_st_t;

typedef enum{
	_RUN_,
	_TRIG_	
}TRIGGER_st_t;

/*---- GPIO for supply routing and other features ----*/
#define VDD_MON_TOGGLE(T) ((T)?(PTADL|=(uint8_t)0x02U): \
						     (PTADL&=~((uint8_t)0x02U))) 		/* Toggle ON/OFF supply to divider bridge for voltage monitor       */
#define VDD_DRV_TOGGLE(T) ((T)?(PTP|=(uint8_t)0x10U):	\
							 (PTP&=~((uint8_t)0x10U)))			/* Toggle ON/OFF supply for L9733 - valve drivers                   */
#define CNS_DIS_TOGGLE(T) ((T)?(PTP|=(uint8_t)0x40U):	\
							 (PTP&=~((uint8_t)0x40U)))			/* Toggle ON/OFF current sensing on VN5E160AS-E - pump driver       */
#define VDD_PSN_TOGGLE(T) ((T)?(PTP|=(uint8_t)0x80U):	\
		 	 	 	 	 	 (PTP&=~((uint8_t)0x80U)))			/* Toggle ON/OFF supply for MPXV5100DP - pressure sensor            */



#endif /* __GPIO__ */
