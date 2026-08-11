/*
 * control.c
 *
 *  Created on: Nov 28, 2022
 *      Author: Nick
 */

/*
	---------- LIN status word decoding ----------
	|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|
			 |             | | | | | | | |-- Error in response
			 |             | | | | | | |---- Successful transfer
			 |             | | | | | |------ Overrun
			 |             | | | | |-------- Goto sleep
			 |             | | | |---------- Bus activity
			 |             | | |------------ Event trigger collision
			 |             | |-------------- Save configuration
			 |             |---------------- 0
	         |------------------------------ Last frame PID 
*/

#define PNEUMATIC_LOCK (5000UL)	/* Pneumatic lock timer if any key was pressed */

#include "control.h"

typedef struct{
	uint8_t MemSave_flag;
	uint8_t MemNumber;
	uint8_t SaveNumber;
	uint8_t BCM_EEM;
	uint8_t Seat_Id;
	uint8_t Seat_Var;
	uint8_t LumbarHeadUp;
	uint8_t LumbarHeadIncrease;
	uint8_t LumbarHeadDown;
	uint8_t LumbarHeadDecrease;
	uint8_t InternalDiagnostics;          
	 int8_t CabinAmbientTemp;
}global_set_t;

typedef struct{ /* Make it 8-bit later */
	uint8_t Massage_on;
	uint8_t Massage_intensity;
	uint8_t Massage_program;
	uint8_t Sidesupport_on;
	uint8_t Sidesupport_backrest;
    uint8_t Sidesupport_cushion;
     int8_t Side_Acceleration;
}pneumo_set_t;

typedef struct{
    uint8_t LumbarCtrUp;
    uint8_t LumbarCtrIncrease;
    uint8_t LumbarCtrDown;
    uint8_t LumbarCtrDecrease;
    uint8_t sigSSWLinError;
}keyboard_in_t;

typedef struct{
	 global_set_t set;
	 pneumo_set_t pne;
	keyboard_in_t key;
	uint8_t Upproj;
	uint8_t startaddr;
	uint8_t any_key;
	uint8_t any_cmd;
}data_exch_t;

data_exch_t input;

struct{
	uint8_t msg_on;
	uint8_t program;
	uint8_t intensity;
}massage_st;

struct{
	uint8_t active;
	uint8_t transit;
	uint8_t memory;		/* Maybe this variable is useless */
	uint8_t _dummy;
}lumbar_st;

struct{
	uint8_t active;
	uint8_t transit;
	uint8_t memory;		/* Maybe this variable is useless */
	uint8_t _dummy;	
}side_s_st;

struct{
	uint8_t data_byte;
	uint8_t outbox;
}diagnostics;

static uint16_t test_lin;
bool side_mem_lin_enable;

/* Debug variables */

/*******************/

/** 
 * @Brief Assemble and assert diagnostic reply
 */
static uint8_t internal_diagnostics(void){
	/****/
	static uint8_t diag_last=(uint8_t)0U;
	/****/
	if(diag_last^input.set.InternalDiagnostics){
		diag_last=input.set.InternalDiagnostics;
		switch((int16_t)diag_last){
			case Seat_lumbar_support_valve_status:
				 diagnostics.data_byte=(uint8_t)1U;
			break;
			case Seat_side_support_valve_status:
			case Seat_lumbar_support_position_actual:	
			case Seat_lumbar_support_position_target:
				 diagnostics.data_byte=(uint8_t)2U;
			break;
			case Seat_massage_control_valve_status:
			case DCTs_lumbar_valves_errors:
			case DCTs_side_support_valves_errors:
			case DCTs_massage_left_valve_errors:
			case DCTs_massage_right_valve_errors: 
				 diagnostics.data_byte=(uint8_t)3U;		
			break;
			default: /* All single byte requests */
				 diagnostics.data_byte=(uint8_t)0U;
			break;	
		}
	}
	/****/
	l_u8_wr_LI0_Diagnostics_Code(diag_last);
	l_bool_wr_LI0_DTCs_Pending(get_dtcs_status());
	l_u8_wr_LI0_Diagnostics_Ctr(diagnostics.data_byte);
	if(diagnostics.data_byte)diagnostics.data_byte--;
	else diag_last=(uint8_t)Seat_inflation_pressure_sensor_input;
	/****/
	switch(diag_last){
		case Seat_inflation_pressure_sensor_input:
			l_u8_wr_LI0_Data_Register((uint8_t)air_pressure());
		break;
		case Seat_lumbar_support_valve_status:
			l_u8_wr_LI0_Data_Register(get_valve_diagnostics(diag_last, diagnostics.data_byte)); //TODO: Unite all request of same kind. See below - (*) */
		break;
		case Seat_air_pump_status:
			if(!get_pump_dtcs((uint8_t)0U)){
				 l_u8_wr_LI0_Data_Register((uint8_t)__pump_active);
			}
			else l_u8_wr_LI0_Data_Register((uint8_t)2U);
		break;
		case Seat_massage_control_valve_status:
			l_u8_wr_LI0_Data_Register(get_valve_diagnostics(diag_last, diagnostics.data_byte)); //TODO: (*) Request for merging */
		break;
		case Seat_side_support_valve_status:
			l_u8_wr_LI0_Data_Register(get_valve_diagnostics(diag_last, diagnostics.data_byte)); //TODO: (*) Request for merging */
		break;
		case Seat_control_switch_status_lumbar_control:
			diagnostics.outbox=((input.set.LumbarHeadDecrease|input.key.LumbarCtrDecrease)&((uint8_t)0x03U))<<((uint8_t)6U);
			diagnostics.outbox|=((input.set.LumbarHeadIncrease|input.key.LumbarCtrIncrease)&((uint8_t)0x03U))<<((uint8_t)4U);
			diagnostics.outbox|=((input.set.LumbarHeadDown|input.key.LumbarCtrDown)&((uint8_t)0x03U))<<((uint8_t)2U);
			diagnostics.outbox|=((input.set.LumbarHeadUp|input.key.LumbarCtrUp)&((uint8_t)0x03U));
			l_u8_wr_LI0_Data_Register(diagnostics.outbox);
		break;
		case Seat_lumbar_support_position_actual:
			switch(diagnostics.data_byte){
				case (uint8_t)2U: l_u8_wr_LI0_Data_Register(lmb_press.upper); break;
				case (uint8_t)1U: l_u8_wr_LI0_Data_Register(lmb_press.middl); break;
				default: l_u8_wr_LI0_Data_Register(lmb_press.lower); break;
			}
		break;
		case Seat_lumbar_support_position_target:
			switch(diagnostics.data_byte){
				case (uint8_t)2U: l_u8_wr_LI0_Data_Register(lmb_memory.upper); break;
				case (uint8_t)1U: l_u8_wr_LI0_Data_Register(lmb_memory.middl); break;
				default: l_u8_wr_LI0_Data_Register(lmb_memory.lower); break;
			}
		break;
		case Seat_side_support_request_status:
			diagnostics.outbox=input.pne.Sidesupport_on?
					((input.pne.Side_Acceleration==(int8_t)1)?((uint8_t)0x02U):((uint8_t)0x01U)):((uint8_t)0x00U);
			diagnostics.outbox|=(input.pne.Sidesupport_backrest&((uint8_t)0x03U))<<((uint8_t)2U);
			diagnostics.outbox|=(input.pne.Sidesupport_backrest&((uint8_t)0x03U))<<((uint8_t)4U);
			l_u8_wr_LI0_Data_Register(diagnostics.outbox);
		break;
		case DTCs_pump_and_pressure_sensor:
			l_u8_wr_LI0_Data_Register(get_pump_dtcs((uint8_t)0U));
			get_pump_dtcs((uint8_t)1U); /* Erase pump DCTs pending */
		break;
		case DCTs_lumbar_valves_errors:
			l_u8_wr_LI0_Data_Register(get_valve_diagnostics(diag_last, diagnostics.data_byte)); /* TODO: (*) Request for merging */
		break;
		
	
	/*
	 Seat_inflation_pressure_sensor_input      =0,
	Seat_lumbar_support_valve_status          =1,
	Seat_air_pump_status                      =2,
	Seat_massage_control_valve_status         =3,
	FIU_seat_massage_request_status	          =4,
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
	 
	 */
	}
	/****/
	return (uint8_t)0U;
}

uint8_t command_extract(void){
	
	static uint32_t notch=0UL;
	
	/* This is temporary code for initial testing as demo firmware */
	static uint32_t tester=0UL;
	static uint8_t latch=(uint8_t)1U;
	static uint8_t r_flag=(uint8_t)0U;
	static uint8_t transit=(uint8_t)0U;
	static uint8_t status=(uint8_t)0U;
	/****/
	static uint8_t tim_init=(uint8_t)1U;
	static uint8_t no_ldata=(uint8_t)1U;
	static uint8_t memory_hold=(uint8_t)0U;
	/****/
	uint16_t buffer;
	/* This is temporary code for initial testing as demo firmware */
	
	if(tim_init){
		notch=__get_millis;
		tim_init=(uint8_t)0U;
	}
	
	if(l_flg_tst_LI0_SMM_brdcst_flag()){	/* Broadcast frame successfully received */
		l_flg_clr_LI0_SMM_brdcst_flag();
		/* Get MemSave flag to store seat settings */
		if(l_flg_tst_LI0_MemSave_flag_flag()){
			l_flg_clr_LI0_MemSave_flag_flag();
			input.set.MemSave_flag=(uint8_t)l_bool_rd_LI0_MemSave_flag();
		}
		/* Get Memory position to maintain/store settings */
		if(l_flg_tst_LI0_MemNumber_flag()){
			l_flg_clr_LI0_MemNumber_flag();
			input.set.MemNumber=(uint8_t)l_u8_rd_LI0_MemNumber();
			if (input.set.MemNumber)
				side_mem_lin_enable = TRUE;	
		}
		/* Get Energy save profile */
		if(l_flg_tst_LI0_BCM_EEM_flag()){
			l_flg_clr_LI0_BCM_EEM_flag();
			input.set.BCM_EEM=l_u8_rd_LI0_BCM_EEM();
		}
		/* Get seat ID for specific behaviour */
		if(l_flg_tst_LI0_Seat_Id_flag()){
			l_flg_clr_LI0_Seat_Id_flag();
			input.set.Seat_Id=(uint8_t)l_u8_rd_LI0_Seat_Id();
		}
		/* Get seat variant */
		if(l_flg_tst_LI0_Seat_Var_flag()){
			l_flg_clr_LI0_Seat_Var_flag();
			input.set.Seat_Var=(uint8_t)l_u8_rd_LI0_Seat_Var();
		}
		/* Get OBU lumbar Up signal */
		if(l_flg_tst_LI0_LumbarHeadUp_flag()){
			l_flg_clr_LI0_LumbarHeadUp_flag();
			input.set.LumbarHeadUp=(uint8_t)l_bool_rd_LI0_LumbarHeadUp();
		}
		/* Get OBU lumbar Increase signal */
		if(l_flg_tst_LI0_LumbarHeadIncrease_flag()){
			l_flg_clr_LI0_LumbarHeadIncrease_flag();
			input.set.LumbarHeadIncrease=(uint8_t)l_bool_rd_LI0_LumbarHeadIncrease();
		}
		/* Get OBU lumbar Down signal */
		if(l_flg_tst_LI0_LumbarHeadDown_flag()){
			l_flg_clr_LI0_LumbarHeadDown_flag();
			input.set.LumbarHeadDown=(uint8_t)l_bool_rd_LI0_LumbarHeadDown();
		}
		/* Get OBU lumbar Decrease signal */
		if(l_flg_tst_LI0_LumbarHeadDecrease_flag()){
			l_flg_clr_LI0_LumbarHeadDecrease_flag();
			input.set.LumbarHeadDecrease=(uint8_t)l_bool_rd_LI0_LumbarHeadDecrease();
		}
		/* Get internal diagnostics command */
		if(l_flg_tst_LI0_Int_Diagnostics_flag()){
			l_flg_clr_LI0_Int_Diagnostics_flag();
			input.set.InternalDiagnostics=(uint8_t)l_u8_rd_LI0_Int_Diagnostics();
		}
		/* Get Cabin Ambient Temperature */
		if(l_flg_tst_LI0_CabinAmbientTemp_flag()){
			l_flg_clr_LI0_CabinAmbientTemp_flag();
			input.set.CabinAmbientTemp=(int8_t)l_u8_rd_LI0_CabinAmbientTemp();
			if(input.set.CabinAmbientTemp&((int8_t)0x20)){
			   input.set.CabinAmbientTemp|=(int8_t)0xC0;
			}
		}	
		/* Sleep controls */
		tester=notch=__get_millis;
		r_flag=(uint8_t)1U;
		/* Memory from EEPROM if signal is false */
		if(input.set.MemNumber&&(!input.set.MemSave_flag)){
			if(memory_hold^input.set.MemNumber){
				memory_hold=input.set.MemNumber;
				buffer=(((uint16_t)memory_hold)<<8U)|0xFFU;
				write_16w((uint8_t)2U,buffer);
			}
			if(mem_status.edited_sds)mem_status.last_m_sds=(uint8_t)0U;
			if(mem_status.edited_lmb)mem_status.last_m_lmb=(uint8_t)0U;
		}
		else{
			input.set.SaveNumber=input.set.MemNumber;
			read_16w((uint8_t)2U,&buffer);
			input.set.MemNumber=(uint8_t)((buffer>>8U)&0xFFU);
			if(input.set.MemNumber>MEM_SETINGS_MAX){
				input.set.MemNumber=(uint8_t)1U;
				buffer=0x01FFU;
				write_16w((uint8_t)2U,buffer);
			}
		}	
		/****/
	}	
	/*  pNsystem_ctr frame processing */
	if(l_flg_tst_LI0_pNsystem_ctr_flag()){	/* pNsystem frame successfully received */
		l_flg_clr_LI0_pNsystem_ctr_flag();
		/* Get massage on flag */
		if(l_flg_tst_LI0_Massage_on_flag()){
			l_flg_clr_LI0_Massage_on_flag();
			input.pne.Massage_on=(uint8_t)l_bool_rd_LI0_Massage_on();		
		}
		/* Get massage intensity setting */
		if(l_flg_tst_LI0_Massage_intensity_flag()){
			l_flg_clr_LI0_Massage_intensity_flag();
			input.pne.Massage_intensity=(uint8_t)l_u8_rd_LI0_Massage_intensity();
		}
		/* Get massage program setting */
		if(l_flg_tst_LI0_Massage_program_flag()){
			l_flg_clr_LI0_Massage_program_flag();
			input.pne.Massage_program=(uint8_t)l_u8_rd_LI0_Massage_program();	
		}
		/* Get side support on flag */
		if(l_flg_tst_LI0_Sidesupport_on_flag()){
			l_flg_clr_LI0_Sidesupport_on_flag();
			input.pne.Sidesupport_on=(uint8_t)l_bool_rd_LI0_Sidesupport_on();
		}		
		/* Side support backrest adjust */
		if(l_flg_tst_LI0_Sidesupport_backrest_flag()){
			l_flg_clr_LI0_Sidesupport_backrest_flag();
			input.pne.Sidesupport_backrest=(uint8_t)l_u8_rd_LI0_Sidesupport_backrest();
		}		
		/* Side support cushion adjust */
		if(l_flg_tst_LI0_Sidesupport_cushion_flag()){
			l_flg_clr_LI0_Sidesupport_cushion_flag();
			input.pne.Sidesupport_cushion=(uint8_t)l_u8_rd_LI0_Sidesupport_cushion();
		}
		/* Side acceleration for Active side support */
		if(l_flg_tst_LI0_Side_Acceleration_flag()){
			l_flg_clr_LI0_Side_Acceleration_flag();
			input.pne.Side_Acceleration=(int8_t)l_u8_rd_LI0_Side_Acceleration();
			/* Signed 6-bit to int8_t type conversion */
			if(input.pne.Side_Acceleration&((int8_t)0x20)){
			   input.pne.Side_Acceleration|=(int8_t)0xC0;			   
			}
		}
		/****/
		/* 21_02_2023 update */
		if((input.pne.Sidesupport_backrest)||
		   (input.pne.Sidesupport_cushion)){
			 input.any_cmd=(uint8_t)1U;
		}
		else input.any_cmd=(uint8_t)0U;
		/*********************/
	}
	/* Seatsetting frame processing */
	if(l_flg_tst_LI0_Seatsetting_flag()){ /* Seatsetting frame successfully received */
		l_flg_clr_LI0_Seatsetting_flag();
		/* Get LumbarCtrUp signal */
		if(l_flg_tst_LI0_LumbarCtrUp_flag()){
			l_flg_clr_LI0_LumbarCtrUp_flag();
			input.key.LumbarCtrUp=(uint8_t)l_bool_rd_LI0_LumbarCtrUp();
			input.key.LumbarCtrUp|=input.set.LumbarHeadUp;
			
			//input.key.LumbarCtrUp=
			//	  input.key.LumbarCtrUp||input.set.LumbarHeadUp;				/* Union with command from OBU */
		}
		/* Get LumbarCtrIncrease signal */
		if(l_flg_tst_LI0_LumbarCtrIncrease_flag()){
			l_flg_clr_LI0_LumbarCtrIncrease_flag();
			input.key.LumbarCtrIncrease=(uint8_t)l_bool_rd_LI0_LumbarCtrIncrease();
			input.key.LumbarCtrIncrease|=input.set.LumbarHeadIncrease;
			
			//input.key.LumbarCtrIncrease=
			//	  input.key.LumbarCtrIncrease||input.set.LumbarHeadIncrease;  /* Union with command from OBU */
		}
		/* Get LumbarCtrDown signal */
		if(l_flg_tst_LI0_LumbarCtrDown_flag()){
			l_flg_clr_LI0_LumbarCtrDown_flag();
			input.key.LumbarCtrDown=(uint8_t)l_bool_rd_LI0_LumbarCtrDown();
			input.key.LumbarCtrDown|=input.set.LumbarHeadDown;
			
			//input.key.LumbarCtrDown=
			//	  input.key.LumbarCtrDown||input.set.LumbarHeadDown;
		}
		/* Get LumbarCtrDecrease signal */
		if(l_flg_tst_LI0_LumbarCtrDecrease_flag()){
			l_flg_clr_LI0_LumbarCtrDecrease_flag();
			input.key.LumbarCtrDecrease=(uint8_t)l_bool_rd_LI0_LumbarCtrDecrease();
			input.key.LumbarCtrDecrease|=input.set.LumbarHeadDecrease;
			
			//input.key.LumbarCtrDecrease=
			//	 input.key.LumbarCtrDecrease||input.set.LumbarHeadDecrease;
		}
		/****/
		if((input.key.LumbarCtrUp)||
		   (input.key.LumbarCtrIncrease)||
		   (input.key.LumbarCtrDown)||
		   (input.key.LumbarCtrDecrease)){
			 input.any_key=(uint8_t)1U;
		}
		else input.any_key=(uint8_t)0U;		
	}
	/* Diagnostic request frame processing */
	if(l_flg_tst_LI0_MasterReq_flag()){
		l_flg_clr_LI0_MasterReq_flag();
		
	}
	
	/* Button union block */
	/*
	input.key.LumbarCtrUp|=input.set.LumbarHeadUp;
	input.key.LumbarCtrIncrease|=input.set.LumbarHeadIncrease;
	input.key.LumbarCtrDown|=input.set.LumbarHeadDown;
	input.key.LumbarCtrDecrease|=input.set.LumbarHeadDecrease;
	if((input.key.LumbarCtrUp)||
	   (input.key.LumbarCtrIncrease)||
	   (input.key.LumbarCtrDown)||
	   (input.key.LumbarCtrDecrease)){
		 input.any_key=(uint8_t)1U;
	}
	else input.any_key=(uint8_t)0U;	
	*/
	/**********************/
	
	
	
#if(0) /* Temporary block to emulate memory toggle by massage commands */
	input.set.MemNumber=input.pne.Massage_intensity?input.pne.Massage_intensity:input.set.MemNumber;
#endif
	
	/* This is temporary code for initial testing as demo firmware */
#if(NO_POWER)//
	status=status;
#else
	if(transit){
		if(!(lumbar_st.active||massage_st.msg_on||
		     side_s_st.active||side_s_st.transit)){
			if(transit==(uint8_t)2U){
				if(!operation_mode(_OFF_,(uint8_t)0U)){
					status=transit=(uint8_t)0U;
				}	
			}
			else{
				if(!operation_mode(_ON_,(LEGACY_OFF?(uint8_t)0U:lumbar_st.memory))){ /* TODO: Attention, LEGACY_OFF macros in code !!!!!!!!!!! */
					no_ldata=latch=transit=(uint8_t)0U;
					notch=__get_millis;
					status=(uint8_t)1U;
				}
			}
		}		
	}
	else if(latch){
		if(r_flag){
			if(status)latch=(uint8_t)0U;
			else{
				transit=(uint8_t)1U;
				lumbar_st.memory=input.set.MemNumber;
			}
		}
		else if(status)transit=(uint8_t)2U;
		else if(no_ldata){
			if(ms_from(notch)>5000UL){
				no_ldata=(uint8_t)0U;
				transit=(uint8_t)2U;
			}	
		}
	}
	else{
#if(1)
		if(ms_from(notch)>5000UL){		// TODO: Hide 5000 and 100 to macro definitions, tune values. Think larger times are more suitable.
			latch=(uint8_t)1U;
		}
		else if(ms_from(notch)>=160UL){	
			r_flag=(uint8_t)0U;
			if(ms_from(tester)>=100UL){
				tester=__get_millis;
				test_lin=l_ifc_read_status(LI0);
				if(test_lin&0x08U){		/* Goto sleep command received */
					latch=(uint8_t)1U;
				}
			}	
		}
#endif
	}
	/***/
	if(latch){
		if(input.any_key){
			input.key.LumbarCtrUp=(uint8_t)0U;
			input.key.LumbarCtrIncrease=(uint8_t)0U;
			input.key.LumbarCtrDown=(uint8_t)0U;
			input.key.LumbarCtrDecrease=(uint8_t)0U;
			/* 21_02_2023 update */
			input.set.LumbarHeadUp=(uint8_t)0U;
			input.set.LumbarHeadIncrease=(uint8_t)0U;
			input.set.LumbarHeadDown=(uint8_t)0U;
			input.set.LumbarHeadDecrease=(uint8_t)0U;
			/*********************/
		}
		/* 21_02_2023 update */
		if(input.any_cmd){
			input.pne.Sidesupport_backrest=(uint8_t)0U;
			input.pne.Sidesupport_cushion=(uint8_t)0U;
		}
		/*********************/
		if(input.pne.Massage_on)input.pne.Massage_on=(uint8_t)0U;
		/* Finish active side support safely before entering sleep mode. */
		input.pne.Side_Acceleration=(int8_t)0;
	}
#endif
	/* This is temporary code for initial testing as demo firmware */
	
	return status;
}

void command_execute(void){
	static uint8_t msg_toggle=(uint8_t)0U;
	static uint8_t adj_access=(uint8_t)0U;
	static uint8_t mem_s_flag=(uint8_t)0U;
	static uint8_t acceleration_mode=(uint8_t)0U;
	static uint32_t notch=0UL;
	static bool side_support_activated = FALSE;
	uint8_t acceleration_request=
		(input.pne.Side_Acceleration==(int8_t)1)?(uint8_t)1U:(uint8_t)0U;
	if(acceleration_request)acceleration_mode=(uint8_t)1U;
	/***/
	if(input.any_key||input.any_cmd||adj_access){
		if(input.any_key||input.any_cmd){
			adj_access=(uint8_t)1U;
			notch=__get_millis;
		}
		else if(ms_from(notch)>PNEUMATIC_LOCK){
			adj_access=(uint8_t)0U;
		}
		

		
		if(lumbar_st.memory^input.set.MemNumber){
			if(!(massage_st.msg_on)){
				lumbar_st.memory=input.set.MemNumber;
				if(input.any_key||lumbar_st.active){
					lumbar_st.transit=apply_lumbar(lumbar_st.memory,(uint8_t)1U);
				}
			}	
		}
		/* 21_02_2023 update */
		if((!acceleration_mode)&&(side_s_st.memory^input.set.MemNumber)){
			if(!(massage_st.msg_on)){
				side_s_st.memory=input.set.MemNumber;
				if(input.any_cmd||side_s_st.active){
					side_s_st.transit=apply_side_support(side_s_st.memory,(uint8_t)1U);
				}
			}	
		}
		/*********************/
		
		msg_toggle=(uint8_t)0U;
		
	}
	/* Memory transit updates */
	else if(lumbar_st.memory^input.set.MemNumber){
		if(!(massage_st.msg_on))lumbar_st.memory=input.set.MemNumber; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		msg_toggle=(uint8_t)0U;
	}
	/* Memory transit updates */
	/* 21_02_2023 update */
	else if((!acceleration_mode)&&(side_s_st.memory^input.set.MemNumber)){
		if(!(massage_st.msg_on))side_s_st.memory=input.set.MemNumber; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		msg_toggle=(uint8_t)0U;
	}
	/*********************/
	else{
		if((massage_st.program^input.pne.Massage_program)||
		   (massage_st.intensity^input.pne.Massage_intensity)){
			if(massage_st.msg_on)msg_toggle=(uint8_t)0U;
			else{
				arm_msg_program(input.pne.Massage_program, input.pne.Massage_intensity);
				massage_st.program=input.pne.Massage_program;
				massage_st.intensity=input.pne.Massage_intensity;
				msg_toggle=input.pne.Massage_on;
			}
		}
		else msg_toggle=input.pne.Massage_on;
	}
	/***/
#if(0)
	if(lumbar_st.active||lumbar_st.transit){
		massage_st.msg_on=msg_run((uint8_t)0U);
	}
	else{
		massage_st.msg_on=msg_run(msg_toggle);
	}
	if(massage_st.msg_on){
		lumbar_st.active=adjust_lumbar((uint8_t)0U,
									(uint8_t)0U,
									(uint8_t)0U,
									(uint8_t)0U);
	}
	else if(!lumbar_st.transit){
		lumbar_st.active=adjust_lumbar(input.key.LumbarCtrUp,
									input.key.LumbarCtrDown,
									input.key.LumbarCtrIncrease,
									input.key.LumbarCtrDecrease);	
	}
	if(!(massage_st.msg_on||lumbar_st.active)){
		lumbar_st.transit=apply_lumbar(3,input.any_key);
	}
#else
	/* Stop a side-support memory transition before active support takes over. */
	if(acceleration_mode&&side_s_st.transit){
		side_s_st.transit=
			apply_side_support(side_s_st.memory,(uint8_t)1U);
	}
	/* Do the same for a lumbar memory transition which uses the same pump. */
	if(acceleration_mode&&lumbar_st.transit){
		lumbar_st.transit=
			apply_lumbar(lumbar_st.memory,(uint8_t)1U);
	}

	if((lumbar_st.active||lumbar_st.transit)||
	   (side_s_st.active||side_s_st.transit)||acceleration_request){
		massage_st.msg_on=msg_run((uint8_t)0U);
	}
	else{
		massage_st.msg_on=msg_run(msg_toggle);
		/* Memory save */
		if(mem_s_flag^input.set.MemSave_flag){
			mem_s_flag=input.set.MemSave_flag;
			if(mem_s_flag){
				save_lumbar(input.set.SaveNumber);
				save_side_s(input.set.SaveNumber);
			}
		}
		/***************/
	}
	if(!((massage_st.msg_on||lumbar_st.transit)||
		  (side_s_st.active||side_s_st.transit))){
		lumbar_st.active=adjust_lumbar(acceleration_request?(uint8_t)0U:input.key.LumbarCtrUp,
									   acceleration_request?(uint8_t)0U:input.key.LumbarCtrDown,
									   acceleration_request?(uint8_t)0U:input.key.LumbarCtrIncrease,
									   acceleration_request?(uint8_t)0U:input.key.LumbarCtrDecrease);
	}
	if(!((massage_st.msg_on||side_s_st.transit)||
		  (lumbar_st.active||lumbar_st.transit))){
		if(acceleration_mode){
			side_s_st.active=acceleration_side_support(acceleration_request);
		}
		else if(input.pne.Sidesupport_on){
			side_s_st.active=
				adjust_side_support(input.pne.Sidesupport_backrest,
									input.pne.Sidesupport_cushion);	
		}
		else{
			side_s_st.active=adjust_side_support((uint8_t)0U,(uint8_t)0U);	
		}
	}
	if((!acceleration_request)&&
	   !((massage_st.msg_on||lumbar_st.active)||
		 (side_s_st.transit||side_s_st.active))){
		lumbar_st.transit=apply_lumbar(lumbar_st.memory,input.any_key);
	}
	if((!acceleration_mode)&&
	   !((massage_st.msg_on||side_s_st.active)||
		 (lumbar_st.transit||lumbar_st.active))){
		if(input.pne.Sidesupport_on)
		{
			side_support_activated = TRUE;
			 side_s_st.transit=apply_side_support(side_s_st.memory,input.any_cmd);
		}
		else if (side_support_activated) 
			side_s_st.transit=apply_side_support((uint8_t)0U,(uint8_t)0U);
	}
	
#endif
	 // lumbar_st.memory
}
