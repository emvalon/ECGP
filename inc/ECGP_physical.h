/**
  ******************************************************************************
  * File Name          : ECGP_physical.h
  * Description        : physical layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#ifndef _ECGP_PHYSICAL_H_
#define _ECGP_PHYSICAL_H_

#include "ECGP_link.h"
#include "ECGP_common.h" 




 ECGP_error ECGP_physicalSend(u8* data, u16 len);
 ECGP_error ECGP_physicalRecv(u8* data, u16 len);


 extern ECGP_error link_hasSent(u16 sendLen);
 extern ECGP_error link_hasReceived(u16 sendLen);






#endif
