/**
 * @file ECGP_porting.h
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
#ifndef _ECGP_PHYSICAL_H_
#define _ECGP_PHYSICAL_H_

#include <stddef.h>
#include "ECGP_common.h"
#include "ECGP_config.h"

void* ECGP_createSemaphore(void);

u8 ECGP_takeSemaphore(void *semaphore, u32 waitTime);

void ECGP_giveSemaphore(void *semaphore);

u32 ECGP_getTime(void);
#endif
