/*
 * control.h
 *
 *  Created on: Nov 28, 2022
 *      Author: Nick
 */

#ifndef __CONTROL__
#define __CONTROL__

#include "lin.h"
#if _TL_FRAME_SUPPORT_ == _TL_MULTI_FRAME_
#include "lin_lin21tl_api.h"
#endif

#include "system.h"
#include "pump.h"
#include "valve.h"
#include "timer.h"

/* Temporary */
#include "eeprom.h"
/*************/

#define NO_POWER (0)

uint8_t command_extract(void);
void command_execute(void);

#endif /* __CONTROL__ */
