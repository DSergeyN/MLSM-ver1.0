/*
 * eeprom.c
 *
 *  Created on: Nov 22, 2022
 *      Author: Nick
 */

#define EEPROM_ADDR (0x00100000UL) 	/* EEPROM memory absolute address */ 

#define COM_ERASE_S (0x1200U)		/* Command Erase Sector for FCCOB0 register */
#define COM_PROGR_W (0x1100U)		/* Command Write Words(word in this implementation) for FCCOB0 register */

#define MAX_16_ADDR (126U)			/* Maximum address for 16-bit words */

#include "eeprom.h"

/**
 * @Brief Erase EEPROM sector - 4 Byte
 */
static uint8_t erase_sector(uint8_t address){
	uint32_t addr;
	addr=((uint32_t)address/(uint32_t)4U)*((uint32_t)4U);
	addr|=EEPROM_ADDR;
	for(;FSTAT&((uint8_t)0x08U);){};
	FSTAT=(uint8_t)0x30U;
	FCCOBIX=(uint8_t)0x01U;
	FCCOB0=COM_ERASE_S|(uint16_t)(addr>>((uint32_t)16U));
	FCCOB1=(uint16_t)(addr&(uint32_t)0x0000FFFFU);
	FSTAT|=(uint8_t)0x80U;
	for(;!(FSTAT&((uint8_t)0x80U));){};
	if(FSTAT&(uint8_t)0x33U)return (uint8_t)1U;
	return (uint8_t)0U;
}

/**
 * @Brief Read 16-bit word 
 */
EEPROM_err_t read_16w(uint8_t address, uint16_t* data){
	uint16_t* addr;
	if(((uint16_t)address%2U)||
	   ((uint16_t)address>MAX_16_ADDR))return ERR_address;
	addr=(uint16_t*)(EEPROM_ADDR|(uint32_t)address);
	for(;FSTAT&((uint8_t)0x08U);){};	
	*data=*addr;
	return (uint8_t)0U;
}

/**
 * @Brief Write 16-bit word, includes sector erase and restore neighbour data if required
 */
EEPROM_err_t write_16w(uint8_t address, uint16_t data){
	uint32_t addr;
	uint16_t buff;
	 uint8_t sadd;
	if((((uint16_t)address)%2U)||
	   (((uint16_t)address)>MAX_16_ADDR))return ERR_address;
	addr=EEPROM_ADDR|(uint32_t)address;
	read_16w(address,&buff);
	if(buff!=0xFFFFU){
		if((uint16_t)address%4U){
			 sadd=address-(uint8_t)2U;
		}
		else sadd=address+(uint8_t)2U;
		read_16w(sadd,&buff);
		if(erase_sector(address))return ERR_erase;
	}
	/* Write target word */
	for(;0x01U;){
		for(;FSTAT&((uint8_t)0x08U);){};
		FSTAT=(uint8_t)0x30U;
		FCCOBIX=(uint8_t)0x02U;
		FCCOB0=COM_PROGR_W|(uint16_t)(addr>>((uint32_t)16U));
		FCCOB1=(uint16_t)(addr&(uint32_t)0x0000FFFFU);
		FCCOB2=data;
		FSTAT|=(uint8_t)0x80U;
		for(;!(FSTAT&((uint8_t)0x80U));){};
		if(FSTAT&(uint8_t)0x33U)return ERR_write;
		/* Restore neighbour word data - cycle */
		if(buff!=0xFFFFU){
			addr=EEPROM_ADDR|(uint32_t)sadd;
			data=buff;
			buff=0xFFFFU;
		}
		else break;
	}	
	return (uint8_t)0U;
}
