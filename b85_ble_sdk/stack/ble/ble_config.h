/********************************************************************************************************
 * @file	ble_config.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	06,2020
 *
 * @par		Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd.
 *			All rights reserved.
 *
 *          The information contained herein is confidential property of Telink
 *          Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *          of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *          Co., Ltd. and the licensee or the terms described here-in. This heading
 *          MUST NOT be removed from this file.
 *
 *          Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
 *          alter) any information contained herein in whole or in part except as expressly authorized
 *          by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
 *          for any claim to the extent arising out of or relating to such deletion(s), modification(s)
 *          or alteration(s).
 *
 *          Licensees are granted free, non-transferable use of the information in this
 *          file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#pragma once


#include "drivers.h"
#include "tl_common.h"

#if(MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
	#define  MAX_DEV_NAME_LEN 				18

	#ifndef DEV_NAME
	#define DEV_NAME                        "tModule"
	#endif
#endif


#if (MCU_CORE_TYPE == MCU_CORE_825x)
	#define	FIX_HW_CRC24_EN									1
	#define HW_ECDH_EN                                      0
#elif (MCU_CORE_TYPE == MCU_CORE_827x)
	#define	FIX_HW_CRC24_EN									0
	#define HW_ECDH_EN                                      1
#elif (MCU_CORE_TYPE == MCU_CORE_9518)
	#define	FIX_HW_CRC24_EN									0
	#define HW_ECDH_EN                                      1
	#define BLT_CONN_MASTER_EN								0
	#define BLT_SCAN_EN										0
	#define BLT_ADV_IN_CONN_SLAVE_EN						0
	#define BLT_SCAN_IN_CONN_SLAVE_EN						0
#else
	#error "unsupported mcu type !"
#endif



///////////////////  Feature ////////////////////////////
#ifndef			BLT_CONN_MASTER_EN
#define			BLT_CONN_MASTER_EN								1
#endif

#ifndef			BLT_SCAN_EN
#define			BLT_SCAN_EN										1
#endif

#ifndef			BLT_ADV_IN_CONN_SLAVE_EN
#define			BLT_ADV_IN_CONN_SLAVE_EN						1
#endif

#ifndef			BLT_SCAN_IN_CONN_SLAVE_EN
#define			BLT_SCAN_IN_CONN_SLAVE_EN						1
#endif



#ifndef			BIS_IN_BIG_NUM_MAX
#define			BIS_IN_BIG_NUM_MAX								4
#endif


#ifndef			LL_CIS_IN_CIG_NUM_MAX
#define 		LL_CIS_IN_CIG_NUM_MAX              				4
#endif

///////////////////  Feature ////////////////////////////


#ifndef SECURE_CONNECTION_ENABLE
#define SECURE_CONNECTION_ENABLE							1
#endif









#ifndef  LL_MASTER_MULTI_CONNECTION
#define  LL_MASTER_MULTI_CONNECTION							0
#endif




#ifndef		BLS_ADV_INTERVAL_CHECK_ENABLE
#define		BLS_ADV_INTERVAL_CHECK_ENABLE					0
#endif





//conn param update/map update
#ifndef	BLS_PROC_MASTER_UPDATE_REQ_IN_IRQ_ENABLE
#define BLS_PROC_MASTER_UPDATE_REQ_IN_IRQ_ENABLE			1
#endif


#ifndef LE_AUTHENTICATED_PAYLOAD_TIMEOUT_SUPPORT_EN
#define LE_AUTHENTICATED_PAYLOAD_TIMEOUT_SUPPORT_EN			0
#endif





#ifndef DEEP_RET_ENTRY_CONDITIONS_OPTIMIZATION_EN
#define	DEEP_RET_ENTRY_CONDITIONS_OPTIMIZATION_EN			0
#endif


#ifndef HOST_CONTROLLER_DATA_FLOW_IMPROVE_EN
#define	HOST_CONTROLLER_DATA_FLOW_IMPROVE_EN				1
#endif



//Link layer feature enable flag default setting
#ifndef BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE
#define BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE						1
#endif

#ifndef LL_FEATURE_SUPPORT_LE_2M_PHY
#define LL_FEATURE_SUPPORT_LE_2M_PHY								1
#endif

#ifndef LL_FEATURE_SUPPORT_LE_CODED_PHY
#define LL_FEATURE_SUPPORT_LE_CODED_PHY								1
#endif

#ifndef LL_FEATURE_SUPPORT_LE_EXTENDED_ADVERTISING
#define LL_FEATURE_SUPPORT_LE_EXTENDED_ADVERTISING					1
#endif

#ifndef LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING
#define LL_FEATURE_SUPPORT_LE_PERIODIC_ADVERTISING					0
#endif

#ifndef LL_FEATURE_SUPPORT_CHANNEL_SELECTION_ALGORITHM2
#define LL_FEATURE_SUPPORT_CHANNEL_SELECTION_ALGORITHM2				1
#endif




//core_5.2 feature begin
#ifndef LL_FEATURE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_MASTER
#define LL_FEATURE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_MASTER		1
#endif

#ifndef LL_FEATURE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_SLAVE
#define LL_FEATURE_SUPPORT_CONNECTED_ISOCHRONOUS_STREAM_SLAVE		1
#endif

#ifndef LL_FEATURE_SUPPORT_ISOCHRONOUS_BROADCASTER
#define LL_FEATURE_SUPPORT_ISOCHRONOUS_BROADCASTER					1
#endif

#ifndef LL_FEATURE_SUPPORT_SYNCHRONIZED_RECEIVER
#define LL_FEATURE_SUPPORT_SYNCHRONIZED_RECEIVER					1
#endif

#ifndef LL_FEATURE_SUPPORT_ISOCHRONOUS_CHANNELS
#define LL_FEATURE_SUPPORT_ISOCHRONOUS_CHANNELS						1
#endif
//core_5.2 feature end




///////////////////////////////////////dbg channels///////////////////////////////////////////
#ifndef	DBG_CHN0_TOGGLE
#define DBG_CHN0_TOGGLE
#endif

#ifndef	DBG_CHN0_HIGH
#define DBG_CHN0_HIGH
#endif

#ifndef	DBG_CHN0_LOW
#define DBG_CHN0_LOW
#endif

#ifndef	DBG_CHN1_TOGGLE
#define DBG_CHN1_TOGGLE
#endif

#ifndef	DBG_CHN1_HIGH
#define DBG_CHN1_HIGH
#endif

#ifndef	DBG_CHN1_LOW
#define DBG_CHN1_LOW
#endif

#ifndef	DBG_CHN2_TOGGLE
#define DBG_CHN2_TOGGLE
#endif

#ifndef	DBG_CHN2_HIGH
#define DBG_CHN2_HIGH
#endif

#ifndef	DBG_CHN2_LOW
#define DBG_CHN2_LOW
#endif

#ifndef	DBG_CHN3_TOGGLE
#define DBG_CHN3_TOGGLE
#endif

#ifndef	DBG_CHN3_HIGH
#define DBG_CHN3_HIGH
#endif

#ifndef	DBG_CHN3_LOW
#define DBG_CHN3_LOW
#endif

#ifndef	DBG_CHN4_TOGGLE
#define DBG_CHN4_TOGGLE
#endif

#ifndef	DBG_CHN4_HIGH
#define DBG_CHN4_HIGH
#endif

#ifndef	DBG_CHN4_LOW
#define DBG_CHN4_LOW
#endif

#ifndef	DBG_CHN5_TOGGLE
#define DBG_CHN5_TOGGLE
#endif

#ifndef	DBG_CHN5_HIGH
#define DBG_CHN5_HIGH
#endif

#ifndef	DBG_CHN5_LOW
#define DBG_CHN5_LOW
#endif

#ifndef	DBG_CHN6_TOGGLE
#define DBG_CHN6_TOGGLE
#endif

#ifndef	DBG_CHN6_HIGH
#define DBG_CHN6_HIGH
#endif

#ifndef	DBG_CHN6_LOW
#define DBG_CHN6_LOW
#endif

#ifndef	DBG_CHN7_TOGGLE
#define DBG_CHN7_TOGGLE
#endif

#ifndef	DBG_CHN7_HIGH
#define DBG_CHN7_HIGH
#endif

#ifndef	DBG_CHN7_LOW
#define DBG_CHN7_LOW
#endif



#ifndef	BLE_IRQ_DBG_EN
#define BLE_IRQ_DBG_EN					0
#endif

#ifndef	BLE_ADV_DBG_EN
#define BLE_ADV_DBG_EN					0
#endif

#ifndef	TX_FIFO_DBG_EN
#define TX_FIFO_DBG_EN					0
#endif


#ifndef		DEBUG_PAIRING_ENCRYPTION
#define 	DEBUG_PAIRING_ENCRYPTION		0
#endif

