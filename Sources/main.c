
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "lin.h"
#if _TL_FRAME_SUPPORT_ == _TL_MULTI_FRAME_
#include "lin_lin21tl_api.h"
#endif
#include "system.h"

#include "pump.h"
#include "valve.h"
#include "timer.h"
#include "analog.h"
#include "gpio.h"
#include "eeprom.h"
#include "control.h"

int16_t a;
int16_t cs;
int8_t p;
uint32_t tim_test;

void main(void) {
	
	l_u8 ret;
	uint32_t timer=0U;
	uint8_t memory=(uint8_t)4U;
	uint8_t mem_sds=(uint8_t)20U;

	cpu_init();
	set_irq_priority();
	rti_init();
	port_init();
	lin_init();
	pwm_init();
	gpio_init();
	spi_init();
	timer_init();
	vreg_conf();
	ms_halt_com(VREG_STABLE);
	bats_init();
	adc_init();
	programm_adc();
	EnableInterrupts;
	
	ms_halt(50);
	
	VDD_MON_TOGGLE(_ON_);
	VDD_DRV_TOGGLE(_ON_);
	CNS_DIS_TOGGLE(_OFF_);
	VDD_PSN_TOGGLE(_ON_);
	
	//PTP|=(uint8_t)0x90U;
	
    ret = l_sys_init();
    ret = l_ifc_init(LI0);
      
    block_lin_PT2();
    arm_msg_program(0, 0); /* Should be 0/0 */
    
	ms_halt(50);

#if(1)
	plug_p_offset(_TRIG_);
	for(;plug_p_offset(_RUN_);){};
#else
	timer=__get_millis;
	for(;ms_from(timer)<(uint32_t)1000U;){
		scan_p_offset();
	}
#endif	
	//timer_init(); /* Again here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

    ms_halt(50);
    
    read_lumbar();
    read_side_s();
    
    ms_halt(50);

/* Lumbar memory tests */
#if(0)
    /* Memory #1 */
    write_16w(memory,((37U<<8U)|(0U)));
    memory+=(uint8_t)2U;
    write_16w(memory,((25U<<8U)|(37U)));
    memory+=(uint8_t)2U;
    /* Memory #2 */
    write_16w(memory,((44U<<8U)|(0U)));
    memory+=(uint8_t)2U;
    write_16w(memory,((44U<<8U)|(0U)));
    memory+=(uint8_t)2U;
    /* Memory #3 */
    write_16w(memory,((28U<<8U)|(25U)));
    memory+=(uint8_t)2U;
    write_16w(memory,((17U<<8U)|(0U)));
#endif

/* Side support memory tests */
#if(0)
    /* Memory #1 */
    write_16w(mem_sds,((0U<<8U)|(50U)));
    /* Memory #2 */
    mem_sds+=(uint8_t)2U;
    write_16w(mem_sds,((25U<<8U)|(25U)));
    /* Memory #3 */
    mem_sds+=(uint8_t)2U; 
    write_16w(mem_sds,((0U<<8U)|(50U)));

#endif
       
	for(;;) {
				
#if(NO_POWER)		
		scan_p_offset(_RUN_);
		a=supply_voltage();
		p=air_pressure();
		cs=pump_current();
#endif
		
		
#if(NO_POWER)
		command_extract();
		command_execute();
#else
		/*
		MSG_1R_BAG_2(1);
		MSG_2L_BAG_3(1);
		
		a=msg_2L.feed_cmd;
		cs=msg_2L.valve.bag_3;
		
		for(;0x01U;){};
		*/
		
		if(command_extract()){
			scan_p_offset(_RUN_);
			command_execute();
		}
		
#endif
		
	  
	__RESET_WATCHDOG();	/* feeds the dog */
	
	} /* loop forever */
	/* please make sure that you never leave main */

}
