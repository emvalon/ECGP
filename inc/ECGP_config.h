/**
  ******************************************************************************
  * File Name          : ECGP_config.h
  * Description        : ECGP configurable options
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#ifndef _ECGP_CONFIG_H_
#define _ECGP_CONFIG_H_

/*
 *************************************************
 * Using crc tabel can improve the compute speed.
 * The other hand, it will take more space.
 *************************************************
 */

//#define ECGP_USE_CRC8_TABLE 
//#define ECGP_USE_CRC16_TABLE 
//#define ECGP_USE_CRC32_TABLE 


/*
 *************************************************
 * If you want to receive message in order ,
 * Pls define this macro.
 *************************************************
 */

#define ECGP_RECV_IN_ORDER


 /*
  *************************************************
  * In order to protect critical data ,
  * Pls define these macro.
  *************************************************
  */

#define _ECGP_ENTER_CRITICAL
#define _ECGP_LEAVE_CRITICAL











#endif


