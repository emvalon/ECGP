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
    u8 seq,resend;
    u8 buf[ECGP_TRANS_LEN_MAX];
}TransTx_TypeDef_t;

static u8 trans_rx_buffer[ECGP_TRANS_LEN_MAX];

static TransTx_TypeDef_t trans_tx[ECGP_TRANS_REBUF_MAX];
static u8  seq=0;
static u32 seqGroup = 0;


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
	u8 i;
	u32 temp = 0x00000001u;
    if(seqGroup == 0){
        return;
    }
    for(i=0;i< ECGP_TRANS_REBUF_MAX;i++){
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
	static u32 groupMask = 0x00000001u << ECGP_TRANS_REBUF_MAX -1;
    u8 i;
    u32 temp = 0x00000001u;
    if (seqGroup >= groupMask) {
        return 0xffffffffu;
    }
    for (i = 0; i < ECGP_TRANS_REBUF_MAX; i++) {
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
	trans_tx[handle].resend  = ECGP_TRANS_NOACK_RESEND;
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
    if (handle >= ECGP_TRANS_REBUF_MAX) {
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
u8 seqCur;
#ifndef ECGP_RECV_IN_ORDER
u8 seqPre;
#endif // !ECGP_RECV_IN_ORDER

ECGP_error ECGP_transportRecv(u8* data, u16 len)
{
    ECGP_error readLen;
    u16 crc,crc_recv;
	u8 seqRecv;
    readLen = ECGP_networkRecv(trans_rx_buffer,ECGP_TRANS_LEN_MAX);
    if( readLen <= 0){
        return readLen;
    }else if(readLen < 2){
        return 0;
    }

	seqRecv = trans_rx_buffer[0];
    // is ack
    if(seqRecv & (~TRANS_SEQ_MASK)){
        if(seqRecv +trans_rx_buffer[1] == 0xff){
            transport_processAck( seqRecv&TRANS_SEQ_MASK );
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
	//Judge sequeue
	if (seqRecv == seqCur) {
		// send ack 
		if(transport_sendAck(seqRecv) != ECGP_ENONE){
			return -ECGP_ESENDACK;
		}
		// copy data
		if(--readLen > len){
			readLen = len;
		}
		memcpy(data,&trans_rx_buffer[1],readLen);
		seqCur++;
		return readLen;
		 
	}
	else {
		return ECGP_transportRecv(data, len);
	}

		   

    
}

/**********************************************************************
* Description:  transport time manage.
* Input:        elapased time(ms)
* Return:       error code
**********************************************************************/
ECGP_error ECGP_transportElapsed(int time)
{
	u8 i;
	u32 temp = 0x00000001u;
    ECGP_error res=0;

    if (seqGroup == 0) {
        return ECGP_ENONE;
    }
    for (i = 0; i < ECGP_TRANS_REBUF_MAX; i++) {
        if (seqGroup&temp) {
            trans_tx[i].timeout -= time;
            if (trans_tx[i].timeout <= 0) {
				if (trans_tx[i].resend != 0) {
					res = transport_resend(i);
					if (res == ECGP_ENONE) {	
						trans_tx[i].resend--;
						trans_tx[i].timeout = ECGP_TRANS_NOACK_TIMEOUT;
					}
					else {
						return res;
					}
				}
				else {
					return -ECGP_ECOMM;
				}
            }
        }
        temp <<= 1;
    }
    return ECGP_ENONE;
}

/**********************************************************************
* Description:  initialize transport layer.
* Input:        none
* Return:       none
**********************************************************************/
void ECGP_transportInit(void)
{
	seqCur = 0;
#ifndef ECGP_RECV_IN_ORDER
	seqPre	= 0xff;
#endif // !ECGP_RECV_IN_ORDER
	seq = 0;
	seqGroup = 0;
	ECGP_networkInit();
}
















