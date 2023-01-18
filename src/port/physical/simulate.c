/**
 * @file simulate.c
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
#include "ECGP_link.h"
#include "ECGP_porting.h" 
#include "string.h"


u8* bufData;
u16 bufLen;
 ECGP_error ECGP_physicalSend(u8* data, u16 len)
 {
    u16 l;

    l = len > bufLen ? bufLen : len;
    memcpy(bufData, data, l);
    link_hasReceived(l);
    link_hasSent(l); 
	return ECGP_ENONE;
 }


 ECGP_error ECGP_physicalRecv(u8* data, u16 len)
 {
     bufData = data;
     bufLen = len;
	 return ECGP_ENONE;
 }









