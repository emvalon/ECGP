/**
 * @file ECGP_presentation.c
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-08
 * 
 * Copyright © 2019-2021 Weilong Shen (valonshen@foxmail.com).
 * All rights reserved.
 * 
 */
#include "ECGP_presentation.h"
#include "ECGP_config.h"

/**
 * @brief Present layer send
 * 
 * @param data Pointer of tx buffer
 * @param len Length of buffer
 * @return ECGP_error 
 */
ECGP_error ECGP_presentSend(u8* data, u16 len)
{
    ECGP_transportSend(data, len);
}

/**
 * @brief Present layer receive
 * 
 * @param data Pointer of rx buffer
 * @param len Length of buffer
 * @return ECGP_error 
 */
ECGP_error ECGP_presentRecv(u8* data, u16 len)
{
    ECGP_transportRecv(data, len);
}

/**
 * @brief Present layer initialization
 * 
 */
void ECGP_presentInit(void)
{
    // ECGP_transportInit();
}