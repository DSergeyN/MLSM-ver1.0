/******************************************************************************
*
* Freescale Semiconductor Inc.
* (c) Copyright 2013-2016 Freescale Semiconductor, Inc.
* Copyright 2016-2023 NXP
* ALL RIGHTS RESERVED.
*
****************************************************************************//*!
*
* @file      lin_cfg.h
*
* @author    FPT Software
*
* @version   1.0
*
* @date      Wed Jun 07 11:14:32 MSK 2023
*
* @brief     Hardware configuration file
*
******************************************************************************/
#ifndef    _LIN_CFG_H_
#define    _LIN_CFG_H_
#include "lin_hw_cfg.h"
/* Define operating mode */
#define _MASTER_MODE_     0
#define _SLAVE_MODE_      1
#define LIN_MODE   _SLAVE_MODE_
/* Define protocol version */
#define PROTOCOL_21       0
#define PROTOCOL_J2602    1
#define PROTOCOL_20       2
#define LIN_PROTOCOL    PROTOCOL_21

#define SCI_ADDR        SCI0_ADDR    /* For slave */

#define LIN_BAUD_RATE    19200    	 /*For slave*/
/**********************************************************************/
/***************          Diagnostic class selection  *****************/
/**********************************************************************/
#define _DIAG_CLASS_I_          0
#define _DIAG_CLASS_II_         1
#define _DIAG_CLASS_III_        2

#define _DIAG_CLASS_SUPPORT_    _DIAG_CLASS_I_

#define MAX_LENGTH_SERVICE 6

#define MAX_QUEUE_SIZE 1


#define _DIAG_NUMBER_OF_SERVICES_    3

#define DIAGSRV_READ_BY_IDENTIFIER_ORDER    0

#define DIAGSRV_ASSIGN_FRAME_ID_RANGE_ORDER    1

#define DIAGSRV_SAVE_CONFIGURATION_ORDER    2


/**************** FRAME SUPPORT DEFINITION ******************/
#define _TL_SINGLE_FRAME_       0
#define _TL_MULTI_FRAME_        1

#define _TL_FRAME_SUPPORT_      _TL_SINGLE_FRAME_

/* frame buffer size */
#define LIN_FRAME_BUF_SIZE			17
#define LIN_FLAG_BUF_SIZE			10

/**********************************************************************/
/***************               Interfaces           *******************/
/**********************************************************************/
typedef enum {
   LI0
}l_ifc_handle;

/**********************************************************************/
/***************               Signals              *******************/
/**********************************************************************/
/* Number of signals */
#define LIN_NUM_OF_SIGS  30
/* List of signals */
typedef enum {

   /* Interface_name = LI0 */

   LI0_MemSave_flag

   , LI0_MemNumber
  
   , LI0_BCM_EEM
  
   , LI0_sigSSWLinError
  
   , LI0_Seat_Id
  
   , LI0_Seat_Var
  
   , LI0_LumbarHeadUp
  
   , LI0_LumbarHeadIncrease
  
   , LI0_LumbarHeadDown
  
   , LI0_LumbarHeadDecrease
  
   , LI0_CabinAmbientTemp
  
   , LI0_Massage_on
  
   , LI0_Massage_intensity
  
   , LI0_Massage_program
  
   , LI0_Sidesupport_on
  
   , LI0_Sidesupport_backrest
  
   , LI0_Sidesupport_cushion
  
   , LI0_Int_Diagnostics
  
   , LI0_Side_Acceleration
  
   , LI0_sigpNsystemLinError
  
   , LI0_Diagnostics_Code
  
   , LI0_DTCs_Pending
  
   , LI0_Diagnostics_Ctr
  
   , LI0_Data_Register
  
   , LI0_LumbarCtrUp
  
   , LI0_LumbarCtrIncrease
  
   , LI0_LumbarCtrDown
  
   , LI0_LumbarCtrDecrease
  
   , LI0_Upproj
  
   , LI0_startaddr
  
} l_signal_handle;
/**********************************************************************/
/*****************               Frame             ********************/
/**********************************************************************/
/* Number of frames */
#define LIN_NUM_OF_FRMS  8
/* List of frames */
typedef enum {
/* All frames for master node */

   /* Interface_name = LI0 */

   LI0_SMM_brdcst

   , LI0_pNsystem_ctr
  
   , LI0_pNsystem_ans
  
   , LI0_Seatsetting
  
   , LI0_Ubootbrd
  
   , LI0_staddrbrd
  
   , LI0_MasterReq
  
   , LI0_SlaveResp
  
} l_frame_handle;
/**********************************************************************/
/***************             Configuration          *******************/
/**********************************************************************/
/* Size of configuration in ROM and RAM used for interface: LI1 */
#define LIN_SIZE_OF_CFG  10
#define LIN_CFG_FRAME_NUM  6
/*********************************************************************
 * global macros
 *********************************************************************/
#define l_bool_rd(SIGNAL) l_bool_rd_##SIGNAL()
#define l_bool_wr(SIGNAL, A) l_bool_wr_##SIGNAL(A)
#define l_u8_rd(SIGNAL) l_u8_rd_##SIGNAL()
#define l_u8_wr(SIGNAL, A) l_u8_wr_##SIGNAL(A)
#define l_u16_rd(SIGNAL) l_u16_rd_##SIGNAL()
#define l_u16_wr(SIGNAL, A) l_u16_wr_##SIGNAL(A)
#define l_bytes_rd(SIGNAL, start, count, data)  l_bytes_rd_##SIGNAL(start, count, data)
#define l_bytes_wr(SIGNAL, start, count, data) l_bytes_wr_##SIGNAL(start, count, data)
#define l_flg_tst(FLAG) l_flg_tst_##FLAG()
#define l_flg_clr(FLAG) l_flg_clr_##FLAG()
#define LIN_TEST_BIT(A,B) ((l_bool)((((A) & (1U << (B))) != 0U) ? 1U : 0U))
#define LIN_SET_BIT(A,B)                      ((A) |= (l_u8) (1U << (B)))
#define LIN_CLEAR_BIT(A,B)               ((A) &= ((l_u8) (~(1U << (B)))))
#define LIN_BYTE_MASK  ((l_u16)(((l_u16)((l_u16)1 << CHAR_BIT)) - (l_u16)1))
#define LIN_FRAME_LEN_MAX                                             10U

/* Returns the low byte of the 32-bit value    */
#define BYTE_0(n)                              ((l_u8)((n) & (l_u8)0xFF))
/* Returns the second byte of the 32-bit value */
#define BYTE_1(n)                        ((l_u8)(BYTE_0((n) >> (l_u8)8)))
/* Returns the third byte of the 32-bit value  */
#define BYTE_2(n)                       ((l_u8)(BYTE_0((n) >> (l_u8)16)))
/* Returns high byte of the 32-bit value       */
#define BYTE_3(n)                       ((l_u8)(BYTE_0((n) >> (l_u8)24)))

/*
 * defines for signal access
 */



#define LIN_BYTE_OFFSET_LI0_MemSave_flag    0U
#define LIN_BIT_OFFSET_LI0_MemSave_flag    0U
#define LIN_SIGNAL_SIZE_LI0_MemSave_flag    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_MemSave_flag    0U
#define LIN_FLAG_BIT_OFFSET_LI0_MemSave_flag    0U


#define LIN_BYTE_OFFSET_LI0_MemNumber    0U
#define LIN_BIT_OFFSET_LI0_MemNumber    1U
#define LIN_SIGNAL_SIZE_LI0_MemNumber    2U
#define LIN_FLAG_BYTE_OFFSET_LI0_MemNumber    0U
#define LIN_FLAG_BIT_OFFSET_LI0_MemNumber    1U


#define LIN_BYTE_OFFSET_LI0_BCM_EEM    0U
#define LIN_BIT_OFFSET_LI0_BCM_EEM    4U
#define LIN_SIGNAL_SIZE_LI0_BCM_EEM    4U
#define LIN_FLAG_BYTE_OFFSET_LI0_BCM_EEM    0U
#define LIN_FLAG_BIT_OFFSET_LI0_BCM_EEM    3U


#define LIN_BYTE_OFFSET_LI0_sigSSWLinError    11U
#define LIN_BIT_OFFSET_LI0_sigSSWLinError    7U
#define LIN_SIGNAL_SIZE_LI0_sigSSWLinError    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_sigSSWLinError    6U
#define LIN_FLAG_BIT_OFFSET_LI0_sigSSWLinError    1U


#define LIN_BYTE_OFFSET_LI0_Seat_Id    1U
#define LIN_BIT_OFFSET_LI0_Seat_Id    0U
#define LIN_SIGNAL_SIZE_LI0_Seat_Id    3U
#define LIN_FLAG_BYTE_OFFSET_LI0_Seat_Id    0U
#define LIN_FLAG_BIT_OFFSET_LI0_Seat_Id    4U


#define LIN_BYTE_OFFSET_LI0_Seat_Var    1U
#define LIN_BIT_OFFSET_LI0_Seat_Var    3U
#define LIN_SIGNAL_SIZE_LI0_Seat_Var    5U
#define LIN_FLAG_BYTE_OFFSET_LI0_Seat_Var    0U
#define LIN_FLAG_BIT_OFFSET_LI0_Seat_Var    5U


#define LIN_BYTE_OFFSET_LI0_LumbarHeadUp    2U
#define LIN_BIT_OFFSET_LI0_LumbarHeadUp    0U
#define LIN_SIGNAL_SIZE_LI0_LumbarHeadUp    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadUp    0U
#define LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadUp    6U


#define LIN_BYTE_OFFSET_LI0_LumbarHeadIncrease    2U
#define LIN_BIT_OFFSET_LI0_LumbarHeadIncrease    1U
#define LIN_SIGNAL_SIZE_LI0_LumbarHeadIncrease    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadIncrease    0U
#define LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadIncrease    7U


#define LIN_BYTE_OFFSET_LI0_LumbarHeadDown    2U
#define LIN_BIT_OFFSET_LI0_LumbarHeadDown    2U
#define LIN_SIGNAL_SIZE_LI0_LumbarHeadDown    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadDown    1U
#define LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadDown    0U


#define LIN_BYTE_OFFSET_LI0_LumbarHeadDecrease    2U
#define LIN_BIT_OFFSET_LI0_LumbarHeadDecrease    3U
#define LIN_SIGNAL_SIZE_LI0_LumbarHeadDecrease    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadDecrease    1U
#define LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadDecrease    1U


#define LIN_BYTE_OFFSET_LI0_CabinAmbientTemp    3U
#define LIN_BIT_OFFSET_LI0_CabinAmbientTemp    0U
#define LIN_SIGNAL_SIZE_LI0_CabinAmbientTemp    6U
#define LIN_FLAG_BYTE_OFFSET_LI0_CabinAmbientTemp    1U
#define LIN_FLAG_BIT_OFFSET_LI0_CabinAmbientTemp    3U


#define LIN_BYTE_OFFSET_LI0_Massage_on    4U
#define LIN_BIT_OFFSET_LI0_Massage_on    0U
#define LIN_SIGNAL_SIZE_LI0_Massage_on    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_Massage_on    2U
#define LIN_FLAG_BIT_OFFSET_LI0_Massage_on    0U


#define LIN_BYTE_OFFSET_LI0_Massage_intensity    4U
#define LIN_BIT_OFFSET_LI0_Massage_intensity    1U
#define LIN_SIGNAL_SIZE_LI0_Massage_intensity    2U
#define LIN_FLAG_BYTE_OFFSET_LI0_Massage_intensity    2U
#define LIN_FLAG_BIT_OFFSET_LI0_Massage_intensity    1U


#define LIN_BYTE_OFFSET_LI0_Massage_program    4U
#define LIN_BIT_OFFSET_LI0_Massage_program    3U
#define LIN_SIGNAL_SIZE_LI0_Massage_program    3U
#define LIN_FLAG_BYTE_OFFSET_LI0_Massage_program    2U
#define LIN_FLAG_BIT_OFFSET_LI0_Massage_program    2U


#define LIN_BYTE_OFFSET_LI0_Sidesupport_on    4U
#define LIN_BIT_OFFSET_LI0_Sidesupport_on    6U
#define LIN_SIGNAL_SIZE_LI0_Sidesupport_on    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_on    2U
#define LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_on    3U


#define LIN_BYTE_OFFSET_LI0_Sidesupport_backrest    5U
#define LIN_BIT_OFFSET_LI0_Sidesupport_backrest    0U
#define LIN_SIGNAL_SIZE_LI0_Sidesupport_backrest    2U
#define LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_backrest    2U
#define LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_backrest    4U


#define LIN_BYTE_OFFSET_LI0_Sidesupport_cushion    5U
#define LIN_BIT_OFFSET_LI0_Sidesupport_cushion    2U
#define LIN_SIGNAL_SIZE_LI0_Sidesupport_cushion    2U
#define LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_cushion    2U
#define LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_cushion    5U


#define LIN_BYTE_OFFSET_LI0_Int_Diagnostics    2U
#define LIN_BIT_OFFSET_LI0_Int_Diagnostics    4U
#define LIN_SIGNAL_SIZE_LI0_Int_Diagnostics    4U
#define LIN_FLAG_BYTE_OFFSET_LI0_Int_Diagnostics    1U
#define LIN_FLAG_BIT_OFFSET_LI0_Int_Diagnostics    2U


#define LIN_BYTE_OFFSET_LI0_Side_Acceleration    5U
#define LIN_BIT_OFFSET_LI0_Side_Acceleration    4U
#define LIN_SIGNAL_SIZE_LI0_Side_Acceleration    6U
#define LIN_FLAG_BYTE_OFFSET_LI0_Side_Acceleration    2U
#define LIN_FLAG_BIT_OFFSET_LI0_Side_Acceleration    6U


#define LIN_BYTE_OFFSET_LI0_sigpNsystemLinError    7U
#define LIN_BIT_OFFSET_LI0_sigpNsystemLinError    5U
#define LIN_SIGNAL_SIZE_LI0_sigpNsystemLinError    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_sigpNsystemLinError    3U
#define LIN_FLAG_BIT_OFFSET_LI0_sigpNsystemLinError    2U


#define LIN_BYTE_OFFSET_LI0_Diagnostics_Code    7U
#define LIN_BIT_OFFSET_LI0_Diagnostics_Code    0U
#define LIN_SIGNAL_SIZE_LI0_Diagnostics_Code    4U
#define LIN_FLAG_BYTE_OFFSET_LI0_Diagnostics_Code    3U
#define LIN_FLAG_BIT_OFFSET_LI0_Diagnostics_Code    0U


#define LIN_BYTE_OFFSET_LI0_DTCs_Pending    7U
#define LIN_BIT_OFFSET_LI0_DTCs_Pending    4U
#define LIN_SIGNAL_SIZE_LI0_DTCs_Pending    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_DTCs_Pending    3U
#define LIN_FLAG_BIT_OFFSET_LI0_DTCs_Pending    1U


#define LIN_BYTE_OFFSET_LI0_Diagnostics_Ctr    7U
#define LIN_BIT_OFFSET_LI0_Diagnostics_Ctr    6U
#define LIN_SIGNAL_SIZE_LI0_Diagnostics_Ctr    2U
#define LIN_FLAG_BYTE_OFFSET_LI0_Diagnostics_Ctr    3U
#define LIN_FLAG_BIT_OFFSET_LI0_Diagnostics_Ctr    3U


#define LIN_BYTE_OFFSET_LI0_Data_Register    8U
#define LIN_BIT_OFFSET_LI0_Data_Register    0U
#define LIN_SIGNAL_SIZE_LI0_Data_Register    8U
#define LIN_FLAG_BYTE_OFFSET_LI0_Data_Register    3U
#define LIN_FLAG_BIT_OFFSET_LI0_Data_Register    4U


#define LIN_BYTE_OFFSET_LI0_LumbarCtrUp    12U
#define LIN_BIT_OFFSET_LI0_LumbarCtrUp    0U
#define LIN_SIGNAL_SIZE_LI0_LumbarCtrUp    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrUp    6U
#define LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrUp    2U


#define LIN_BYTE_OFFSET_LI0_LumbarCtrIncrease    12U
#define LIN_BIT_OFFSET_LI0_LumbarCtrIncrease    1U
#define LIN_SIGNAL_SIZE_LI0_LumbarCtrIncrease    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrIncrease    6U
#define LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrIncrease    3U


#define LIN_BYTE_OFFSET_LI0_LumbarCtrDown    12U
#define LIN_BIT_OFFSET_LI0_LumbarCtrDown    2U
#define LIN_SIGNAL_SIZE_LI0_LumbarCtrDown    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrDown    6U
#define LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrDown    4U


#define LIN_BYTE_OFFSET_LI0_LumbarCtrDecrease    12U
#define LIN_BIT_OFFSET_LI0_LumbarCtrDecrease    3U
#define LIN_SIGNAL_SIZE_LI0_LumbarCtrDecrease    1U
#define LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrDecrease    6U
#define LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrDecrease    5U


#define LIN_BYTE_OFFSET_LI0_Upproj    13U
#define LIN_BIT_OFFSET_LI0_Upproj    0U
#define LIN_SIGNAL_SIZE_LI0_Upproj    2U
#define LIN_FLAG_BYTE_OFFSET_LI0_Upproj    8U
#define LIN_FLAG_BIT_OFFSET_LI0_Upproj    0U


#define LIN_BYTE_OFFSET_LI0_startaddr    15U
#define LIN_BIT_OFFSET_LI0_startaddr    0U
#define LIN_SIGNAL_SIZE_LI0_startaddr    8U
#define LIN_FLAG_BYTE_OFFSET_LI0_startaddr    9U
#define LIN_FLAG_BIT_OFFSET_LI0_startaddr    0U




#define LIN_FLAG_BYTE_OFFSET_LI0_SMM_brdcst             0
#define LIN_FLAG_BIT_OFFSET_LI0_SMM_brdcst              0

#define LIN_FLAG_BYTE_OFFSET_LI0_pNsystem_ctr             2
#define LIN_FLAG_BIT_OFFSET_LI0_pNsystem_ctr              0

#define LIN_FLAG_BYTE_OFFSET_LI0_pNsystem_ans             3
#define LIN_FLAG_BIT_OFFSET_LI0_pNsystem_ans              0

#define LIN_FLAG_BYTE_OFFSET_LI0_Seatsetting             4
#define LIN_FLAG_BIT_OFFSET_LI0_Seatsetting              0

#define LIN_FLAG_BYTE_OFFSET_LI0_Ubootbrd             8
#define LIN_FLAG_BIT_OFFSET_LI0_Ubootbrd              0

#define LIN_FLAG_BYTE_OFFSET_LI0_staddrbrd             9
#define LIN_FLAG_BIT_OFFSET_LI0_staddrbrd              0


/**********************************************************************/
/***************        Static API Functions        *******************/
/**********************************************************************/
/*
 * the static signal access macros
 */


/* static access macros for signal LI0_MemSave_flag */

 
#define l_bool_rd_LI0_MemSave_flag() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_MemSave_flag], \
    LIN_BIT_OFFSET_LI0_MemSave_flag))

#define l_bool_wr_LI0_MemSave_flag(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_MemSave_flag], \
    LIN_BIT_OFFSET_LI0_MemSave_flag)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_MemSave_flag], \
    LIN_BIT_OFFSET_LI0_MemSave_flag));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_MemSave_flag],\
         LIN_FLAG_BIT_OFFSET_LI0_MemSave_flag);}
 
/* static access macros for signal LI0_MemNumber */
 
#define l_u8_rd_LI0_MemNumber() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_MemNumber]) >> 1U) & 0x03U))


#define l_u8_wr_LI0_MemNumber(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_MemNumber] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_MemNumber] & 0xf9U) | \
    (((A) << 1U) & 0x06U)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_MemNumber],\
         LIN_FLAG_BIT_OFFSET_LI0_MemNumber); \
    }


 
/* static access macros for signal LI0_BCM_EEM */
 
#define l_u8_rd_LI0_BCM_EEM() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_BCM_EEM]) >> 4U) & 0x0fU))


#define l_u8_wr_LI0_BCM_EEM(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_BCM_EEM] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_BCM_EEM] & 0x0fU) | \
    (((A) << 4U) & 0xf0U)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_BCM_EEM],\
         LIN_FLAG_BIT_OFFSET_LI0_BCM_EEM); \
    }


/* static access macros for signal LI0_sigSSWLinError */

 
#define l_bool_rd_LI0_sigSSWLinError() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_sigSSWLinError], \
    LIN_BIT_OFFSET_LI0_sigSSWLinError))

#define l_bool_wr_LI0_sigSSWLinError(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_sigSSWLinError], \
    LIN_BIT_OFFSET_LI0_sigSSWLinError)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_sigSSWLinError], \
    LIN_BIT_OFFSET_LI0_sigSSWLinError));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_sigSSWLinError],\
         LIN_FLAG_BIT_OFFSET_LI0_sigSSWLinError);}
 
/* static access macros for signal LI0_Seat_Id */
 
#define l_u8_rd_LI0_Seat_Id() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Seat_Id]) >> 0U) & 0x07U))


#define l_u8_wr_LI0_Seat_Id(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Seat_Id] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Seat_Id] & 0xf8U) | \
    (((A) << 0U) & 0x07U)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Seat_Id],\
         LIN_FLAG_BIT_OFFSET_LI0_Seat_Id); \
    }


 
/* static access macros for signal LI0_Seat_Var */
 
#define l_u8_rd_LI0_Seat_Var() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Seat_Var]) >> 3U) & 0x1fU))


#define l_u8_wr_LI0_Seat_Var(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Seat_Var] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Seat_Var] & 0x07U) | \
    (((A) << 3U) & 0xf8U)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Seat_Var],\
         LIN_FLAG_BIT_OFFSET_LI0_Seat_Var); \
    }


/* static access macros for signal LI0_LumbarHeadUp */

 
#define l_bool_rd_LI0_LumbarHeadUp() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadUp], \
    LIN_BIT_OFFSET_LI0_LumbarHeadUp))

#define l_bool_wr_LI0_LumbarHeadUp(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadUp], \
    LIN_BIT_OFFSET_LI0_LumbarHeadUp)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadUp], \
    LIN_BIT_OFFSET_LI0_LumbarHeadUp));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadUp],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadUp);}
/* static access macros for signal LI0_LumbarHeadIncrease */

 
#define l_bool_rd_LI0_LumbarHeadIncrease() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadIncrease], \
    LIN_BIT_OFFSET_LI0_LumbarHeadIncrease))

#define l_bool_wr_LI0_LumbarHeadIncrease(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadIncrease], \
    LIN_BIT_OFFSET_LI0_LumbarHeadIncrease)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadIncrease], \
    LIN_BIT_OFFSET_LI0_LumbarHeadIncrease));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadIncrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadIncrease);}
/* static access macros for signal LI0_LumbarHeadDown */

 
#define l_bool_rd_LI0_LumbarHeadDown() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadDown], \
    LIN_BIT_OFFSET_LI0_LumbarHeadDown))

#define l_bool_wr_LI0_LumbarHeadDown(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadDown], \
    LIN_BIT_OFFSET_LI0_LumbarHeadDown)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadDown], \
    LIN_BIT_OFFSET_LI0_LumbarHeadDown));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadDown],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadDown);}
/* static access macros for signal LI0_LumbarHeadDecrease */

 
#define l_bool_rd_LI0_LumbarHeadDecrease() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadDecrease], \
    LIN_BIT_OFFSET_LI0_LumbarHeadDecrease))

#define l_bool_wr_LI0_LumbarHeadDecrease(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadDecrease], \
    LIN_BIT_OFFSET_LI0_LumbarHeadDecrease)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarHeadDecrease], \
    LIN_BIT_OFFSET_LI0_LumbarHeadDecrease));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadDecrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadDecrease);}
 
/* static access macros for signal LI0_CabinAmbientTemp */
 
#define l_u8_rd_LI0_CabinAmbientTemp() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_CabinAmbientTemp]) >> 0U) & 0x3fU))


#define l_u8_wr_LI0_CabinAmbientTemp(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_CabinAmbientTemp] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_CabinAmbientTemp] & 0xc0U) | \
    (((A) << 0U) & 0x3fU)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_CabinAmbientTemp],\
         LIN_FLAG_BIT_OFFSET_LI0_CabinAmbientTemp); \
    }


/* static access macros for signal LI0_Massage_on */

 
#define l_bool_rd_LI0_Massage_on() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Massage_on], \
    LIN_BIT_OFFSET_LI0_Massage_on))

#define l_bool_wr_LI0_Massage_on(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Massage_on], \
    LIN_BIT_OFFSET_LI0_Massage_on)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Massage_on], \
    LIN_BIT_OFFSET_LI0_Massage_on));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Massage_on],\
         LIN_FLAG_BIT_OFFSET_LI0_Massage_on);}
 
/* static access macros for signal LI0_Massage_intensity */
 
#define l_u8_rd_LI0_Massage_intensity() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Massage_intensity]) >> 1U) & 0x03U))


#define l_u8_wr_LI0_Massage_intensity(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Massage_intensity] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Massage_intensity] & 0xf9U) | \
    (((A) << 1U) & 0x06U)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Massage_intensity],\
         LIN_FLAG_BIT_OFFSET_LI0_Massage_intensity); \
    }


 
/* static access macros for signal LI0_Massage_program */
 
#define l_u8_rd_LI0_Massage_program() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Massage_program]) >> 3U) & 0x07U))


#define l_u8_wr_LI0_Massage_program(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Massage_program] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Massage_program] & 0xc7U) | \
    (((A) << 3U) & 0x38U)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Massage_program],\
         LIN_FLAG_BIT_OFFSET_LI0_Massage_program); \
    }


/* static access macros for signal LI0_Sidesupport_on */

 
#define l_bool_rd_LI0_Sidesupport_on() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Sidesupport_on], \
    LIN_BIT_OFFSET_LI0_Sidesupport_on))

#define l_bool_wr_LI0_Sidesupport_on(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Sidesupport_on], \
    LIN_BIT_OFFSET_LI0_Sidesupport_on)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Sidesupport_on], \
    LIN_BIT_OFFSET_LI0_Sidesupport_on));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_on],\
         LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_on);}
 
/* static access macros for signal LI0_Sidesupport_backrest */
 
#define l_u8_rd_LI0_Sidesupport_backrest() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Sidesupport_backrest]) >> 0U) & 0x03U))


#define l_u8_wr_LI0_Sidesupport_backrest(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Sidesupport_backrest] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Sidesupport_backrest] & 0xfcU) | \
    (((A) << 0U) & 0x03U)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_backrest],\
         LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_backrest); \
    }


 
/* static access macros for signal LI0_Sidesupport_cushion */
 
#define l_u8_rd_LI0_Sidesupport_cushion() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Sidesupport_cushion]) >> 2U) & 0x03U))


#define l_u8_wr_LI0_Sidesupport_cushion(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Sidesupport_cushion] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Sidesupport_cushion] & 0xf3U) | \
    (((A) << 2U) & 0x0cU)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_cushion],\
         LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_cushion); \
    }


 
/* static access macros for signal LI0_Int_Diagnostics */
 
#define l_u8_rd_LI0_Int_Diagnostics() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Int_Diagnostics]) >> 4U) & 0x0fU))


#define l_u8_wr_LI0_Int_Diagnostics(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Int_Diagnostics] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Int_Diagnostics] & 0x0fU) | \
    (((A) << 4U) & 0xf0U)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Int_Diagnostics],\
         LIN_FLAG_BIT_OFFSET_LI0_Int_Diagnostics); \
    }


 
/* static access macros for signal LI0_Side_Acceleration */
 
#define l_u8_rd_LI0_Side_Acceleration() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Side_Acceleration] + (lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Side_Acceleration + 1U] << 8U)) >> 4U) & 0x3fU))


#define l_u8_wr_LI0_Side_Acceleration(A) \
    { \
    buffer_backup_data[1U] =  lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Side_Acceleration]; \
    lin_frame_updating_flag_tbl[LI0_pNsystem_ctr] |= (1U << 1); \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Side_Acceleration] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Side_Acceleration] & 0x0fU) | \
    (((A) << 4U) & 0xf0U)); \
    buffer_backup_data[1U + 1U] =  lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Side_Acceleration + 1U]; \
    lin_frame_updating_flag_tbl[LI0_pNsystem_ctr] |= (1U << (1 + 1U)); \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Side_Acceleration + 1U] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Side_Acceleration + 1U] & 0xfcU) | \
    (((A) >> 4U) & 0x03U)); \
    lin_frame_updating_flag_tbl[LI0_pNsystem_ctr] &= (~(0x03 << 1)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Side_Acceleration],\
         LIN_FLAG_BIT_OFFSET_LI0_Side_Acceleration); \
    }


/* static access macros for signal LI0_sigpNsystemLinError */

 
#define l_bool_rd_LI0_sigpNsystemLinError() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_sigpNsystemLinError], \
    LIN_BIT_OFFSET_LI0_sigpNsystemLinError))

#define l_bool_wr_LI0_sigpNsystemLinError(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_sigpNsystemLinError], \
    LIN_BIT_OFFSET_LI0_sigpNsystemLinError)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_sigpNsystemLinError], \
    LIN_BIT_OFFSET_LI0_sigpNsystemLinError));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_sigpNsystemLinError],\
         LIN_FLAG_BIT_OFFSET_LI0_sigpNsystemLinError);}
 
/* static access macros for signal LI0_Diagnostics_Code */
 
#define l_u8_rd_LI0_Diagnostics_Code() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Diagnostics_Code]) >> 0U) & 0x0fU))


#define l_u8_wr_LI0_Diagnostics_Code(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Diagnostics_Code] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Diagnostics_Code] & 0xf0U) | \
    (((A) << 0U) & 0x0fU)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Diagnostics_Code],\
         LIN_FLAG_BIT_OFFSET_LI0_Diagnostics_Code); \
    }


/* static access macros for signal LI0_DTCs_Pending */

 
#define l_bool_rd_LI0_DTCs_Pending() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_DTCs_Pending], \
    LIN_BIT_OFFSET_LI0_DTCs_Pending))

#define l_bool_wr_LI0_DTCs_Pending(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_DTCs_Pending], \
    LIN_BIT_OFFSET_LI0_DTCs_Pending)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_DTCs_Pending], \
    LIN_BIT_OFFSET_LI0_DTCs_Pending));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_DTCs_Pending],\
         LIN_FLAG_BIT_OFFSET_LI0_DTCs_Pending);}
 
/* static access macros for signal LI0_Diagnostics_Ctr */
 
#define l_u8_rd_LI0_Diagnostics_Ctr() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Diagnostics_Ctr]) >> 6U) & 0x03U))


#define l_u8_wr_LI0_Diagnostics_Ctr(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Diagnostics_Ctr] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Diagnostics_Ctr] & 0x3fU) | \
    (((A) << 6U) & 0xc0U)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Diagnostics_Ctr],\
         LIN_FLAG_BIT_OFFSET_LI0_Diagnostics_Ctr); \
    }


 
/* static access macros for signal LI0_Data_Register */
 
#define l_u8_rd_LI0_Data_Register() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Data_Register]) >> 0U) & 0xffU))


#define l_u8_wr_LI0_Data_Register(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Data_Register] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Data_Register] & 0x00U) | \
    (((A) << 0U) & 0xffU)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Data_Register],\
         LIN_FLAG_BIT_OFFSET_LI0_Data_Register); \
    }


/* static access macros for signal LI0_LumbarCtrUp */

 
#define l_bool_rd_LI0_LumbarCtrUp() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrUp], \
    LIN_BIT_OFFSET_LI0_LumbarCtrUp))

#define l_bool_wr_LI0_LumbarCtrUp(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrUp], \
    LIN_BIT_OFFSET_LI0_LumbarCtrUp)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrUp], \
    LIN_BIT_OFFSET_LI0_LumbarCtrUp));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrUp],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrUp);}
/* static access macros for signal LI0_LumbarCtrIncrease */

 
#define l_bool_rd_LI0_LumbarCtrIncrease() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrIncrease], \
    LIN_BIT_OFFSET_LI0_LumbarCtrIncrease))

#define l_bool_wr_LI0_LumbarCtrIncrease(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrIncrease], \
    LIN_BIT_OFFSET_LI0_LumbarCtrIncrease)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrIncrease], \
    LIN_BIT_OFFSET_LI0_LumbarCtrIncrease));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrIncrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrIncrease);}
/* static access macros for signal LI0_LumbarCtrDown */

 
#define l_bool_rd_LI0_LumbarCtrDown() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrDown], \
    LIN_BIT_OFFSET_LI0_LumbarCtrDown))

#define l_bool_wr_LI0_LumbarCtrDown(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrDown], \
    LIN_BIT_OFFSET_LI0_LumbarCtrDown)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrDown], \
    LIN_BIT_OFFSET_LI0_LumbarCtrDown));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrDown],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrDown);}
/* static access macros for signal LI0_LumbarCtrDecrease */

 
#define l_bool_rd_LI0_LumbarCtrDecrease() \
    (LIN_TEST_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrDecrease], \
    LIN_BIT_OFFSET_LI0_LumbarCtrDecrease))

#define l_bool_wr_LI0_LumbarCtrDecrease(A) \
    {(A) ? \
    (LIN_SET_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrDecrease], \
    LIN_BIT_OFFSET_LI0_LumbarCtrDecrease)):\
    (LIN_CLEAR_BIT(lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_LumbarCtrDecrease], \
    LIN_BIT_OFFSET_LI0_LumbarCtrDecrease));\
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrDecrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrDecrease);}
 
/* static access macros for signal LI0_Upproj */
 
#define l_u8_rd_LI0_Upproj() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Upproj]) >> 0U) & 0x03U))


#define l_u8_wr_LI0_Upproj(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Upproj] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_Upproj] & 0xfcU) | \
    (((A) << 0U) & 0x03U)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Upproj],\
         LIN_FLAG_BIT_OFFSET_LI0_Upproj); \
    }


 
/* static access macros for signal LI0_startaddr */
 
#define l_u8_rd_LI0_startaddr() \
    ((l_u8)  (((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_startaddr]) >> 0U) & 0xffU))


#define l_u8_wr_LI0_startaddr(A) \
    { \
    lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_startaddr] = \
    (l_u8)((lin_pFrameBuf[LIN_BYTE_OFFSET_LI0_startaddr] & 0x00U) | \
    (((A) << 0U) & 0xffU)); \
    LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_startaddr],\
         LIN_FLAG_BIT_OFFSET_LI0_startaddr); \
    }




/* Signal flag APIs */

#define l_flg_tst_LI0_MemSave_flag_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_MemSave_flag],\
         LIN_FLAG_BIT_OFFSET_LI0_MemSave_flag)
#define l_flg_clr_LI0_MemSave_flag_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_MemSave_flag],\
         LIN_FLAG_BIT_OFFSET_LI0_MemSave_flag)

#define l_flg_tst_LI0_MemNumber_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_MemNumber],\
         LIN_FLAG_BIT_OFFSET_LI0_MemNumber)
#define l_flg_clr_LI0_MemNumber_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_MemNumber],\
         LIN_FLAG_BIT_OFFSET_LI0_MemNumber)

#define l_flg_tst_LI0_BCM_EEM_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_BCM_EEM],\
         LIN_FLAG_BIT_OFFSET_LI0_BCM_EEM)
#define l_flg_clr_LI0_BCM_EEM_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_BCM_EEM],\
         LIN_FLAG_BIT_OFFSET_LI0_BCM_EEM)

#define l_flg_tst_LI0_sigSSWLinError_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_sigSSWLinError],\
         LIN_FLAG_BIT_OFFSET_LI0_sigSSWLinError)
#define l_flg_clr_LI0_sigSSWLinError_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_sigSSWLinError],\
         LIN_FLAG_BIT_OFFSET_LI0_sigSSWLinError)

#define l_flg_tst_LI0_Seat_Id_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Seat_Id],\
         LIN_FLAG_BIT_OFFSET_LI0_Seat_Id)
#define l_flg_clr_LI0_Seat_Id_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Seat_Id],\
         LIN_FLAG_BIT_OFFSET_LI0_Seat_Id)

#define l_flg_tst_LI0_Seat_Var_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Seat_Var],\
         LIN_FLAG_BIT_OFFSET_LI0_Seat_Var)
#define l_flg_clr_LI0_Seat_Var_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Seat_Var],\
         LIN_FLAG_BIT_OFFSET_LI0_Seat_Var)

#define l_flg_tst_LI0_LumbarHeadUp_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadUp],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadUp)
#define l_flg_clr_LI0_LumbarHeadUp_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadUp],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadUp)

#define l_flg_tst_LI0_LumbarHeadIncrease_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadIncrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadIncrease)
#define l_flg_clr_LI0_LumbarHeadIncrease_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadIncrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadIncrease)

#define l_flg_tst_LI0_LumbarHeadDown_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadDown],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadDown)
#define l_flg_clr_LI0_LumbarHeadDown_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadDown],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadDown)

#define l_flg_tst_LI0_LumbarHeadDecrease_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadDecrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadDecrease)
#define l_flg_clr_LI0_LumbarHeadDecrease_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarHeadDecrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarHeadDecrease)

#define l_flg_tst_LI0_CabinAmbientTemp_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_CabinAmbientTemp],\
         LIN_FLAG_BIT_OFFSET_LI0_CabinAmbientTemp)
#define l_flg_clr_LI0_CabinAmbientTemp_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_CabinAmbientTemp],\
         LIN_FLAG_BIT_OFFSET_LI0_CabinAmbientTemp)

#define l_flg_tst_LI0_Massage_on_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Massage_on],\
         LIN_FLAG_BIT_OFFSET_LI0_Massage_on)
#define l_flg_clr_LI0_Massage_on_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Massage_on],\
         LIN_FLAG_BIT_OFFSET_LI0_Massage_on)

#define l_flg_tst_LI0_Massage_intensity_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Massage_intensity],\
         LIN_FLAG_BIT_OFFSET_LI0_Massage_intensity)
#define l_flg_clr_LI0_Massage_intensity_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Massage_intensity],\
         LIN_FLAG_BIT_OFFSET_LI0_Massage_intensity)

#define l_flg_tst_LI0_Massage_program_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Massage_program],\
         LIN_FLAG_BIT_OFFSET_LI0_Massage_program)
#define l_flg_clr_LI0_Massage_program_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Massage_program],\
         LIN_FLAG_BIT_OFFSET_LI0_Massage_program)

#define l_flg_tst_LI0_Sidesupport_on_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_on],\
         LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_on)
#define l_flg_clr_LI0_Sidesupport_on_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_on],\
         LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_on)

#define l_flg_tst_LI0_Sidesupport_backrest_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_backrest],\
         LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_backrest)
#define l_flg_clr_LI0_Sidesupport_backrest_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_backrest],\
         LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_backrest)

#define l_flg_tst_LI0_Sidesupport_cushion_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_cushion],\
         LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_cushion)
#define l_flg_clr_LI0_Sidesupport_cushion_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Sidesupport_cushion],\
         LIN_FLAG_BIT_OFFSET_LI0_Sidesupport_cushion)

#define l_flg_tst_LI0_Int_Diagnostics_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Int_Diagnostics],\
         LIN_FLAG_BIT_OFFSET_LI0_Int_Diagnostics)
#define l_flg_clr_LI0_Int_Diagnostics_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Int_Diagnostics],\
         LIN_FLAG_BIT_OFFSET_LI0_Int_Diagnostics)

#define l_flg_tst_LI0_Side_Acceleration_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Side_Acceleration],\
         LIN_FLAG_BIT_OFFSET_LI0_Side_Acceleration)
#define l_flg_clr_LI0_Side_Acceleration_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Side_Acceleration],\
         LIN_FLAG_BIT_OFFSET_LI0_Side_Acceleration)

#define l_flg_tst_LI0_sigpNsystemLinError_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_sigpNsystemLinError],\
         LIN_FLAG_BIT_OFFSET_LI0_sigpNsystemLinError)
#define l_flg_clr_LI0_sigpNsystemLinError_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_sigpNsystemLinError],\
         LIN_FLAG_BIT_OFFSET_LI0_sigpNsystemLinError)

#define l_flg_tst_LI0_Diagnostics_Code_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Diagnostics_Code],\
         LIN_FLAG_BIT_OFFSET_LI0_Diagnostics_Code)
#define l_flg_clr_LI0_Diagnostics_Code_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Diagnostics_Code],\
         LIN_FLAG_BIT_OFFSET_LI0_Diagnostics_Code)

#define l_flg_tst_LI0_DTCs_Pending_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_DTCs_Pending],\
         LIN_FLAG_BIT_OFFSET_LI0_DTCs_Pending)
#define l_flg_clr_LI0_DTCs_Pending_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_DTCs_Pending],\
         LIN_FLAG_BIT_OFFSET_LI0_DTCs_Pending)

#define l_flg_tst_LI0_Diagnostics_Ctr_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Diagnostics_Ctr],\
         LIN_FLAG_BIT_OFFSET_LI0_Diagnostics_Ctr)
#define l_flg_clr_LI0_Diagnostics_Ctr_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Diagnostics_Ctr],\
         LIN_FLAG_BIT_OFFSET_LI0_Diagnostics_Ctr)

#define l_flg_tst_LI0_Data_Register_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Data_Register],\
         LIN_FLAG_BIT_OFFSET_LI0_Data_Register)
#define l_flg_clr_LI0_Data_Register_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Data_Register],\
         LIN_FLAG_BIT_OFFSET_LI0_Data_Register)

#define l_flg_tst_LI0_LumbarCtrUp_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrUp],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrUp)
#define l_flg_clr_LI0_LumbarCtrUp_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrUp],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrUp)

#define l_flg_tst_LI0_LumbarCtrIncrease_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrIncrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrIncrease)
#define l_flg_clr_LI0_LumbarCtrIncrease_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrIncrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrIncrease)

#define l_flg_tst_LI0_LumbarCtrDown_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrDown],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrDown)
#define l_flg_clr_LI0_LumbarCtrDown_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrDown],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrDown)

#define l_flg_tst_LI0_LumbarCtrDecrease_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrDecrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrDecrease)
#define l_flg_clr_LI0_LumbarCtrDecrease_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_LumbarCtrDecrease],\
         LIN_FLAG_BIT_OFFSET_LI0_LumbarCtrDecrease)

#define l_flg_tst_LI0_Upproj_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Upproj],\
         LIN_FLAG_BIT_OFFSET_LI0_Upproj)
#define l_flg_clr_LI0_Upproj_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_Upproj],\
         LIN_FLAG_BIT_OFFSET_LI0_Upproj)

#define l_flg_tst_LI0_startaddr_flag() \
         LIN_TEST_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_startaddr],\
         LIN_FLAG_BIT_OFFSET_LI0_startaddr)
#define l_flg_clr_LI0_startaddr_flag() \
         LIN_CLEAR_BIT(lin_flag_handle_tbl[LIN_FLAG_BYTE_OFFSET_LI0_startaddr],\
         LIN_FLAG_BIT_OFFSET_LI0_startaddr)



/* Frame flag APIs */

   /* Interface_name = LI0 */

 #define l_flg_tst_LI0_SMM_brdcst_flag() \
          lin_frame_flag_tbl[LI0_SMM_brdcst]
 #define l_flg_clr_LI0_SMM_brdcst_flag() \
          lin_frame_flag_tbl[LI0_SMM_brdcst] = 0

 #define l_flg_tst_LI0_pNsystem_ctr_flag() \
          lin_frame_flag_tbl[LI0_pNsystem_ctr]
 #define l_flg_clr_LI0_pNsystem_ctr_flag() \
          lin_frame_flag_tbl[LI0_pNsystem_ctr] = 0

 #define l_flg_tst_LI0_pNsystem_ans_flag() \
          lin_frame_flag_tbl[LI0_pNsystem_ans]
 #define l_flg_clr_LI0_pNsystem_ans_flag() \
          lin_frame_flag_tbl[LI0_pNsystem_ans] = 0

 #define l_flg_tst_LI0_Seatsetting_flag() \
          lin_frame_flag_tbl[LI0_Seatsetting]
 #define l_flg_clr_LI0_Seatsetting_flag() \
          lin_frame_flag_tbl[LI0_Seatsetting] = 0

 #define l_flg_tst_LI0_Ubootbrd_flag() \
          lin_frame_flag_tbl[LI0_Ubootbrd]
 #define l_flg_clr_LI0_Ubootbrd_flag() \
          lin_frame_flag_tbl[LI0_Ubootbrd] = 0

 #define l_flg_tst_LI0_staddrbrd_flag() \
          lin_frame_flag_tbl[LI0_staddrbrd]
 #define l_flg_clr_LI0_staddrbrd_flag() \
          lin_frame_flag_tbl[LI0_staddrbrd] = 0

 #define l_flg_tst_LI0_MasterReq_flag() \
          lin_frame_flag_tbl[LI0_MasterReq]
 #define l_flg_clr_LI0_MasterReq_flag() \
          lin_frame_flag_tbl[LI0_MasterReq] = 0

 #define l_flg_tst_LI0_SlaveResp_flag() \
          lin_frame_flag_tbl[LI0_SlaveResp]
 #define l_flg_clr_LI0_SlaveResp_flag() \
          lin_frame_flag_tbl[LI0_SlaveResp] = 0



/* INTERFACE MANAGEMENT */

#define l_ifc_init_LI0() l_ifc_init(LI0)



#define l_ifc_wake_up_LI0() l_ifc_wake_up(LI0)



#define l_ifc_rx_LI0() l_ifc_rx(LI0)



#define l_ifc_tx_LI0() l_ifc_tx(LI0)



#define l_ifc_aux_LI0() l_ifc_aux(LI0)



#define l_ifc_read_status_LI0() l_ifc_read_status(LI0)


#endif    /* _LIN_CFG_H_ */