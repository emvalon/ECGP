/**
 * @file ECGP_mempool.c
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
#include <ext/queue.h>
#include <misc/ECGP_mempool.h>
#include <string.h>

struct ECGP_memblock
{
    
    STAILQ_ENTRY(ECGP_memblock) next;
};

/**
 * @brief This is called to initialize memory pool.
 * 
 * @param mp Pointer of memory pool
 * @param blockNum Number of blocks in memory pool
 * @param blockSize Size of every block
 * @param buffer Memory buffer (size >= blockNum * blockSize)
 */
ECGP_error ECGP_mempoolInit(struct ECGP_mempool *mp, u32 blockNum,
                            u32 blockSize, u8 *buffer)
{
    struct ECGP_memblock *block;

    ECGP_ASSERT(mp);
    ECGP_ASSERT(buffer);

    if (!blockNum || !blockSize) {
        return ECGP_EPARAM;
    }

    if (blockSize < sizeof(struct ECGP_memblock)) {
        return ECGP_EPARAM;
    }

    memset(mp, 0, sizeof(*mp));

    mp->startAddr = (void*)buffer;
    mp->blockNum = blockNum;

    STAILQ_INIT(mp);
    for(blockNum; blockNum > 0; blockNum--) {
        block = (struct ECGP_memblock*)buffer;
        STAILQ_INSERT_TAIL(mp, block, next);
        buffer += blockSize;
    }

    mp->endAddr = (void*)(buffer - blockSize);

    return 0;
}

/**
 * @brief This is called to get a block form given pool.
 * 
 * @param mp Pointer of memory pool
 * @return void* Address of block.
 *               NULL if no free block
 */
void* ECGP_mempoolMalloc(struct ECGP_mempool *mp)
{
    struct ECGP_memblock *block;

    ECGP_ASSERT(mp);

    block = STAILQ_FIRST(mp);

    if(block) {
        ECGP_ASSERT(mp->blockNum > 0);

        STAILQ_REMOVE_HEAD(mp,next);
        --mp->blockNum;
    } else {
        ECGP_ASSERT(mp->blockNum == 0);
    }

    return (void*)block;
}

/**
 * @brief This is called to free a block to given pool.
 * 
 * @param mp Pointer of memory pool
 * @param addr Address of block
 */
void ECGP_mempoolFree(struct ECGP_mempool *mp, void *addr)
{
    struct ECGP_memblock *block;

    ECGP_ASSERT(mp);
    ECGP_ASSERT(addr);

    block = (struct ECGP_memblock*)addr;
    STAILQ_INSERT_TAIL(mp, block, next);  

    ++mp->blockNum;  
} 

/**
 * @brief This is called to check if block is allocated 
 * form given pool.
 * 
 * @param mp Pointer of memory pool
 * @param addr Address of block 
 * @return ECGP_Bool
 */
ECGP_Bool ECGP_mempoolIsFrom(struct ECGP_mempool *mp, void *addr)
{
    if((addr >= mp->startAddr) && (addr <= mp->endAddr)) {
        return ECGP_TRUE;
    }
    return ECGP_FALSE;
}

/**
 * @brief This is called to get the number of free blocks left.
 * 
 * @param mp Pointer of memory pool
 * @return u32
 */
u32 ECGP_mempoolGetBlockNum(struct ECGP_mempool *mp)
{
    ECGP_ASSERT(mp);

    return mp->blockNum;
}