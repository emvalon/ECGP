/**
 * @file ECGP_application.c
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-07
 * 
 * Copyright © 2019-2021 Weilong Shen (valonshen@foxmail.com).
 * All rights reserved.
 * 
 */
#include "ECGP_application.h"
#include "ECGP_config.h"

/**
* @brief        Application layer send.
* @param[in] data   Pointer to data buffer.
* @param[in] len    Length of data
* @return       Error code
*/
ECGP_error ECGP_send(u8* data, u16 len)
{
    if(len > ECGP_APPLICATION_LEN_MAX){
        return -ECGP_ELENGTH;
    }
    // return ECGP_presentSend(data,len);
}

/**
* @brief        Application layer receive.
* @param[in] data   Pointer to data buffer.
* @param[in] len    Length of data buffer.
* @return       Error code ( < 0 ) or length of received data ( >= 0 ).
*/
ECGP_error ECGP_recv(u8* data, u16 len)
{
    // return ECGP_presentRecv(data,len);
}

/**
 * @brief 
 * 
 * @param data 
 * @param len 
 * @return ECGP_error 
 */
ECGP_error ECGP_tryToRecv(u8* data, u16 len)
{
    // return ECGP_presentRecv(data,len);
}

/**
* @brief        Invoke this function periodically to sync time.
*               It should be called in a single time management thread.
*               All messages will be sent in this function.
* @param[in] time   Elapsed time .
* @return       Error code.
*/
ECGP_error ECGP_timeElapsed(int time)
{
    // return ECGP_presentElapsed(time);
}

/**
* @brief        Set rx callback.
*               This function will be called after the completion of receiving.
*               To improve communication speed, it can send a semaphore to receiving thread.
*               Then invoke @ECGP_recv to read coming message.
* @param[in] fun   Pointer to callback function.
* @return       None
*/
// void ECGP_setRxCallback(link_callback_typedef fun)
// {
//     ECGP_rx_callback = fun;
// }

// /**
// * @brief        Set tx callback.
// *               This function will be called after the completion of sending.
// *               To improve communication speed, it can send a semaphore to time management thread.
// *               Then @ECGP_timeElapsed will be invoked and try to send next message.
// * @param[in] fun   Pointer to callback function.
// * @return       None
// */
// void ECGP_setTxCallback(link_callback_typedef fun)
// {
// 	ECGP_tx_callback = fun;
// }

/**
* @brief        Initialize ECGP stack.
* @param        None
* @return       None
*/
void ECGP_init(void)
{
	// ECGP_presentInit();
}























