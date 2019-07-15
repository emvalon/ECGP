/**
  ******************************************************************************
  * File Name          : ECGP_link.h
  * Description        : link layer
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#ifndef _ECGP_LINK_H_
#define _ECGP_LINK_H_

#include "ECGP_network.h"
#include "ECGP_common.h" 

#define LINK_CRC_INIT         0xffffffff
#define ECGP_LINK_LEN_MAX    (2*(ECGP_NET_LEN_MAX+4)+3)

//定义fifo大小
#define ECGP_LINK_RX_FIFO_LEN  (3*ECGP_LINK_LEN_MAX)
#define ECGP_LINK_TX_FIFO_LEN  (3*ECGP_LINK_LEN_MAX)



#define ECGP_END        0xC0	//END	帧结束符
#define	ECGP_ESC        0XDB    //ESC   帧退出符
#define ECGP_ESC_END    0xDC	//	帧结束转义符
#define ECGP_ESC_ESC    0xDD	//	帧退出转义符


typedef struct {
    u16 in, out;
    ECGP_Bool full;
    u8* buf;
}ECGP_Link_Fifo;




void link_init(void);
void link_test(void);



#endif


