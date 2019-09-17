/**
  ******************************************************************************
  * File Name          : ECGP_application.h
  * Description        : application layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#ifndef _ECGP_APPLICATION_H_
#define _ECGP_APPLICATION_H_

#include "ECGP_presentation.h"
#include "ECGP_common.h"

#define ECGP_APPLICATION_LEN_MAX            100




ECGP_error ECGP_send(u8* data, u16 len);
ECGP_error ECGP_recv(u8* data, u16 len);
ECGP_error ECGP_timeElapsed(int time);

void ECGP_setRxCallback(link_callback_typedef fun);
void ECGP_init(void);







#endif

