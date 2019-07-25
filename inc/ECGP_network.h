/**
  ******************************************************************************
  * File Name          : ECGP_netwotk.h
  * Description        : network layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#ifndef _ECGP_NETWORK_H_
#define _ECGP_NETWORK_H_

#include "ECGP_transport .h"
#include "ECGP_link.h"
#include "ECGP_common.h"

#define ECGP_NET_LEN_MAX        ECGP_TRANS_LEN_MAX


#define ECGP_networkSend        ECGP_linkSend
#define ECGP_networkRecv        ECGP_linkRecv


//ECGP_error ECGP_networkSend(u8* data, u16 len);
//ECGP_error ECGP_networkRecv(u8* data, u16 len);










#endif


