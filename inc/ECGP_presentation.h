/**
  ******************************************************************************
  * File Name          : ECGP_presentation.h
  * Description        : presentation layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#ifndef _ECGP_PRESENTATION_H_
#define _ECGP_PRESENTATION_H_

#include "ECGP_application.h"
#include "ECGP_transport .h"
#include "ECGP_common.h"

#define ECGP_PRESENT_LEN_MAX        ECGP_APPLICATION_LEN_MAX

#define ECGP_presentSend                ECGP_transportSend
#define ECGP_presentRecv                ECGP_transportRecv
#define ECGP_presentElapsed             ECGP_transportElapsed
//ECGP_error ECGP_presentSend(u8* data, u16 len);
//ECGP_error ECGP_presentRecv(u8* data, u16 len);












#endif


