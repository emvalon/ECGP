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



static u16 rx_fifo_out;

typedef struct{ 
   u16 in,out;
   ECGP_Bool full; 
   u8 buf[ECGP_LINK_RX_FIFO_LEN];
}ECGP_Link_RxFifo;

static ECGP_Link_RxFifo   ECGP_rx_fifo; 

inline static void link_rx_fifo_out_increase(void)
{
    if( ++rx_fifo_out >= ECGP_LINK_RX_FIFO_LEN ){
            rx_fifo_out = 0;
    }
}

static void link_fifo_init(void)
{
    ECGP_rx_fifo.in     = 1;
    ECGP_rx_fifo.out    = 0;
    ECGP_rx_fifo.full   = ECGP_FALSE;
}
/*
    功能：  将上层数据进行封装
    参数：  sec     输入数据
            dst     输出数据，最大长度为2*len+2
            len     输入数据长度
    返回：  封装后的数据长度
*/
static u16 link_frame( u8* src, u8* dst, u16 len )
{
    u16 i,index=1;
    u16 crc;
    u8 temp[4];
    

    crc = ECGP_crc16(src,len,LINK_CRC_INIT);
    ECGP_SET_U16(temp,len);
    ECGP_SET_U16(temp+2,crc);

    dst[0]  =   ECGP_END;
    for( i=0; i<4; i++ ){
        if( temp[i] == ECGP_END ){
            dst[index++] = ECGP_ESC;
            dst[index++] = ECGP_ESC_END;
        }
        else if( temp[i] == ECGP_ESC ){
            dst[index++] = ECGP_ESC;
            dst[index++] = ECGP_ESC_ESC;
        }
        else
        {
            dst[index++] = temp[i];
        }
    }

    for( i=0; i<len; i++ ){
        if( src[i] == ECGP_END ){
            dst[index++] = ECGP_ESC;
            dst[index++] = ECGP_ESC_END;
        }
        else if( src[i] == ECGP_ESC ){
            dst[index++] = ECGP_ESC;
            dst[index++] = ECGP_ESC_ESC;
        }
        else
        {
            dst[index++] = src[i];
        }
    }
    
    dst[index++]  =   ECGP_END;
    return index;
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

static long link_verfy(u8* dst, u16 len)
{
    u8 temp[2];
    u16 len_recv=0,crc_recv;
    u16 crc,ret;
    while ( (ECGP_rx_fifo.out != ECGP_rx_fifo.in) && (ECGP_rx_fifo.buf[ECGP_rx_fifo.out] == ECGP_END))
    {
        link_rx_fifo_out_increase();
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





























