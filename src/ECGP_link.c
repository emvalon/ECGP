
/**
  ******************************************************************************
  * File Name          : ECGP_link.c
  * Description        : link layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#include "ECGP_link.h"
#include "ECGP_porting.h"
#include "ECGP_config.h"
#include "ECGP_crc.h"
#include "string.h"


static u8  link_frameBuf[ECGP_LINK_LEN_MAX];
static u16 rx_fifo_out;
static u16 frameIndex;



static ECGP_Link_Fifo   ECGP_rx_fifo, ECGP_tx_fifo;
static u8 _rx_fifo[ECGP_LINK_RX_FIFO_LEN];
static u8 _tx_fifo[ECGP_LINK_TX_FIFO_LEN];

link_callback_typedef ECGP_rx_callback = NULL;
link_callback_typedef ECGP_tx_callback = NULL;

inline static void link_rx_fifo_out_increase(void)
{
    if( ++rx_fifo_out >= ECGP_LINK_RX_FIFO_LEN ){
            rx_fifo_out = 0;
    }
}



/**
* @brief        only used by link layer.
*               Encode data with SLIP style.
*               write the data into frame buffer. 
* @param[in] data   pointer to what will be writen into encode buffer.
* @param[in] len    length of data
* @param[in] index  index of start
* @return       current index
*/
static u16 link_encode(u8* data , u16 len ,u16 index)
{
    u16 i;

    for(i=0; i<len; i++){
        if( data[i] == ECGP_END ){
            link_frameBuf[index++] = ECGP_ESC;
            link_frameBuf[index++] = ECGP_ESC_END;
        }
        else if( data[i] == ECGP_ESC ){
            link_frameBuf[index++] = ECGP_ESC;
            link_frameBuf[index++] = ECGP_ESC_ESC;          
        }
        else
        {
            link_frameBuf[index++] = data[i];
        }
    }
    return index;
}


static ECGP_error link_writeRxfifo(u8* src, u16 len)
{
    u16 i;
    for (i = 0; i < len; i++) {

    }
}
/**
* @brief        only used by link layer.
*               write encode frame data into transmit buffer .
*               tx buffer is used for sending.
* @param[in] len  length of sending data.
* @return         error code
*/
static ECGP_error link_writeTxfifo(u16 len)
{
    u16 len_remain;
    u16 in,out;
    u16 index;
 
    if (ECGP_tx_fifo.full)  
    {
        return -ECGP_EFULL;
    }
    in  = ECGP_tx_fifo.in;
    out = ECGP_tx_fifo.out;
    //judge buffer size
    if ( in >= out)
    {
        len_remain = ECGP_LINK_TX_FIFO_LEN - in + out;
    }
    else{
        len_remain = out - in;
    }
    if (len_remain < len )
    {
        return -ECGP_EFULL;
    }
    //stash
    if( in + len < ECGP_LINK_TX_FIFO_LEN ){
        memcpy(&ECGP_tx_fifo.buf[in],link_frameBuf,len );
    }
    else{
        index = ECGP_LINK_TX_FIFO_LEN-in;
        memcpy(&ECGP_tx_fifo.buf[in], link_frameBuf, index );
        memcpy(ECGP_tx_fifo.buf, &link_frameBuf[index], len-index );
    }
    //update input index 
    ECGP_tx_fifo.in = (in+len)%ECGP_LINK_TX_FIFO_LEN;
    if(ECGP_tx_fifo.in == out){
        ECGP_tx_fifo.full = ECGP_TRUE;
    }
    ECGP_tx_fifo.empty = ECGP_FALSE;
    return ECGP_ENONE;
} 

/**
* @brief    Only used by link layer.
*           Pack data pointed by src with link layer frame. 
*           Write into tx buffer.
* @param[in] src     pointer to what received from upper layer.
* @param[in] len     length of data
* @return    error code
*/
static ECGP_error link_frame( u8* src,  u16 len )
{
    ECGP_error err;
    u16 crc;
    u8 temp[4];

    if (frameIndex > 1) {
        err = link_writeTxfifo(frameIndex);
        if (err != ECGP_ENONE) {
            return err;
        }
        else {
            //write previous message into TX fifo successfully        
        }
    }
    else{
        //no previous message needs to be sent
    }
    
    crc = ECGP_crc16(src,len,LINK_CRC_INIT);
    ECGP_SET_U16(temp,len);
    ECGP_SET_U16(temp+2,crc);
    frameIndex = link_encode(temp,sizeof(temp),1);
    frameIndex = link_encode(src,len, frameIndex);
    link_frameBuf[frameIndex++] = ECGP_END;
    link_frameBuf[frameIndex++] = ECGP_END;

    err = link_writeTxfifo(frameIndex);
    if (err == ECGP_ENONE) {
        frameIndex = 1;
    }
    else {
        //save the value of frameIndex.
        //resend in the next attempt.
    }
    return err;
}

/**
* @brief    Only used by link layer.
*           Unpack data from link layer frame.
* @param[in] dst     pointer to what received from under layer.
* @param[in] len     length of data
* @return    length of read data
*/
static u16 link_parse( u8* dst, u16 len )
{
    u16 cnt=0;
    u8 temp;
    
    while ( (rx_fifo_out != ECGP_rx_fifo.in) && (cnt < len))
    {
        temp = ECGP_rx_fifo.buf[rx_fifo_out];
        if( temp != ECGP_ESC ){
            if( temp != ECGP_END ){
                dst[cnt++] = temp;
            }
            else{
                break;
            }
        }
        else
        {
            link_rx_fifo_out_increase();
            temp = ECGP_rx_fifo.buf[rx_fifo_out];
            if( temp == ECGP_ESC_END ){
                dst[cnt++] = ECGP_END;
            }
            else if(temp == ECGP_ESC_ESC){
                dst[cnt++] = ECGP_ESC;
            }
            else
            {
                //link error
            }
            
        }
        link_rx_fifo_out_increase();
        
    }
    //if read to end,return 0. wait for next parsing.
    if( (rx_fifo_out == ECGP_rx_fifo.in) && (cnt < len) )
        return 0;
    return cnt;
    

} 

/**
* @brief   only used by link layer.
*          Read rx buffer and verify data.
*          write unpacked data into buffer pointed by dst.
* @param[in] dst    pointer to buffer which will stash decoded data.
* @param[in] len    length of buffer
* @return     error code
*/
static ECGP_error link_verfy(u8* dst, u16 len)
{
    u8 temp[2];
    u16 len_recv=0,crc_recv;
    u16 crc,ret;
    while ( (ECGP_rx_fifo.out != ECGP_rx_fifo.in) && (ECGP_rx_fifo.buf[ECGP_rx_fifo.out] == ECGP_END))
    {
        if( ++ECGP_rx_fifo.out >= ECGP_LINK_RX_FIFO_LEN ){
            ECGP_rx_fifo.out = 0;
        }
    }
    //use temporary output index.
    //if read whole package, update rx fifo output index
    rx_fifo_out = ECGP_rx_fifo.out;

    if(rx_fifo_out != ECGP_rx_fifo.in){
        //get data length
        ret = link_parse(temp,sizeof(len_recv));
        if(sizeof(len_recv) != ret ){      
            if(ret != 0){           //error end indentifier
                ECGP_rx_fifo.out = rx_fifo_out;
                return -ECGP_EEND;
            }
            return 0;
        }
        len_recv = ECGP_GET_U16(temp);
        if(len_recv > len){
            return -ECGP_EMEMOUT;
        }
        //get crc value
        ret = link_parse(temp,sizeof(crc_recv));
        if(sizeof(crc_recv) != ret ){   
            if(ret != 0){           //error end indentifier
                ECGP_rx_fifo.out = rx_fifo_out;
                return -ECGP_EEND;
            }
            return 0;
        }
        crc_recv = ECGP_GET_U16(temp);
        //get data
        ret = link_parse(dst,len_recv);
        if(len_recv != ret ){   
            if(ret != 0){           //error end indentifier
                ECGP_rx_fifo.out = rx_fifo_out;
                return -ECGP_EEND;
            }
            return 0;
        }
        //verify crc
        crc = ECGP_crc16(dst,len_recv,LINK_CRC_INIT);
        if(crc != crc_recv){
            len_recv = 0;
        }       
    }
    ECGP_rx_fifo.out = rx_fifo_out;
    return len_recv;
}

/**
* @brief    Used by physical layer.
*           Check if rx fifo is full.
*           If not, call pyh function to receive.
* @param[in] recvLen    length of received data
* @return   error code
*/
ECGP_error link_hasReceived(u16 recvLen)
{
    ECGP_error res;
    u16 in, out, len;
    if (ECGP_rx_fifo.full) {
        return 0;
    }
    in = ECGP_rx_fifo.in;
    out = ECGP_rx_fifo.out;
    if (in < out) {
        len = out - in;
    }
    else {
        len = ECGP_LINK_RX_FIFO_LEN - in;
    }
    //update rx fifo index.
    //if fifo is full, set flag.
    if (recvLen != 0) {
        
        if (len > recvLen) {
            if (ECGP_rx_callback != NULL) {
                ECGP_rx_callback(recvLen);
            }
            len -= recvLen;
            ECGP_rx_fifo.in += recvLen;
            //reset rx fifo empty flag   
            ECGP_rx_fifo.empty = ECGP_FALSE;
        }
        else {
            if (ECGP_rx_callback != NULL) {
                ECGP_rx_callback(len);
            }
            ECGP_rx_fifo.in = (in + len) % ECGP_LINK_RX_FIFO_LEN;
            //reset rx fifo empty flag   
            ECGP_rx_fifo.empty = ECGP_FALSE;
            if (ECGP_rx_fifo.in == out) {
                ECGP_rx_fifo.full = ECGP_TRUE;
                return 0;
            }
            else {
                return link_hasReceived(0);
            }
        }
    }
    
    //call phy function to receive
    res = ECGP_physicalRecv(&ECGP_rx_fifo.buf[ECGP_rx_fifo.in], len);
    if (res == ECGP_ENONE) {
        return len;
    }
    return res;
}

/**
* @brief    Used by physical layer.
*           Update tx fifo output index.
*           if need to send other data, call pyh function.
* @param[in] sendLen    length of sending data
* @return   error code
*/
ECGP_error link_hasSent(u16 sendLen)
{
    ECGP_error res;
    u16 len,in,out;
    //if tx fifo is empty, needn't handle. 
    if(ECGP_tx_fifo.empty){
        return 0;
    }
	
    in = ECGP_tx_fifo.in;
    out = ECGP_tx_fifo.out; 
    if(in > out){
        len =  in - out;      
    }
    else{
        len = ECGP_LINK_TX_FIFO_LEN - out;      
    }
    //update tx fifo index.
    //if fifo is empty, set flag.
    if (sendLen != 0){
        if (len > sendLen) {
            len -= sendLen;
            ECGP_tx_fifo.out += sendLen;
            //reset tx fifo full flag   
            ECGP_tx_fifo.full = ECGP_FALSE;
        }
        else {
            ECGP_tx_fifo.out = (out+len) % ECGP_LINK_TX_FIFO_LEN;
            //reset tx fifo full flag   
            ECGP_tx_fifo.full = ECGP_FALSE;
            if (ECGP_tx_fifo.out == in) {
                ECGP_tx_fifo.empty = ECGP_TRUE;
                return 0;
            }
            else{
                return link_hasSent(0);
            }
        }       
    }
    
    //call phy function to send
    res = ECGP_physicalSend(&ECGP_tx_fifo.buf[ECGP_tx_fifo.out], len);
	if (ECGP_tx_callback != NULL) {
		ECGP_tx_callback(sendLen);
	}
    if( res == ECGP_ENONE ){
        return len;
    }
    return res;
}

/**
* @brief    Only used by network layer..
*           Send data
* @param[in] data    pointer to sending data
* @param[in] len     length of data
* @return   error code
*/
ECGP_error ECGP_linkSend(u8* data, u16 len)
{
    ECGP_error res;
    res = link_frame(data,len);
    if(res != ECGP_ENONE){
        return res;
    }
    res = link_hasSent(0);
    if (res < 0) {
        return res;
    }
    return ECGP_ENONE;
}

/**
* @brief    Only used by network layer..
*           Reveive data.
* @param[in] data    pointer to buffer which will stash decoded data.
* @param[in] len     length of buffer
* @return   error code
*/
ECGP_error ECGP_linkRecv(u8* data, u16 len)
{
    return link_verfy(data,len);
}

/**
* @brief    initialize link layer.
* @return   none
*/
void ECGP_linkInit(void)
{
    ECGP_rx_fifo.buf = _rx_fifo;
    ECGP_tx_fifo.buf = _tx_fifo;

    frameIndex = 1;
    link_frameBuf[0] = ECGP_END;

    ECGP_rx_fifo.in    = 0;
    ECGP_rx_fifo.out   = 0;
    ECGP_rx_fifo.full  = ECGP_FALSE;
    ECGP_rx_fifo.empty = ECGP_TRUE;

    ECGP_tx_fifo.in    = 0;
    ECGP_tx_fifo.out   = 0;
    ECGP_tx_fifo.full  = ECGP_FALSE;
    ECGP_tx_fifo.empty = ECGP_TRUE;
    link_hasReceived(0);
}




















