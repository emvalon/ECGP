/**
  ******************************************************************************
  * File Name          : ECGP_transport.c
  * Description        : transport layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#include "ECGP_transport .h"
#include "ECGP_crc.h"
#include "ECGP_config.h"
#include "string.h"

#define TRANS_SEQ_MASK          0X7Fu
#define TRANS_CRC_INIT          0XFFFFu

typedef struct {
    int timeout;
    u16 len;
    u8 seq;
    u8 buf[ECGP_TRANS_LEN_MAX];

}TransTx_TypeDef_t;

static u8 trans_rx_buffer[ECGP_TRANS_LEN_MAX];

static TransTx_TypeDef_t trans_tx[8];
static u8 seqGroup=0,seq=0;


/*
    BIT7        BIT6~0
    isAck       sequence
 */
/**********************************************************************
* Description:  only used by transport layer.
                send ack package to network layer. 
* Input:        sequence number
* Return:       error code
**********************************************************************/
static ECGP_error transport_sendAck(u8 seq)
{
    u8 temp[2];
    temp[0] = seq|(~TRANS_SEQ_MASK);
    temp[1] = 0xffu - temp[0];
    //send to network lager
    return ECGP_networkSend(temp,2);
}
/**********************************************************************
* Description:  only used by transport layer.
                process received ack number .
* Input:        sequence number
* Return:       none
**********************************************************************/
static void transport_processAck(u8 seq)
{ 
    u8 i,temp=0x01u;
    if(seqGroup == 0){
        return;
    }
    for(i=0;i<8;i++){
        if(seqGroup&temp){
            if(seq == trans_tx[i].seq){
                seqGroup &= ~temp;
                return;
            }
        }
        temp <<= 1;
    }
}
/**********************************************************************
* Description:  only used by transport layer.
                get ack handle used for marking.
* Input:        none
* Return:       handle
**********************************************************************/
static u8 transport_getAckHandle(void)
{
    u8 i;
    u8 temp = 0x01u;
    if (seqGroup == 0xff) {
        return 0xff;
    }
    for (i = 0; i < 8; i++) {
        if ((seqGroup&temp) == 0) {
            break;
        }
        temp <<= 1;
    }
    return i;
}
/**********************************************************************
* Description:  only used by transport layer.
                set ack waiting information.
* Input:        handle,sequence number
* Return:       none
**********************************************************************/
static void transport_setAckWaiting(u16 handle,u8 seq)
{
    trans_tx[handle].seq = seq;
    seqGroup |= 0x01u << handle;
    trans_tx[handle].timeout = ECGP_TRANS_NOACK_TIMEOUT;
}
/**********************************************************************
* Description:  only used by transport layer.
                Get sequence number.
* Input:        none
* Return:       sequence number
**********************************************************************/
static u8 transport_getSequence(void)
{ 
    return (seq++)&TRANS_SEQ_MASK;
}
/**********************************************************************
* Description:  transport layer resend.
* Input:        handle of resend data
* Return:       error code
**********************************************************************/
static ECGP_error transport_resend(u8 handle)
{
    return ECGP_networkSend(trans_tx[handle].buf, trans_tx[handle].len);
}
/**********************************************************************
* Description:  transport layer send.
* Input:        point to data buffer, buffer length
* Return:       error code
**********************************************************************/
ECGP_error ECGP_transportSend(u8* data, u16 len)
{
    ECGP_error res;
    u16 crc;
    u8 handle,seq;
    handle = transport_getAckHandle();
    if (handle >= 8) {
        return -ECGP_ESEQ;
    }
    //get sequence if no full
    seq = transport_getSequence();

    trans_tx[handle].buf[0] = seq;
    memcpy(&trans_tx[handle].buf[1],data,len);
    crc = ECGP_crc16(trans_tx[handle].buf,len+1,TRANS_CRC_INIT);
    ECGP_SET_U16(&trans_tx[handle].buf[len+1],crc);
    trans_tx[handle].len = len + 3;
    //send to network lager
    res = ECGP_networkSend(trans_tx[handle].buf, trans_tx[handle].len);
    if (res == ECGP_ENONE) {
        //need ack check
        transport_setAckWaiting(handle,seq);
    }
    return res;
}

/**********************************************************************
* Description:  transport layer receive.
* Input:        point to data buffer, buffer length
* Return:       error code
**********************************************************************/
ECGP_error ECGP_transportRecv(u8* data, u16 len)
{
    ECGP_error readLen;
    u16 crc,crc_recv;
    readLen = ECGP_networkRecv(trans_rx_buffer,ECGP_TRANS_LEN_MAX);
    if( readLen <= 0){
        return readLen;
    }else if(readLen < 2){
        return 0;
    }
    // is ack
    if(trans_rx_buffer[0] & (~TRANS_SEQ_MASK)){
        if(trans_rx_buffer[0]+trans_rx_buffer[1] == 0xff){
            transport_processAck(trans_rx_buffer[0]&TRANS_SEQ_MASK);
        }
        else{
            // ignore
        }
        //check if there is any data left
        return ECGP_transportRecv(data,len);
    }
    // is not ack
    readLen -= 2;
    crc = ECGP_crc16(trans_rx_buffer,readLen,TRANS_CRC_INIT);
    crc_recv = ECGP_GET_U16(&trans_rx_buffer[readLen]);
    if(crc != crc_recv){
        // ignore , continue to check
        return  ECGP_transportRecv(data,len);
    }
    // send ack
    
    if(transport_sendAck(trans_rx_buffer[0]) != ECGP_ENONE){
        return -ECGP_ESENDACK;
    }
    // copy data
    if(--readLen > len){
        readLen = len;
    }
    memcpy(data,&trans_rx_buffer[1],readLen);
    return readLen;
}

/**********************************************************************
* Description:  transport time manage.
* Input:        elapased time(ms)
* Return:       error code
**********************************************************************/
ECGP_error ECGP_transportElapsed(int time)
{
    u8 i,temp=0x01u;
    ECGP_error res;

    if (seqGroup == 0) {
        return ECGP_ENONE;
    }
    for (i = 0; i < 8; i++) {
        if (seqGroup&temp) {
            trans_tx[i].timeout -= time;
            if (trans_tx[i].timeout <= 0) {
                res = transport_resend(i);
                if (res == ECGP_ENONE) {
                    trans_tx[i].timeout = ECGP_TRANS_NOACK_TIMEOUT;
                }
                else {
                    return res;
                }
            }
        }
        temp <<= 1;
    }
    return ECGP_ENONE;
}




















