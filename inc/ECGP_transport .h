/**
 * @file ECGP_transport .h
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

#ifndef _ECGP_TRANSPORT_H_
#define _ECGP_TRANSPORT_H_


#include "ECGP_presentation.h"
#include "ECGP_network.h"
#include "ECGP_common.h"

// Transport layer buffer maximum length
#define ECGP_TRANS_LEN_MAX      (ECGP_PRESENT_LEN_MAX + 3)
//ack timeout value(MS)
#define ECGP_TRANS_NOACK_TIMEOUT    100
//ack timeout resend times
#define ECGP_TRANS_NOACK_RESEND     5
//Resend buffer max number
#define ECGP_TRANS_REBUF_MAX		8


ECGP_error ECGP_transportSend(u8* data, u16 len);
ECGP_error ECGP_transportRecv(u8* data, u16 len);
ECGP_error ECGP_transportElapsed(int time);
void ECGP_transportInit(void);












#endif


