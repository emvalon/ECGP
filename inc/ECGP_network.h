/**
 * @file ECGP_network.h
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
#ifndef _ECGP_NETWORK_H_
#define _ECGP_NETWORK_H_

#include "ECGP_transport .h"
#include "ECGP_link.h"
#include "ECGP_common.h"

#define ECGP_NET_LEN_MAX        ECGP_TRANS_LEN_MAX


#define ECGP_networkSend        ECGP_linkSend
#define ECGP_networkRecv        ECGP_linkRecv
#define ECGP_networkInit		ECGP_linkInit

//ECGP_error ECGP_networkSend(u8* data, u16 len);
//ECGP_error ECGP_networkRecv(u8* data, u16 len);










#endif


