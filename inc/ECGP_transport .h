/**
  ******************************************************************************
  * File Name          : ECGP_transport.h
  * Description        : transport layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#ifndef _ECGP_TRANSPORT_H_
#define _ECGP_TRANSPORT_H_


#include "ECGP_presentation.h"
#include "ECGP_network.h"
#include "ECGP_common.h"

// Transport layer buffer maximum length
#define ECGP_TRANS_LEN_MAX      (ECGP_PRESENT_LEN_MAX + 3)

ECGP_error ECGP_transportSend(u8* data, u16 len);
ECGP_error ECGP_transportRecv(u8* data, u16 len);














#endif


