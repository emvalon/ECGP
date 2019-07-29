/**
  ******************************************************************************
  * File Name          : simulate.c
  * Description        : physical layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/


#include "ECGP_link.h"
#include "ECGP_physical.h" 
#include "string.h"


u8* bufData;
u16 bufLen;
 ECGP_error ECGP_physicalSend(u8* data, u16 len)
 {
    u16 l;
    if (len > bufLen) {

    }
    else {
        memcpy(bufData, data, len);
        link_hasReceived(len);
    }   
    link_hasSent(len); 
 }


 ECGP_error ECGP_physicalRecv(u8* data, u16 len)
 {
     bufData = data;
     bufLen = len;
 }









