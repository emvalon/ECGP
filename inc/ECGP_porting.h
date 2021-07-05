/**
 * @file ECGP_porting.h
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-05
 * 
 * Copyright © 2019-2021 Weilong Shen (valonshen@foxmail.com).
 * All rights reserved.
 * 
 */

#ifndef _ECGP_PHYSICAL_H_
#define _ECGP_PHYSICAL_H_

#include <stddef.h>
#include "ECGP_common.h" 

u32 ECGP_getTime(void);
ECGP_error ECGP_physicalSend(u8* data, u16 len);
ECGP_error ECGP_physicalRecv(u8* data, u16 len);


extern ECGP_error link_hasSent(u16 sendLen);
extern ECGP_error link_hasReceived(u16 sendLen);
#endif
