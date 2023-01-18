/**
 * @file ECGP_mempool.h
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
#ifndef _ECGP_MEMPOOL_H_
#define _ECGP_MEMPOOL_H_

#include <ECGP_common.h>
#include <ext/queue.h>

struct ECGP_mempool
{
    u32 blockSize;
    u32 blockNum;;
    void *startAddr;
    void *endAddr;
    STAILQ_HEAD(, ECGP_memblock);
};

ECGP_error ECGP_mempoolInit(struct ECGP_mempool *mp, u32 blockNum,
                     u32 blockSize, u8 *buffer);

void* ECGP_mempoolMalloc(struct ECGP_mempool *mp);

void ECGP_mempoolFree(struct ECGP_mempool *mp, void *addr);

ECGP_Bool ECGP_mempoolIsFrom(struct ECGP_mempool *mp, void *addr);

u32 ECGP_mempoolGetBlockNum(struct ECGP_mempool *mp);
#endif