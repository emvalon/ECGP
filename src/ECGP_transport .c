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
#include "ECGP_list.h"
#include <string.h>

#define TRANS_SEQ_MASK          0X7Fu
#define TRANS_CRC_INIT          0XFFFFu


typedef struct TransTx_node {
	ECGP_Node_t* node;
	int timeout;
	u16 len;
	u8 resend;
	u8 buf[ECGP_TRANS_LEN_MAX]; 
}TransTx_node_t;

static ECGP_List_t txFreeList, waitAckList, sendList;

static u8 trans_rx_buffer[ECGP_TRANS_LEN_MAX];
static TransTx_node_t trans_tx_node[ECGP_TRANS_REBUF_MAX];
static u8  sequence=0;


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
                remove node in ackwaiting list.
* Input:        sequence number
* Return:       none
**********************************************************************/ 
static void transport_removeAckWaiting(u8 seq)
{ 
	u8 i;
	TransTx_node_t* buffer;

	_ECGP_ENTER_CRITICAL;
	buffer = (TransTx_node_t*)ECGP_listGetFirstNode(&waitAckList);
	while (buffer != NULL) {
		if (buffer->buf[0] == seq) {
			ECGP_listDeleteNode(&waitAckList, &buffer->node);
			ECGP_listAddNode(&txFreeList, &buffer->node);
			return;
		}
		buffer = (TransTx_node_t*)ECGP_listGetNextNode(&buffer->node);
	}
	_ECGP_LEAVE_CRITICAL;
}

/**********************************************************************
* Description:  only used by transport layer.
                set ack waiting information.
* Input:        handle,sequence number
* Return:       none
**********************************************************************/
static void transport_setAckWaiting(TransTx_node_t* unit)
{
	ECGP_listAddNode(&waitAckList, unit->node);
	unit->timeout = ECGP_TRANS_NOACK_TIMEOUT;
	unit->resend  = ECGP_TRANS_NOACK_RESEND;
}

/**********************************************************************
* Description:  only used by transport layer.
                Get sequence number.
* Input:        none
* Return:       sequence number
**********************************************************************/
static u8 transport_getSequence(void)
{ 
    return (sequence++)&TRANS_SEQ_MASK;
}

/**********************************************************************
* Description:  only used by transport layer.
				use tx callback to invoke elapse function.
* Input:        none
* Return:       none
**********************************************************************/
static void transport_invokeElapse(void)
{
	if (ECGP_tx_callback != NULL) {
		ECGP_tx_callback(0);
	}
}

/**********************************************************************
* Description:  only used by transport layer.
				add node into send list.
* Input:        tx node
* Return:       none
**********************************************************************/
static void transport_pendSending(TransTx_node_t *unit)
{
	_ECGP_ENTER_CRITICAL;
	ECGP_listAddNode(&sendList, &unit->node);
	_ECGP_LEAVE_CRITICAL;
}
/**********************************************************************
* Description:  transport layer resend.
				send node in sendList to network layer.
* Input:        none
* Return:       error code
**********************************************************************/
static ECGP_error transport_sendToNetwork(void)
{
	ECGP_error res;
	TransTx_node_t* unit;
	
	while (true) 
	{
		_ECGP_ENTER_CRITICAL;	
		unit = (TransTx_node_t*)ECGP_listGetFirstNode(&sendList);
		_ECGP_LEAVE_CRITICAL;
		if (unit == NULL) {
			return ECGP_ENONE;
		}

		res = ECGP_networkSend(unit->buf, unit->len);
		if ( res == ECGP_ENONE){
			_ECGP_ENTER_CRITICAL;
			ECGP_listDeleteNode(&sendList,&unit->node);
			ECGP_listAddNode(&waitAckList, &unit->node);
			_ECGP_LEAVE_CRITICAL;
		}
		else if (res == -ECGP_EFULL) {
			return ECGP_ENONE;
		}
		else {
			return res;
		}
	}
}

/**********************************************************************
* Description:  transport layer send.
* Input:        point to data buffer, buffer length
* Return:       error code
**********************************************************************/
ECGP_error ECGP_transportSend(u8* data, u16 len)
{
	TransTx_node_t* freeNode;
    u16 crc;
    u8  seq;
    
	_ECGP_ENTER_CRITICAL;
	freeNode = ECGP_listPopFirstNode(&txFreeList);
	_ECGP_LEAVE_CRITICAL;
    if (freeNode == NULL) {
        return -ECGP_EFULL;
    }
    //get sequence if no full
    seq = transport_getSequence();

	freeNode->buf[0] = seq;
    memcpy(&freeNode->buf[1],data,len);
    crc = ECGP_crc16(freeNode->buf,len+1,TRANS_CRC_INIT);
    ECGP_SET_U16(&freeNode->buf[len+1],crc);
	freeNode->len = len + 3;
	freeNode->timeout = ECGP_TRANS_NOACK_TIMEOUT;
	freeNode->resend = ECGP_TRANS_NOACK_RESEND;
	//ready to send
	transport_pendSending(freeNode);
	//revoke elapse function
	transport_invokeElapse();
	/*
    //send to network layer
    res = ECGP_networkSend(freeNode->buf, freeNode->len);
    if (res == ECGP_ENONE) {
        //need ack check
        transport_setAckWaiting(freeNode);
    }
	else if (res == -ECGP_EFULL) {
		//need be resent in next time
		transport_resendNextTime(freeNode);
	}
	printf("size %d\n", sizeof(TransTx_node_t));
	*/
	return ECGP_ENONE;
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
            transport_removeAckWaiting( seqRecv&TRANS_SEQ_MASK );
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
	TransTx_node_t* unit;

	_ECGP_ENTER_CRITICAL;
	unit = (TransTx_node_t*)ECGP_listGetFirstNode(&waitAckList);
	while (unit != NULL) {
		unit->timeout -= time;
		if (unit->timeout <= 0) {
			if (unit->resend != 0) {
				ECGP_listDeleteNode(&waitAckList, &unit->node);
				unit->resend--;
				unit->timeout = ECGP_TRANS_NOACK_TIMEOUT;
				ECGP_listAddNode(&sendList, &unit->node);
			}
			else {
				return -ECGP_ECOMM;
			}
		}
		unit = ECGP_listGetNextNode(&unit->node);
	}
	_ECGP_LEAVE_CRITICAL;
	return transport_sendToNetwork();
}

/**********************************************************************
* Description:  initialize transport layer.
* Input:        none
* Return:       none
**********************************************************************/

void ECGP_transportInit(void)
{
	u32 i;

	seqCur = 0;
#ifndef ECGP_RECV_IN_ORDER
	seqPre	= 0xff;
#endif // !ECGP_RECV_IN_ORDER
	sequence = 0;
	ECGP_networkInit();
	//init tx free buffer list 
	ECGP_listClear(&txFreeList);
	for (i = 0; i < ECGP_TRANS_REBUF_MAX; i++) {
		ECGP_listAddNode(&txFreeList, &trans_tx_node[i].node);
	}
	//init other list
	ECGP_listClear(&waitAckList);
	ECGP_listClear(&sendList);
}
















