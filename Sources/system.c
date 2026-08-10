/*
 * system.c
 *
 *  Created on: Nov 8, 2022
 *      Author: Nick
 */

#define _WHY_ /* Because why cannot fly */

#include "system.h"

/**
 * @Brief Initialise CPU core to run 12.5 MHz with 6.25 MHz bus clock.
 */
void cpu_init(void){
	/* ### MC9S12ZVL32_48 "Cpu" init code ... */
	/* Initialisation code after reset */
	/* IVBR: IVB_ADDR=0x7FFF,??=0 */
	setReg16(IVBR, 0xFFFEU);              
	/* ECLKCTL: NECLK=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
	setReg8(ECLKCTL, 0x80U);              
	/*  System clock initialisation */
	/* CPMUPROT: ??=0,??=0,??=1,??=0,??=0,??=1,??=1,PROT=0 */
	setReg8(CPMUPROT, 0x26U);            /* Disable protection of clock configuration registers */ 
	/* CPMUCLKS: PSTP=0 */
	clrReg8Bits(CPMUCLKS, 0x40U);         
	/* CPMUCLKS: PLLSEL=1 */
	setReg8Bits(CPMUCLKS, 0x80U);        /* Enable the PLL to allow write to divider registers */ 
	/* CPMUPOSTDIV: ??=0,??=0,??=0,POSTDIV4=0,POSTDIV3=0,POSTDIV2=0,POSTDIV1=1,POSTDIV0=1 */
	setReg8(CPMUPOSTDIV, 0x03U);         /* Set the post divider register */ 
	/* Whenever changing PLL reference clock (REFCLK) frequency to a higher value
	it is recommended to write CPMUSYNR = 0x00 in order to stay within specified
	maximum frequency of the MCU */
	/* CPMUSYNR: VCOFRQ1=0,VCOFRQ0=0,SYNDIV5=0,SYNDIV4=0,SYNDIV3=0,SYNDIV2=0,SYNDIV1=0,SYNDIV0=0 */
	setReg8(CPMUSYNR, 0x00U);            /* Set the multiplier register */ 
	/* CPMUPLL: ??=0,??=0,FM1=0,FM0=0,??=0,??=0,??=0,??=0 */
	setReg8(CPMUPLL, 0x00U);             /* Set the PLL frequency modulation */ 
	/* CPMUSYNR: VCOFRQ1=0,VCOFRQ0=1,SYNDIV5=0,SYNDIV4=1,SYNDIV3=1,SYNDIV2=0,SYNDIV1=0,SYNDIV0=0 */
	setReg8(CPMUSYNR, 0x58U);            /* Set the multiplier register */ 
	while(CPMUIFLG_LOCK == 0U) {         /* Wait until the PLL is within the desired tolerance of the target frequency */
	}
	/* CPMUPROT: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,PROT=0 */
	setReg8(CPMUPROT, 0x00U);            /* Enable protection of clock configuration registers */ 
	/* CPMUCOP: RSBCK=0,WRTMASK=1 */
	clrSetReg8Bits(CPMUCOP, 0x40U, 0x20U); 
	/* CPMUHTCTL: ??=0,??=0,VSEL=1,??=0,HTE=1,HTDS=0,HTIE=1,HTIF=0 */
	setReg8(CPMUHTCTL, 0x2AU);    
	/* CPMUVREGCTL: ??=0,??=0,??=0,??=0,??=0,??=0,EXTXON=0,INTXON=1 */
	setReg8(CPMUVREGCTL, 0x01U);          
	/*** End of initialisation code after reset ***/	
}

/**
 * @Brief Configure device interrupt priorities.
 */
void set_irq_priority(void){
#if(0)
	setReg8(INT_CFADDR, 0x70U);     /*  Address   Priority   Periphery             */
	setReg8(INT_CFDATA4, 0x01U);	/* 0x00FFFFD0 	 1 	      ivVrti               */ 
	setReg8(INT_CFADDR, 0x50U);     /*      									   */
	setReg8(INT_CFDATA0, 0x05U);    /* 0x00FFFF40    5        ivVbats              */
	setReg8(INT_CFDATA1, 0x02U);    /* 0x00FFFF44    2        ivVlinphy0           */ 
	setReg8(INT_CFADDR, 0x68U);     /*     										   */
	setReg8(INT_CFDATA0, 0x04U);	/* 0x00FFFFA0    4        ivVspi0              */ 
	setReg8(INT_CFADDR, 0x60U);     /*											   */     
	setReg8(INT_CFDATA1, 0x03U);	/* 0x00FFFF84    3        ivVadc0conv_compl    */ 
	setReg8(INT_CFDATA2, 0x03U);	/* 0x00FFFF88    3        ivVadc0conv_seq_abrt */ 
	setReg8(INT_CFDATA3, 0x03U);	/* 0x00FFFF8C    3        ivVadc0err           */ 
	setReg8(INT_CFDATA7, 0x06U);	/* 0x00FFFF9C    6        ivVsci0              */ 
#else	
	setReg8(INT_CFADDR, 0x70U);     /*  Address   Priority   Periphery             */
	setReg8(INT_CFDATA4, 0x01U);	/* 0x00FFFFD0 	 1 	      ivVrti               */ 
	setReg8(INT_CFADDR, 0x50U);     /*      									   */
	setReg8(INT_CFDATA1, 0x02U);    /* 0x00FFFF44    2        ivVlinphy0           */ 
	setReg8(INT_CFADDR, 0x60U);     /*											   */     
	setReg8(INT_CFDATA1, 0x03U);	/* 0x00FFFF84    3        ivVadc0conv_compl    */ 
	setReg8(INT_CFDATA2, 0x03U);	/* 0x00FFFF88    3        ivVadc0conv_seq_abrt */ 
	setReg8(INT_CFDATA3, 0x03U);	/* 0x00FFFF8C    3        ivVadc0err           */ 
	setReg8(INT_CFADDR, 0x50U);     /*      									   */
	setReg8(INT_CFDATA0, 0x04U);    /* 0x00FFFF40    4        ivVbats              */
	setReg8(INT_CFADDR, 0x68U);     /*     										   */
	setReg8(INT_CFDATA0, 0x05U);	/* 0x00FFFFA0    5        ivVspi0              */ 
	setReg8(INT_CFADDR, 0x28U);           
	setReg8(INT_CFDATA3, 0x05U);    /* 0x00FFFEAC    5        ivVtim1ch0           */ 
	setReg8(INT_CFADDR, 0x60U);     /*											   */
	setReg8(INT_CFDATA7, 0x06U);	/* 0x00FFFF9C    6        ivVsci0              */ 
#endif	
	return;
}

/**
 * @Brief Configure mS System Timer.
 */
void rti_init(void){
	/* CPMUINT: RTIE=0 */
	clrReg8Bits(CPMUINT, 0x80U);          
	/* CPMUIFLG: RTIF=1,??=0,??=0,LOCKIF=0,LOCK=0,??=0,OSCIF=0,UPOSC=0 */
	setReg8(CPMUIFLG, 0x80U);             
	/* CPMUPROT: ??=0,??=0,??=1,??=0,??=0,??=1,??=1,PROT=0 */
	setReg8(CPMUPROT, 0x26U);            /* Disable protection of clock-source register */ 
	/* CPMUCLKS: PRE=0,RTIOSCSEL=0 */
	clrReg8Bits(CPMUCLKS, 0x0AU);         
	/* CPMUPROT: ??=0,??=0,??=0,??=0,??=0,??=0,??=0,PROT=0 */
	setReg8(CPMUPROT, 0x00U);            /* Re-Enable protection of clock-source register */ 
	/* CPMUINT: RTIE=1 */
	setReg8Bits(CPMUINT, 0x80U);          
	/* CPMURTI: RTDEC=1,RTR6=0,RTR5=0,RTR4=0,RTR3=0,RTR2=0,RTR1=0,RTR0=0 */
	setReg8(CPMURTI, 0x80U);  
	return;	
}

/*
 * @Brief Configure Pin routing and other Port features.
 */
void port_init(void){
	/* Common initialisation of the CPU registers */
	/* MODRR0: S0L0RR&=~3 */
	clrReg8Bits(MODRR0, 0x03U);           
	/* WOMS: WOMS2=0,WOMS1=0,WOMS0=0 */
	clrReg8Bits(WOMS, 0x07U);             
	/* MODRR1: PWM2RR=1 */
	setReg8Bits(MODRR1, 0x04U); 
	/* MODRR1: T0C3RR=1 Disable TIM0 influence by PT3 pin 14.12.2022 */
	MODRR2|=(uint8_t)0x08U;
	/* CPMUINT: LOCKIE=0,OSCIE=0 */
	clrReg8Bits(CPMUINT, 0x12U);          
	/* CPMULVCTL: LVIE=0 */
	clrReg8Bits(CPMULVCTL, 0x02U);        
	/* ECCIE: SBEEIE=0 */
	clrReg8Bits(ECCIE, 0x01U);            
	/* ECCDCMD: ECCDRR=0 */
	clrReg8Bits(ECCDCMD, 0x80U);          
	/* RDRP: RDRP7=0,RDRP5=0,RDRP3=0,RDRP1=0 */
	clrReg8Bits(RDRP, 0xAAU);             

	for(;!(FSTAT&((uint8_t)0x80U));){};
	FCLKDIV=0x05U; 

#ifdef _WHY_
	/* IRQCR: IRQEN=0 */
	clrReg8Bits(IRQCR, 0x40U);            /* Actually remove - we don't use EXIRQ in no way */
	/* ### MC9S12ZVL32_48 "Cpu" init code ... */
#endif
	return;
}

/**
 * @Brief Initialise LIN Physical interface.
 */
void lin_init(void){
	/* LP0CR: ??=0,??=0,??=0,??=0,LPE=0,RXONLY=0,LPWUE=0,LPPUE=0 */
	setReg8(LP0CR, 0x00U);                
	if (LP0IF & 0x80U) {                 /* Is LPDTIF flag set? */
		/* LP0IF: LPDTIF=1,LPOCIF=0,??=0,??=0,??=0,??=0,??=0,??=0 */
		setReg8(LP0IF, 0x80U);            
	}
	if (LP0IF & 0x40U) {                 /* Is LPOCIF flag set? */
		/* LP0IF: LPDTIF=0,LPOCIF=1,??=0,??=0,??=0,??=0,??=0,??=0 */
		setReg8(LP0IF, 0x40U);            
	}
	/* LP0DR: ??=0,??=0,??=0,??=0,??=0,??=0,LPDR1=1,LPDR0=1 */
	setReg8(LP0DR, 0x03U);                
	/* LP0SLRM: LPDTDIS=0,LPSLR=0 */
	clrReg8Bits(LP0SLRM, 0x83U);          
	/* LP0IE: LPDTIE=0,LPOCIE=0 */
	clrReg8Bits(LP0IE, 0xC0U);            
	/* LP0SLRM: LPDTDIS=0 */
	clrReg8Bits(LP0SLRM, 0x80U);
	/* LP0CR: ??=0,??=0,??=0,??=0,LPE=1,RXONLY=0,LPWUE=0,LPPUE=0 */
	setReg8(LP0CR, 0x08U); 
	return;
}

/**
 * @Brief Initialise PWM for air pump control.
 */
void pwm_init(void){
	/* PWME: PWME7=0,PWME6=0,PWME5=0,PWME4=0,PWME3=0,PWME2=0,PWME1=0,PWME0=0 */
	setReg8(PWME, 0x00U);                /* Disable all PWM channels */ 
	/* PWMPOL: PPOL7=0,PPOL6=0,PPOL5=0,PPOL4=0,PPOL3=0,PPOL2=1,PPOL1=0,PPOL0=0 */
	setReg8(PWMPOL, 0x04U);               
	/* PWMCLK: PCLK7=0,PCLK6=0,PCLK5=0,PCLK4=0,PCLK3=0,PCLK2=0,PCLK1=0,PCLK0=0 */
	setReg8(PWMCLK, 0x00U);               
	/* PWMCLKAB: PCLKAB7=0,PCLKAB6=0,PCLKAB5=0,PCLKAB4=0,PCLKAB3=0,PCLKAB2=1,PCLKAB1=0,PCLKAB0=0 */
	setReg8(PWMCLKAB, 0x04U);             
	/* PWMCAE: CAE7=0,CAE6=0,CAE5=0,CAE4=0,CAE3=0,CAE2=0,CAE1=0,CAE0=0 */
	setReg8(PWMCAE, 0x00U);               
	/* PWMCTL: CON67=0,CON45=0,CON23=0,CON01=0,PSWAI=0,PFRZ=0,??=0,??=0 */
	setReg8(PWMCTL, 0x00U);               
	/* PWMDTY2: PWMDTY2=0 */
	setReg8(PWMDTY2, 0x00U);              
	/* PWMPER2: PWMPER2=0xFF */
	setReg8(PWMPER2, 0xFFU);              
	/* PWMSCLA: BIT7=0,BIT6=0,BIT5=0,BIT4=0,BIT3=0,BIT2=0,BIT1=0,BIT0=0 */
	setReg8(PWMSCLA, 0x00U);              
	/* PWMSCLB: BIT7=0,BIT6=0,BIT5=0,BIT4=0,BIT3=0,BIT2=0,BIT1=0,BIT0=0 */
	setReg8(PWMSCLB, 0x00U);              
	/* PWMPRCLK: PCKB2=0,PCKB1=0,PCKB0=0,PCKA2=0,PCKA1=0,PCKA0=0 */
	setReg8(PWMPRCLK, 0x00U);             
	/* PWME: PWME7=0,PWME6=0,PWME5=0,PWME4=0,PWME3=0,PWME2=1,PWME1=0,PWME0=0 */
	setReg8(PWME, 0x04U);                /* Enable only configured PWM channels */ 
	return;
}

/**
 * @Brief GPIO peripheral initialisation.
 * @Scope 1) PORT ADL - PAD1 - Pin 13: VSUP_M_ENABLE - output - initial LOW;
 * 		  2) PORT P   - PP4 -- Pin 32: VDD_DRV_EN ---- output - initial LOW;
 *			 		  - PP6 -- Pin 18: CS_DIS_PWM ---- output - initial LOW;
 * 		  			  - PP7 -- Pin 44: VDD_PR_S ------ output - initial LOW;
 * 		  3) PORT T   - PT1 -- Pin 27: CS_SD_MSG1 ---- output - initial HIGH;
 * 		  			  - PT2 -- Pin 40: CS_LMB -------- output - initial HIGH;
 * 		  			  - PT3 -- Pin 43: CS_SD_MSG2 ---- output - initial HIGH.		  			  
 */
void gpio_init(void){
	/*
	 * PORT ADL configuration
	 */
	/* PIEADL: PIEADL7=0,PIEADL6=0,PIEADL5=0,PIEADL4=0,PIEADL3=0,PIEADL2=0,PIEADL1=0,PIEADL0=0 */
	setReg8(PIEADL, 0x00U);               
	/* PIFADL: PIFADL7=1,PIFADL6=1,PIFADL5=1,PIFADL4=1,PIFADL3=1,PIFADL2=1,PIFADL1=1,PIFADL0=1 */
	setReg8(PIFADL, 0xFFU);               
	/* PTADL: PTADL1=0 */
	clrReg8Bits(PTADL, 0x02U);            
	/* DDRADL: DDRADL1=1 */
	setReg8Bits(DDRADL, 0x02U);
	/*
	 * PORT P configuration
	 */
	/* PIEP: PIEP7=0,PIEP6=0,PIEP5=0,PIEP4=0,PIEP3=0,PIEP2=0,PIEP1=0,PIEP0=0 */
	setReg8(PIEP, 0x00U);                 
	/* OCIEP: OCIEP7=0,OCIEP5=0,OCIEP3=0,OCIEP1=0 */
	clrReg8Bits(OCIEP, 0xAAU);            
	/* PIFP: PIFP7=1,PIFP6=1,PIFP5=1,PIFP4=1,PIFP3=1,PIFP2=1,PIFP1=1,PIFP0=1 */
	setReg8(PIFP, 0xFFU);                 
	/* OCIFP: OCIFP7=1,OCIFP5=1,OCIFP3=1,OCIFP1=1 */
	setReg8Bits(OCIFP, 0xAAU);            
	/* PTP: PTP7=0,PTP6=0,PTP4=0 */
	clrReg8Bits(PTP, 0xD0U);              
	/* DDRP: DDRP7=1,DDRP6=1,DDRP4=1 */
	setReg8Bits(DDRP, 0xD0U);
	/*
	 * PORT T configuration
	 */
	/* PTT: PTT3=1,PTT2=1,PTT1=1 */
	setReg8Bits(PTT, 0x0EU);              
	/* DDRT: DDRT3=1,DDRT2=1,DDRT1=1 */
	setReg8Bits(DDRT, 0x0EU); 
	return;
}

/**
 * @Brief Initialise SPI interface for valve control.
 */
void spi_init(void){
	/* SPI0CR1: SPIE=0,SPE=0,SPTIE=0,MSTR=0,CPOL=0,CPHA=0,SSOE=0,LSBFE=0 */
	setReg8(SPI0CR1, 0x00U);             /* Disable the SPI0 module and clearing flags in SPISR register */ 
	/* SPI0CR2: ??=0,XFRW=1,??=0,MODFEN=0,BIDIROE=0,??=0,SPISWAI=0,SPC0=0 */
	setReg8(SPI0CR2, 0x40U);              
	/* SPI0BR: ??=0,SPPR2=0,SPPR1=0,SPPR0=0,??=0,SPR2=0,SPR1=1,SPR0=0 */
	setReg8(SPI0BR, 0x02U);               
	/* SPI0CR1: SPIE=1,SPE=1,SPTIE=0,MSTR=1,CPOL=1,CPHA=1,SSOE=0,LSBFE=1 */
	setReg8(SPI0CR1, 0x5DU); //DD
	return;
}

void timer_init(void){
	/* TIM1TSCR1: TEN=0 */
	clrReg8Bits(TIM1TSCR1, 0x80U);        
	/* TIM1TIE: ??=0,??=0,??=0,??=0,??=0,??=0,C1I=0,C0I=0 */
	setReg8(TIM1TIE, 0x00U);              
	/* TIM1TIOS: ??=0,??=0,??=0,??=0,??=0,??=0,IOS1=0,IOS0=1 */
	setReg8(TIM1TIOS, 0x01U);             
	/* TIM1TC0: BIT=0x20 */
	setReg16(TIM1TC0, 0x14U);             
	/* TIM1OCPD: ??=0,??=0,??=0,??=0,??=0,??=0,OCPD1=0,OCPD0=1 */
	setReg8(TIM1OCPD, 0x03U); /* Was 0x01U - 14.12.2022 */         
	/* TIM1TTOV: ??=0,??=0,??=0,??=0,??=0,??=0,TOV1=0,TOV0=0 */
	setReg8(TIM1TTOV, 0x00U);             
	/* TIM1TCTL2: ??=0,??=0,??=0,??=0,OM1=0,OL1=0,OM0=0,OL0=0 */
	setReg8(TIM1TCTL2, 0x00U);            
	/* TIM1TCTL4: ??=0,??=0,??=0,??=0,EDG1B=0,EDG1A=0,EDG0B=0,EDG0A=0 */
	setReg8(TIM1TCTL4, 0x00U);            
	/* TIM1TFLG1: ??=0,??=0,??=0,??=0,??=0,??=0,C1F=1,C0F=1 */
	setReg8(TIM1TFLG1, 0x03U);            
	/* TIM1TFLG2: TOF=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
	setReg8(TIM1TFLG2, 0x80U);            
	/* TIM1TIE: ??=0,??=0,??=0,??=0,??=0,??=0,C1I=0,C0I=1 */
	setReg8(TIM1TIE, 0x01U);              
	/* TIM1TSCR2: TOI=0,??=0,??=0,??=0,??=0,PR2=0,PR1=1,PR0=1 */
	setReg8(TIM1TSCR2, 0x03U);            
	/* TIM1PTPSR: PTPS7=0,PTPS6=0,PTPS5=0,PTPS4=0,PTPS3=0,PTPS2=0,PTPS1=0,PTPS0=0 */
	setReg8(TIM1PTPSR, 0x00U);            
	/* TIM1TSCR1: TEN=1,TSWAI=0,TSFRZ=0,TFFCA=0,PRNT=0,??=0,??=0,??=0 */
	setReg8(TIM1TSCR1, 0x80U);  
	return;
}

/**
 * @Scope Battery sense initialisation with following thresholds:
 * 		  1) High Voltage Warning (HBI 1) ~ 16.5V;
 * 		  2) Low Voltage Warning (LBI 4)  ~ 9.5V.
 */
void bats_init(void){
	/* BATIF: ??=0,??=0,??=0,??=0,??=0,??=0,BVHIF=1,BVLIF=1 */
	setReg8(BATIF, 0x03U);                
	/* BATE: ??=0,BVHS=0,BVLS=3,BSUAE=0,BSUSE=1 */
	clrSetReg8Bits(BATE, 0xC8U, 0x34U); 
	/* BATIE: ??=0,??=0,??=0,??=0,??=0,??=0,BVHIE=1,BVLIE=1 */
	setReg8(BATIE, 0x03U);  
	return;
}

/**
 * @Brief Configure VREG to operate in internal mode with 5V output
 */
void vreg_conf(void){
	CPMUVREGCTL=(uint8_t)0x81U; /* Set VREG5VEN - enable 5V LDO mode, EXTXON/INTXON (0/1) - external BJT transistor isn't used */
	return;
}

/**
 * @Brief ADC initialisation for analogue inputs sensing.
 */
void adc_init(void){
	/* ADC0CTL: ADC_EN=0,ADC_SR=0,FRZ_MOD=0,SWAI=0,ACC_CFG=0,STR_SEQA=0,MOD_CFG=0,CSL_BMOD=0,RVL_BMOD=0,SMOD_ACC=0,AUT_RSTA=0,??=0,??=0,??=0,??=0 */
	setReg16(ADC0CTL, 0x00U);             
	/* ADC0FMT: DJM=1,??=0,??=0,??=0,??=0,SRES=2 */
	setReg8(ADC0FMT, 0x82U);   //           
	/* ADC0TIM: ??=0,PRS=0x1F */
	setReg8(ADC0TIM, 0x1FU);              
	/* ADC0CTL: FRZ_MOD=0,SWAI=0,ACC_CFG=0,STR_SEQA=0,MOD_CFG=1,CSL_BMOD=0,RVL_BMOD=0,SMOD_ACC=0,AUT_RSTA=0,??=0,??=0,??=0,??=0 */
	clrSetReg16Bits(ADC0CTL, 0x3EFFU, 0x0100U); 
	/* ADC0STS: CSL_SEL=0,RVL_SEL=0,DBECC_ERR=0,??=0,READY=1,??=0,??=0,??=0 */
	setReg8(ADC0STS, 0x08U);
#ifdef __DO_IT_NOW__
	ADC0RBP = (void *)0;
	ADC0CBP = (void *)0;
#endif
	/* ADC0CROFF1: ??=0,CMDRES_OFF1=0 */
	setReg8(ADC0CROFF1, 0x00U);           
	/* ADC0EIE: IA_EIE=1,CMD_EIE=0,EOL_EIE=0,??=0,TRIG_EIE=0,RSTAR_EIE=0,LDOK_EIE=0,??=0 */
	
#ifdef DEBUG
	setReg8(ADC0EIE, 0x0);    //0xFEU          /* Fine tuning required, probably, will be used only in development stage !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  was 0x80 */
	/* ADC0IE: SEQAD_IE=1,CONIF_OIE=0,??=0,??=0,??=0,??=0,??=0,??=0 */
	setReg8(ADC0IE, 0x80U);     //0xC0U          /* Fine tuning required, probably, will be used only in development stage !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! was 0x80 */
#else 
	setReg8(ADC0EIE, 0xFEU);    //0xFEU          /* Fine tuning required, probably, will be used only in development stage !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  was 0x80 */
	/* ADC0IE: SEQAD_IE=1,CONIF_OIE=0,??=0,??=0,??=0,??=0,??=0,??=0 */
	setReg8(ADC0IE, 0xC0U);     //0xC0U          /* Fine tuning required, probably, will be used only in development stage !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! was 0x80 */	
#endif
	
	
	
	/* ADC0CONIE: CON_IE15=0,CON_IE14=0,CON_IE13=0,CON_IE12=0,CON_IE11=0,CON_IE10=0,CON_IE9=0,CON_IE8=0,CON_IE7=0,CON_IE6=0,CON_IE5=0,CON_IE4=0,CON_IE3=0,CON_IE2=0,CON_IE1=1,EOL_IE=0 */
	setReg16(ADC0CONIE, 0x03U);           /* CLS will consist from two groups of 4 similar channels, finish of the first group conversion will release 0x02 interrupt flag
	 	 	 	 	 	 	 	 	 	 	 and second group will release 0x01 EOL_IF !!!!! was 0x02U*/
#ifdef __DO_IT_NOW__
	/* ADC0CTL: ADC_EN=0 */
	clrReg16Bits(ADC0CTL, 0x8000U);  
#endif
	return;
}

/**
 * @Brief Repairs LIN stack - suppresses TIM0 output compare on Ch-2,
 *        this feature will be disturbed by LIN stack initialisation.
 *        Function isolates PT2 from the timer. Others too 30.11.22.
 */
void block_lin_PT2(void){
	TIM0OCPD|=(uint8_t)0x3FU; //0x04U;
	return;
}



