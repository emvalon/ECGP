/**
 * @file ECGP_memchain.h
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-11
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
#ifndef _ECGP_MEMCHAIN_H_
#define _ECGP_MEMCHAIN_H_

#include <ECGP_common.h>
#include <ext/queue.h>

struct ECGP_memchainBlock {
    SLIST_ENTRY(ECGP_memchainBlock) entry;
    /** The available size that can be used in this block. */
    i32 availableSize;
    u8 data[0];
};

struct ECGP_memchainPool {
    SLIST_HEAD(, ECGP_memchainBlock) memblocksHeader;
    u32 totalSize;
};

struct ECGP_memchain {
    i32 leadingBytes;                   /**< The number of leading bytes     */
    i32 dataSize;                       /**< The size of this memchain       */
    struct ECGP_memchainPool *mcp;      /**< The memchainPool allocated from */
    u32 totalSize;                      /**< The total size after catenating */
    SLIST_ENTRY(ECGP_memchain) nextMc;  /**< Used for memory catenating      */
};

#define ECGP_MEMCHAIN_BLOCK_SIZE(bufLen)                                    \
    (bufLen + sizeof(struct ECGP_memchainBlock) + sizeof(struct ECGP_memchain))

/** This is used to define memchain buffer */
#define ECGP_MEMCHAIN_BUF_DEF(name, bufLen, blockNum)                       \
    u8 name[blockNum * ECGP_MEMCHAIN_BLOCK_SIZE(bufLen)]

void ECGP_memchainPoolInit(struct ECGP_memchainPool *mcp);

ECGP_error ECGP_memchainPoolAddBuffer(struct ECGP_memchainPool *mcp, u32 blockSize, 
                                      u32 blockNum, u8 *buffer);

struct ECGP_memchain* ECGP_memchainAlloc(struct ECGP_memchainPool *mcp, u32 len, u32 leading);

u32 ECGP_memchainWrite(struct ECGP_memchain *mc, u8 *buffer, u32 offset, u32 len);

u32 ECGP_memchainRead(struct ECGP_memchain *mc, u8 *buffer, u32 offset, u32 len);

void ECGP_memchainFree(struct ECGP_memchain *mc);

void ECGP_memchainMultiFree(struct ECGP_memchain *mc);

ECGP_error ECGP_memchainInsertLeading(struct ECGP_memchain *mc, u8 *data, u32 len);

ECGP_error ECGP_memchainAdjustLeading(struct ECGP_memchain *mc, u32 len);

u32 ECGP_memchainGetFreeSize(struct ECGP_memchainPool *mc);
#endif