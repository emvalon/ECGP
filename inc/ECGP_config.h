/**
 * @file ECGP_config.h
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
#ifndef _ECGP_CONFIG_H_
#define _ECGP_CONFIG_H_

/*
 * Using crc tabel can improve the computing speed.
 * The other hand, it will take up more space.
 */
#define ECGP_USE_CRC8_TABLE 
#define ECGP_USE_CRC16_TABLE 
#define ECGP_USE_CRC32_TABLE 

/*
 * If you want to receive message in order,
 * Pls define this macro. 
 */
#define ECGP_RECV_IN_ORDER

/*
 * In order to protect critical data ,
 * Pls define these macro.
 */
#define _ECGP_ENTER_CRITICAL(_sr)
#define _ECGP_EXIT_CRITICAL(_sr)


/* Implement assert interface */
#ifndef ECGP_ASSERT
#include <stdio.h>
#if 0
#define ECGP_ASSERT(cond)
#else
#define ECGP_ASSERT(cond)                       \
    if(!(cond)) {                               \
        printf("assertion:%s,%d,%s\n",#cond,    \
                                     __LINE__,  \
                                     __func__); \
        while(1);                               \
    }
#endif
#endif
#endif


