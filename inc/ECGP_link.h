/**
 * @file ECGP_link.h
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-12
 * 
 * Copyright © 2021 - 2022 Weilong Shen (valonshen@foxmail.com)
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 */
#ifndef _ECGP_LINK_H_
#define _ECGP_LINK_H_

#include "ECGP_network.h"
#include "ECGP_porting.h" 
#include "ECGP_common.h"

#define LINK_CRC_INIT         0xffffu
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
    ECGP_Bool empty;
    u8* buf;
}ECGP_Link_Fifo;

extern link_callback_typedef ECGP_rx_callback;
extern link_callback_typedef ECGP_tx_callback;

ECGP_error ECGP_linkSend(u8* data, u16 len);
ECGP_error ECGP_linkRecv(u8* data, u16 len);
void ECGP_linkInit(void);


#endif


