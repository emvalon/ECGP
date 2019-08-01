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

static u8 trans_tx_buffer[ECGP_TRANS_LEN_MAX];
static u8 trans_rx_buffer[ECGP_TRANS_LEN_MAX];
static u8 seqBitmap[8],seqGroup=0;
static u8 seq = 0;


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
    u16 handle;
    u8 temp[2];
    temp[0] = seq|(~TRANS_SEQ_MASK);
    temp[1] = 0xffu - temp[0];
    handle = transport_getAckHandler();
    //send to network lager
    return ECGP_networkSend(handle,temp,2);
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
            if(seq == seqBitmap[i]){
                seqGroup ^= temp;
                return;
            }
        }
        temp <<= 1;
    }
}
/**********************************************************************
* Description:  only used by transport layer.
                get ack handle used for marking this message.
* Input:        none
* Return:       error code or handle
**********************************************************************/
static ECGP_error transport_getAckHandler(void)
{
    u8 i, temp = 0x01u;
    for (i = 0; i < 8; i++) {
        if ((seqGroup&temp) == 0) {
            return i;
        }
        temp <<= 1;
    }
    return -ECGP_ESEQ;
}
/**********************************************************************
* Description:  only used by transport layer.
                set ack waiting information.
* Input:        handle,sequence number
* Return:       void
**********************************************************************/
static void transport_setAckWaiting(u16 handle, u8 seq )
{
    seqBitmap[handle] = seq;
    seqGroup |= 0x01u<<handle;
}
static u8 transport_getSequence(void)
{
    if(seqGroup == 0xff){
        return 0xff;
    }
    return (seq++)&TRANS_SEQ_MASK;
}

/**********************************************************************
* Description:  transport layer send.
* Input:        point to data buffer, buffer length
* Return:       error code
**********************************************************************/
ECGP_error ECGP_transportSend(u8* data, u16 len)
{
    ECGP_error res;
    u16 crc,handle;
    //get sequence if no full
    trans_tx_buffer[0] = transport_getSequence();
    if(trans_tx_buffer[0] > TRANS_SEQ_MASK){
        return -ECGP_ESEQ;
    }

    memcpy(&trans_tx_buffer[1],data,len);
    crc = ECGP_crc16(trans_tx_buffer,len+1,TRANS_CRC_INIT);
    ECGP_SET_U16(&trans_tx_buffer[len+1],crc);
    //need ack check
    handle = transport_getAckHandler();
    //send to network lager
    res = ECGP_networkSend(handle,trans_tx_buffer,len+3);
    if (res == ECGP_ENONE) {
        transport_setAckWaiting(handle,trans_tx_buffer[0]);
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























