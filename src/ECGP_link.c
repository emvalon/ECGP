/**
  ******************************************************************************
  * File Name          : ECGP_link.c
  * Description        : link layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#include "ECGP_link.h"
#include "ECGP_config.h"
#include "ECGP_common.h" 
#include "ECGP_crc.h"

#define ECGP_LINK_RX_FIFO_LEN  4096
#define ECGP_LINK_TX_FIFO_LEN  4096


static u16 rx_fifo_out;
static u16 tx_fifo_in;

typedef struct{ 
   u16 in,out;
   ECGP_Bool full; 
   u8 buf[ECGP_LINK_RX_FIFO_LEN];
}ECGP_Link_RxFifo;

typedef struct{ 
   u16 in,out;
   ECGP_Bool full; 
   u8 buf[ECGP_LINK_TX_FIFO_LEN];
}ECGP_Link_TxFifo;

static ECGP_Link_RxFifo   ECGP_rx_fifo; 
static ECGP_Link_TxFifo   ECGP_tx_fifo; 


inline static void link_rx_fifo_out_increase(void)
{
    if( ++rx_fifo_out >= ECGP_LINK_RX_FIFO_LEN ){
            rx_fifo_out = 0;
    }
}

static void link_fifo_init(void)
{
    ECGP_rx_fifo.in     = 0;
    ECGP_rx_fifo.out    = 0;
    ECGP_rx_fifo.full   = ECGP_FALSE;

    ECGP_tx_fifo.in     = 0;
    ECGP_tx_fifo.out    = 0;
    ECGP_tx_fifo.full   = ECGP_FALSE;
}

static u8 link_tx_fifo_in_increase()
{
    if(tx_fifo_in >= ECGP_LINK_TX_FIFO_LEN){
        tx_fifo_in = 0;
    }
    if(tx_fifo_in == ECGP_tx_fifo.out){
        return 1;
    }
    return 0;
}
/*
    功能：  写入数据到发送buffer
    参数：  data    数据
    返回：  ECGP_ENONE        写入成功
            ECGP_EFULL        写入成功，fifo已满
            -ECGP_EFULL       写入失败,FIFO已满
*/
static ECGP_error link_writeTxfifo(u8 data)
{
    if(ECGP_tx_fifo.full){
        return 0;
    }
    else{
        if( data == ECGP_END ){
            ECGP_tx_fifo.buf[tx_fifo_in] = ECGP_ESC;
            if( link_tx_fifo_in_increase() ){
                return -ECGP_EFULL;
            }
            ECGP_tx_fifo.buf[tx_fifo_in] = ECGP_ESC_END;
        }
        else if( data == ECGP_ESC ){
            ECGP_tx_fifo.buf[tx_fifo_in] = ECGP_ESC;
            if( link_tx_fifo_in_increase() ){
                return -ECGP_EFULL;
            }
            ECGP_tx_fifo.buf[tx_fifo_in] = ECGP_ESC_ESC;           
        }
        else
        {
            ECGP_tx_fifo.buf[tx_fifo_in] = data;
        }

        if( link_tx_fifo_in_increase() ){
            return ECGP_EFULL;
        }
        return ECGP_ENONE;
    }
} 
/*
    功能：  将上层数据进行封装
    参数：  sec     输入数据
            dst     输出数据，最大长度为2*len+2
            len     输入数据长度
    返回：  
*/
static ECGP_error link_frame( u8* src,  u16 len )
{
    u16 i;
    u16 crc;
    u8 temp[4];

    if(ECGP_tx_fifo.full ){
        return -ECGP_EFULL;
    }
    if (len < 1)
    {
        return -ECGP_EPARA;
    }
    
    crc = ECGP_crc16(src,len,LINK_CRC_INIT);
    ECGP_SET_U16(temp,len);
    ECGP_SET_U16(temp+2,crc);

    tx_fifo_in = ECGP_rx_fifo.in;
    //添加2个结束符
    for（ i=0; i<2; i++ ）{
        ECGP_tx_fifo.buf[tx_fifo_in] = ECGP_END;
        if( link_tx_fifo_in_increase() ){
            return -ECGP_EFULL;
        }
    }
    //添加帧头
    for( i=0; i<4; i++ ){
        if( link_writeTxfifo(temp[i]) != ECGP_ENONE ){
            return -ECGP_EFULL;
        }
    }
    //添加数据
    for( i=0; i<len; i++ ){
        if( link_writeTxfifo(src[i]) != ECGP_ENONE ){
            return -ECGP_EFULL;
        }
    }
    //添加结束符
    ECGP_tx_fifo.buf[tx_fifo_in] = ECGP_END;
    if ( link_tx_fifo_in_increase() )
    {
        ECGP_tx_fifo.full = ECGP_TRUE;
    }
    ECGP_tx_fifo.in = tx_fifo_in;
    return ECGP_ENONE;
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





























