/**
  ******************************************************************************
  * File Name          : ECGP_application.c
  * Description        : application layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#include "ECGP_application.h"
#include "ECGP_config.h"









/**********************************************************************
* Description:  Application layer send.
* Input:        point to data buffer, buffer length
* Return:       error code
**********************************************************************/
ECGP_error ECGP_send(u8* data, u16 len)
{
    if(len > ECGP_APPLICATION_LEN_MAX){
        return -ECGP_ELENGTH;
    }
    return ECGP_presentSend(data,len);
}
/**********************************************************************
* Description:  Application layer receive.
* Input:        point to data buffer, buffer length
* Return:       error code
**********************************************************************/
ECGP_error ECGP_recv(u8* data, u16 len)
{
    return ECGP_presentRecv(data,len);
}
/**********************************************************************
* Description:  Application layer receive.
* Input:        point to data buffer, buffer length
* Return:       error code
**********************************************************************/
ECGP_error ECGP_timeElapsed(int time)
{
    return ECGP_presentElapsed(time);
}

/**********************************************************************
* Description:  Set rx callback.
* Input:        point to callback function
* Return:       none
**********************************************************************/
void ECGP_setRxCallback(link_callback_typedef fun)
{
    ECGP_rx_callback = fun;
}

























