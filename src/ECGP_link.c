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


static u8 link_frameBuf[ECGP_LINK_LEN_MAX];
static u16 rx_fifo_out;



static ECGP_Link_Fifo   ECGP_rx_fifo, ECGP_tx_fifo;
static u8 _rx_fifo[ECGP_LINK_RX_FIFO_LEN];
static u8 _tx_fifo[ECGP_LINK_TX_FIFO_LEN];




inline static void link_rx_fifo_out_increase(void)
{
    if( ++rx_fifo_out >= ECGP_LINK_RX_FIFO_LEN ){
            rx_fifo_out = 0;
    }
}

void link_init(void)
{
    ECGP_rx_fifo.buf = _rx_fifo;
    ECGP_tx_fifo.buf = _tx_fifo;

    link_frameBuf[0]    = ECGP_END;
    link_frameBuf[1]    = ECGP_END;

    ECGP_rx_fifo.in     = 0;
    ECGP_rx_fifo.out    = 0;
    ECGP_rx_fifo.full   = ECGP_FALSE;
    ECGP_rx_fifo.empty  = ECGP_FALSE;

    ECGP_tx_fifo.in     = 0;
    ECGP_tx_fifo.out    = 0;
    ECGP_tx_fifo.full   = ECGP_FALSE;
    ECGP_tx_fifo.empty  = ECGP_FALSE;
}


/*
    功能：  对数据编码，保存到buffer
    参数：  data    数据
            len     数据长度
            index   第一个buffer的索引
    返回:   当前buffer的索引
*/
static u16 link_writeBuf(u8* data , u16 len ,u16 index)
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
/*
    功能：  写入数据到发送buffer
    参数：  len    数据长度
    返回：  ECGP_ENONE        写入成功
            -ECGP_EFULL       写入失败,FIFO已满
*/
static ECGP_error link_writeRxfifo(u8* src, u16 len)
{
    u16 i;
    for (i = 0; i < len; i++) {

    }
}
/*
    功能：  写入数据到发送buffer
    参数：  len    数据长度
    返回：  ECGP_ENONE        写入成功
            -ECGP_EFULL       写入失败,FIFO已满
*/
static ECGP_error link_writeTxfifo(u16 len)
{
    u16 i,len_remain;
    u16 in,out;
 
    if (ECGP_tx_fifo.full)  
    {
        return -ECGP_EFULL;
    }
    in  = ECGP_tx_fifo.in;
    out = ECGP_tx_fifo.out;
    //判断剩余缓存大小
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
    //存储
    if( in + len < ECGP_LINK_TX_FIFO_LEN ){
        memcpy(&ECGP_tx_fifo.buf[in],link_frameBuf,len );
    }
    else{
        u16 index = ECGP_LINK_TX_FIFO_LEN-in;
        memcpy(&ECGP_tx_fifo.buf[in], link_frameBuf, index );
        memcpy(ECGP_tx_fifo.buf, &link_frameBuf[index], len-index );
    }
    //更新in的值
    ECGP_tx_fifo.in = (in+len)%ECGP_LINK_TX_FIFO_LEN;
    if(ECGP_tx_fifo.in == out){
        ECGP_tx_fifo.full = ECGP_TRUE;
    }
    return ECGP_ENONE;

} 
/*
    功能：  将上层数据进行封装
    参数：  sec     输入数据
            dst     输出数据，最大长度为2*len+2
            len     输入数据长度
    返回：  ECGP_ENONE     写入正常
            -ECGP_EFULL      缓存不足
*/
ECGP_error link_frame( u8* src,  u16 len )
{
    u16 i,index;
    u16 crc;
    u8 temp[4];

    crc = ECGP_crc16(src,len,LINK_CRC_INIT);
    ECGP_SET_U16(temp,len);
    ECGP_SET_U16(temp+2,crc);

    index = link_writeBuf(temp,sizeof(temp),2);
    index = link_writeBuf(src,len,index);
    link_frameBuf[index++] = ECGP_END;

    return link_writeTxfifo(index);
}


/*
    功能：  将接受缓存的数据解析出来
    参数：  dst     保存数据的数组的地址
            len     保存数据的数组的长度
    返回：  0       没有数据 或 读到了fifo的末尾没有读到完整的包
            其他    读取的数据长度
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
    //在读到fifo末尾时，仍没收到len长度的包，返回0，等待下次解析
    if( (rx_fifo_out == ECGP_rx_fifo.in) && (cnt < len) )
        return 0;
    return cnt;
    

}
/*
    功能：  读取接收缓存的数据，校验出完整的包，忽略错误的包
    参数：  dst     保存数据的数组的地址
            len     保存数据的数组的长度
    返回：  0       没有数据 或 读到了fifo的末尾没有读到完整的包
            <0      接收到错误的包
            其他    读取的数据长度
*/
ECGP_error link_verfy(u8* dst, u16 len)
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
    //使用备份，如果收到完整的包，再去更改ECGP_rx_fifo.out
    rx_fifo_out = ECGP_rx_fifo.out;

    if(rx_fifo_out != ECGP_rx_fifo.in){
        //获取数据长度
        ret = link_parse(temp,sizeof(len_recv));
        if(sizeof(len_recv) != ret ){      
            if(ret != 0){           //错误的end结束符
                ECGP_rx_fifo.out = rx_fifo_out;
                return -ECGP_EEND;
            }
            return 0;
        }
        len_recv = ECGP_GET_U16(temp);
        if(len_recv > len){
            return -ECGP_EMEMOUT;
        }
        //获取crc
        ret = link_parse(temp,sizeof(crc_recv));
        if(sizeof(crc_recv) != ret ){   
            if(ret != 0){           //错误的end结束符
                ECGP_rx_fifo.out = rx_fifo_out;
                return -ECGP_EEND;
            }
            return 0;
        }
        crc_recv = ECGP_GET_U16(temp);
        //获取数据
        ret = link_parse(dst,len_recv);
        if(len_recv != ret ){   
            if(ret != 0){           //错误的end结束符
                ECGP_rx_fifo.out = rx_fifo_out;
                return -ECGP_EEND;
            }
            return 0;
        }
        //校验crc
        crc = ECGP_crc16(dst,len_recv,LINK_CRC_INIT);
        if(crc != crc_recv){
            len_recv = 0;
        }       
    }
    ECGP_rx_fifo.out = rx_fifo_out;
    return len_recv;
}

/*
    功能：  检测发送buffer，调用phy接口发送
    参数：  dst     保存数据的数组的地址
            len     保存数据的数组的长度
    返回：  0       没有数据 或 读到了fifo的末尾没有读到完整的包
            <0      接收到错误的包
            其他    读取的数据长度
*/
u16 link_sendToPhy()
{
    u16 len,in,out;
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

    if(phy_send(&ECGP_tx_fifo.buf[out],len) == ECGP_ENONE ){
        ECGP_tx_fifo.out = (out + len) % ECGP_LINK_TX_FIFO_LEN;
        if(ECGP_tx_fifo.out == in){
            ECGP_tx_fifo.empty = ECGP_TRUE;
        }
        return len;
    }
    return 0;
}

ECGP_error ECGP_linkSend(u8* data, u16 len)
{
    ECGP_error res;
    res = link_frame(data,len);
    if(res != ECGP_ENONE){
        return res;
    }
    ECGP_physicalStartSend();
}

ECGP_error ECGP_linkRecv(u8* data, u16 len)
{
    ECGP_error res;
    res = link_verfy(data,len);
    if(res != ECGP_ENONE){
        return res;
    }
}

void link_test(void)
{
    memcpy(&ECGP_tx_fifo, &ECGP_rx_fifo, sizeof(ECGP_tx_fifo));
}























