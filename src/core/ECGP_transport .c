/**
 * @file ECGP_transport .c
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-08
 * 
 * Copyright © 2019-2021 Weilong Shen (valonshen@foxmail.com).
 * All rights reserved.
 * 
 */
#include "ext/queue.h"
#include "ECGP_transport .h"
#include "ECGP_crc.h"
#include "ECGP_config.h"
#include "ECGP_list.h"
#include <string.h>

#define TRANS_SEQ_MASK          0X7Fu
#define TRANS_CRC_INIT          0XFFFFu

typedef struct {
	u16		in;
	u8		buf[ECGP_TRANS_LEN_MAX];
}Transport_interMsgTypeDef;

struct transTx_node{
    STAILQ_ENTRY(transTx_node) next;
	u32		timeout;
	u16		len;
	u8		resend;
	u8		buf[ECGP_TRANS_LEN_MAX]; 
};

static u8 trans_rx_buffer[ECGP_TRANS_LEN_MAX];
static u8 sequence = 0;
static struct transTx_node g_trans_tx_node[ECGP_TRANS_REBUF_MAX];
static Transport_interMsgTypeDef interMsg;

STAILQ_HEAD(transTx_head, transTx_node);
static struct transTx_head g_trans_FreeQ_head;
static struct transTx_head g_trans_waitAckQ_head;
static struct transTx_head g_trans_sendQ_head;

/**
* @brief		Only used by transport layer.
*				Save ack package in inter message buffer. 
*				BIT	|	BIT7	|	BIT6~0
*					|  ack flag |   sequence
* @param[in] seq	Sequence number
* @return		Error code
*/
static ECGP_error transport_sendAck(u8 seq)
{
	seq |= (~TRANS_SEQ_MASK);
	_ECGP_ENTER_CRITICAL;
	if ((interMsg.in + 1) < ECGP_TRANS_LEN_MAX) {
		interMsg.buf[(interMsg.in)++] = seq;
		interMsg.buf[(interMsg.in)++] = 0xffu - seq;
		return ECGP_ENONE;
	}
	else {
		return -ECGP_EMEMOUT;
	}
	_ECGP_LEAVE_CRITICAL;
}

/**
* @brief		Only used by transport layer.
*				Remove node from ackwaiting list.
* @param[in] seq	Sequence number
* @return		None
*/
static void transport_removeAckWaiting(u8 seq)
{ 
	TransTx_node_t* buffer;

	_ECGP_ENTER_CRITICAL;
	buffer = (TransTx_node_t*)ECGP_listGetFirstNode(&waitAckList);
	while (buffer != NULL) 
	{
		if (buffer->buf[0] == seq) {
			ECGP_listDeleteNode(&waitAckList, &buffer->node);
			ECGP_listAddNode(&txFreeList, &buffer->node);
			return;
		}
		buffer = (TransTx_node_t*)ECGP_listGetNextNode(&buffer->node);
	}
	_ECGP_LEAVE_CRITICAL;
}

/**
* @brief		Only used by transport layer.
*				Add node to ackwaiting list.
* @param[in] unit	Pointer to a node.
* @return		None
*/
static void transport_setAckWaiting(TransTx_node_t* unit)
{
	ECGP_listAddNode(&waitAckList, &unit->node);
	unit->timeout = ECGP_TRANS_NOACK_TIMEOUT;
	unit->resend  = ECGP_TRANS_NOACK_RESEND;
}

/**
* @brief		Only used by transport layer.
*				Get sequence number.
* @param[in] unit	None.
* @return		Sequence number 
*/
static u8 transport_getSequence(void)
{ 
    return (sequence++)&TRANS_SEQ_MASK;
}

/**
* @brief		Only used by transport layer.
*				Use tx callback to invoke elapsing function.
* @param		None
* @return		None
*/
static void transport_invokeElapse(void)
{
	if (ECGP_tx_callback != NULL) {
		ECGP_tx_callback(0);
	}
}

/**
* @brief		Only used by transport layer.
*				Add node into sending list.
* @param[in] unit	Pointer to a node.
* @return		None
*/
static void transport_pendSending(TransTx_node_t *unit)
{
	_ECGP_ENTER_CRITICAL;
	ECGP_listAddNode(&sendList, &unit->node);
	_ECGP_LEAVE_CRITICAL;
}

/**
* @brief		Only used by transport layer.
*				Send a node in sendingList to network layer.
* @param		None
* @return		Error code
*/
static ECGP_error transport_sendToNetwork(void)
{
	ECGP_error res = ECGP_ENONE;
	TransTx_node_t* unit;

	_ECGP_ENTER_CRITICAL;
	if (interMsg.in != 0) {
		res = ECGP_networkSend(interMsg.buf, interMsg.in);
		if (res == ECGP_ENONE) {
			interMsg.in = 0;
		}
	}
	_ECGP_LEAVE_CRITICAL;

	while (res == ECGP_ENONE)
	{ 
		_ECGP_ENTER_CRITICAL;	
		unit = (TransTx_node_t*)ECGP_listGetFirstNode(&sendList);
		_ECGP_LEAVE_CRITICAL;
		if (unit == NULL) {
			break;
		}

		res = ECGP_networkSend(unit->buf, unit->len);
		if ( res == ECGP_ENONE){
			_ECGP_ENTER_CRITICAL;
			ECGP_listDeleteNode(&sendList,&unit->node);
			ECGP_listAddNode(&waitAckList, &unit->node);
			_ECGP_LEAVE_CRITICAL;
		}
	}
	return res;
}

/**
* @brief		Transport layer send.
* @param[in] data	Pointer to data buffer.
* @param[in] len	Length of data.
* @return		Error code
*/
ECGP_error ECGP_transportSend(u8* data, u16 len)
{
	TransTx_node_t* freeNode;
    u16 crc;
    u8  seq;
    
	_ECGP_ENTER_CRITICAL;
	freeNode = (TransTx_node_t*)ECGP_listPopFirstNode(&txFreeList);
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
	freeNode->timeout = 0;
	freeNode->resend  = ECGP_TRANS_NOACK_RESEND;
	//ready to send
	transport_pendSending(freeNode);
	//invoke elapsing function
	transport_invokeElapse();
	return ECGP_ENONE;
}

/**
* @brief		Transport layer receive.
* @param[in] data	Pointer to data buffer.
* @param[in] len	Length of buffer.
* @return		Error code ( < 0 ) or length of coming data ( >= 0 ).
*/
u8 seqCur;
#ifndef ECGP_RECV_IN_ORDER
u8 seqPre;
#endif // !ECGP_RECV_IN_ORDER

ECGP_error ECGP_transportRecv(u8* data, u16 len)
{
    ECGP_error readLen;
    u16 crc,crc_recv,i;
	u8 seqRecv;
    readLen = ECGP_networkRecv(trans_rx_buffer,ECGP_TRANS_LEN_MAX);
    if( readLen == 0){
		if (interMsg.in != 0) {
			transport_invokeElapse();
		}
        return readLen;
    }
	else if (readLen < 0) {
		return readLen;
	}
	else if(readLen < 2){
		return ECGP_transportRecv(data, len);
    }

	seqRecv = trans_rx_buffer[0];
    // is ack
    if(seqRecv & (~TRANS_SEQ_MASK))
	{
		for (i = 0; i < readLen; i += 2) 
		{
			if(trans_rx_buffer[i] +trans_rx_buffer[i+1] == 0xff){
				transport_removeAckWaiting(trans_rx_buffer[i]&TRANS_SEQ_MASK );
			}
			else{
				// ignore
			}
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
	if (seqRecv == seqCur) 
	{
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

/**
* @brief		Transport time management.
*				Move the timeout node to sendList from waitAckList.
*				Send nodes in sendList to network layer.
* @param[in] time	Elapsed time.
* @return		Error code
*/
ECGP_error ECGP_transportElapsed(int time)
{
	u32 temp;
	TransTx_node_t* unit;
	ECGP_error err = ECGP_ENONE;
	static u32 elapsedTime	= 0;
	static u32 timeout		= 0;

	elapsedTime += time;
	if ((timeout + elapsedTime) >= ECGP_TRANS_NOACK_TIMEOUT) 
	{
		timeout		= 0;
		_ECGP_ENTER_CRITICAL;
		unit = (TransTx_node_t*)ECGP_listGetFirstNode(&waitAckList);
		while (unit != NULL)
		{
			temp = unit->timeout += elapsedTime;
			if (temp >= ECGP_TRANS_NOACK_TIMEOUT)
			{
				ECGP_listDeleteNode(&waitAckList, &unit->node);
				if (unit->resend != 0) {
					//try to resend this message
					unit->resend -= 1;
					unit->timeout = 0;
					ECGP_listAddNode(&sendList, &unit->node);
				}
				else {
					//can't resend message. communication failure.
					ECGP_listAddNode(&txFreeList, &unit->node);
					err = -ECGP_ECOMM;
				}
			}
			else
			{
				if (temp > timeout) {
					//get the largest time counter.
					timeout = temp;
				}
				else {
					//current time is the largest. 
				}
			}
			unit = (TransTx_node_t*)ECGP_listGetNextNode(&unit->node);
		}
		_ECGP_LEAVE_CRITICAL;
		elapsedTime = 0;
	}
	else
	{
		//no message timeout.
	}

	if (err != ECGP_ENONE) {
		return err;
	}
	else {
		return transport_sendToNetwork();
	}
}

/**
* @brief		Initialize transport layer.
* @param		None
* @return		None
*/
void ECGP_transportInit(void)
{
	u32 i;

	seqCur = 0;
#ifndef ECGP_RECV_IN_ORDER
	seqPre	= 0xff;
#endif // !ECGP_RECV_IN_ORDER
	sequence = 0;

    STAILQ_INIT(&g_trans_FreeQ_head);
    STAILQ_INIT(&g_trans_waitAckQ_head);
    STAILQ_INIT(&g_trans_sendQ_head);

	for (i = 0; i < ECGP_TRANS_REBUF_MAX; i++) {
        STAILQ_INSERT_HEAD(&g_trans_FreeQ_head, &g_trans_tx_node[i], next);
	}
	interMsg.in = 0;

	ECGP_networkInit();
}
















