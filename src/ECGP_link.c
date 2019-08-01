
/**
  ******************************************************************************
  * File Name          : ECGP_link.c
  * Description        : link layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#include "ECGP_link.h"
#include "ECGP_physical.h"
#include "ECGP_config.h"
#include "ECGP_crc.h"
#include "string.h"

#if ECGP_SENDBUF_MAX <= 8
static u8 bufBitmap = 0;
#else
#error "The buffer number is too big!"
#endif

typedef struct {
    u16 len;
    u8 buffer[ECGP_LINK_LEN_MAX];
}link_buffer_t;

static link_buffer_t link_frameBuf[ECGP_SENDBUF_MAX];
static u16 rx_fifo_out;



static ECGP_Link_Fifo   ECGP_rx_fifo;
static u8 _rx_fifo[ECGP_LINK_RX_FIFO_LEN];




inline static void link_rx_fifo_out_increase(void)
{
    if( ++rx_fifo_out >= ECGP_LINK_RX_FIFO_LEN ){
            rx_fifo_out = 0;
    }
}




/**********************************************************************
* Description:  only used by link layer.
                write data into frame buffer. 
* Input:        handle used for marking this message,data pointer, 
                data length, index of start 
* Return:       current index
**********************************************************************/
static u16 link_writeBuf(u16 handle, u8* data , u16 len ,u16 index)
{
    u16 i;

    for(i=0; i<len; i++){
        if( data[i] == ECGP_END ){
            link_frameBuf[handle].buffer[index++] = ECGP_ESC;
            link_frameBuf[handle].buffer[index++] = ECGP_ESC_END;
        }
        else if( data[i] == ECGP_ESC ){
            link_frameBuf[handle].buffer[index++] = ECGP_ESC;
            link_frameBuf[handle].buffer[index++] = ECGP_ESC_ESC;
        }
        else
        {
            link_frameBuf[handle].buffer[index++] = data[i];
        }
    }
    return index;
}
/**********************************************************************
* Description:  only used by link layer.
* Input:        sequence number
* Return:       error code
**********************************************************************/
static ECGP_error link_writeRxfifo(u8* src, u16 len)
{
    u16 i;
    for (i = 0; i < len; i++) {

    }
}

/**********************************************************************
* Description:  Only used by link layer.
                Pack data pointed by src with link layer frame. 
                Write into tx buffer.
* Input:        handle used for marking this message.date pointer, 
                data length .
* Return:       error code
**********************************************************************/
static ECGP_error link_frame(  u16 handle, u8* src,  u16 len )
{
    u16 i,index;
    u16 crc;
    u8 temp[4];

    if (handle < ECGP_SENDBUF_MAX) {
        crc = ECGP_crc16(src,len,LINK_CRC_INIT);
        ECGP_SET_U16(temp,len);
        ECGP_SET_U16(temp+2,crc);

        index = link_writeBuf(handle,temp,sizeof(temp),2);
        index = link_writeBuf(handle,src,len,index);
        link_frameBuf[handle].buffer[index++] = ECGP_END;
        link_frameBuf[handle].len = index;
        return ECGP_ENONE;
    }
    else {
        return -ECGP_EFULL;
    }
    
}


/**********************************************************************
* Description:  Only used by link layer.
                Unpack data from link layer frame.
* Input:        pointer of stashing address, receive buffer size
* Return:       length of read data
**********************************************************************/
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
/**********************************************************************
* Description:  only used by link layer.
                Read rx buffer and verify data.
                write unpacked data into buffer pointed by dst.
* Input:        stashing data buffer, buffer length
* Return:       error code
**********************************************************************/
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
/**********************************************************************
* Description:  Used by physical layer.
                Check if rx fifo is full.
                If not, call pyh function to receive.
* Input:        length of received data
* Return:       length of fifo remaining data or error code
**********************************************************************/
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
            len -= recvLen;
            ECGP_rx_fifo.in += recvLen;
            //reset rx fifo empty flag   
            ECGP_rx_fifo.empty = ECGP_FALSE;
        }
        else {
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
/**********************************************************************
* Description:  Used by physical layer.
                Update tx fifo output index.
                if need to send other data, call pyh function.
* Input:        length of sent data
* Return:       length of next sending data or error code
**********************************************************************/
ECGP_error link_hasSent(u16 sendLen)
{
    ECGP_error res;
    static u16 currenthandle;
    
#if ECGP_SENDBUF_MAX <= 8
    u8 temp = 0x01u;
    //if tx buffer is empty, needn't handle. 
    if(bufBitmap){
        return 0;
    }
    if (sendLen!=0 && sendLen==link_frameBuf[currenthandle].len){
        bufBitmap &= ~(0x01u << currenthandle);
        if (bufBitmap) {
            return 0;
        }
    }
    for (u8 i = 0; i < ECGP_SENDBUF_MAX; i++) {
        if (bufBitmap&temp) {
            currenthandle = i;
        }
        temp <<= 1;
    }
#endif
     
    //call phy function to send
    res = ECGP_physicalSend(link_frameBuf[currenthandle].buffer, link_frameBuf[currenthandle].len);
    if( res == ECGP_ENONE ){
        return link_frameBuf[currenthandle].len;
    }
    return res;
}
/**********************************************************************
* Description:  Only used by network layer.
                Send data
* Input:        handle used for marking this message,data address,
                length of sent data.
* Return:       error code
**********************************************************************/
ECGP_error ECGP_linkSend(u16 handle, u8* data, u16 len )
{
    ECGP_error res;
    res = link_frame(handle,data,len);
    if(res != ECGP_ENONE){
        return res;
    }
    res = link_hasSent(0);
    if (res < 0) {
        return res;
    }
    return ECGP_ENONE;
}
/**********************************************************************
* Description:  Only used by network layer.
                Reveive data.
* Input:        data address,length of sent data
* Return:       error code
**********************************************************************/
ECGP_error ECGP_linkRecv(u8* data, u16 len)
{
    ECGP_error res;
    res = link_verfy(data,len);
    if(res != ECGP_ENONE){
        return res;
    }
}



void link_init(void)
{
    memset(link_frameBuf, 0, sizeof(link_frameBuf));
    ECGP_rx_fifo.buf = _rx_fifo;
    ECGP_rx_fifo.in = 0;
    ECGP_rx_fifo.out = 0;
    ECGP_rx_fifo.full = ECGP_FALSE;
    ECGP_rx_fifo.empty = ECGP_TRUE;

    link_hasReceived(0);
}




















