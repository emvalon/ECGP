/**
 * @file ECGP_session.h
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
#ifndef _ECGP_SESSION_H_
#define _ECGP_SESSION_H_

#include "queue.h"
#include "ECGP_porting.h"
#include "ECGP_common.h"
#include "ECGP_config.h"

struct ECGP_socket {
    SLIST_ENTRY(ECGP_socket) entry;
};

void ECGP_sessionInit(void);

ECGP_error ECGP_sessionSocketInit(struct ECGP_socket *socket);

u32 ECGP_sessionWrite(struct ECGP_socket *socket, u8 *data, u32 len);

u32 ECGP_sessionRead(struct ECGP_socket *socket, u8 *buffer, u32 len);

#endif