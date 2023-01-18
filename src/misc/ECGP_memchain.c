/**
 * @file ECGP_memchain.c
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
#include <misc/ECGP_memchain.h>
#include <misc/ECGP_mempool.h>
#include <ext/queue.h>
#include <string.h>

/**
 * @brief This is used to copy data form/to memory chain.
 * 
 * @param mc Pointer of memchain
 * @param buffer Pointer of data
 * @param offset The offset that we will copy from
 * @param len Length to read/write
 * @param isWrite 
 * @return u32 
 *  Actual length of read/written data
 */
static u32 ECGP_memchainCopy(struct ECGP_memchain *mc, u8 *buffer, 
                             u32 offset, u32 len, ECGP_Bool isWrite)
{
    struct ECGP_memchainBlock *block;
    u32 copyLen;
    u32 rc;
    u8 *pData;

    rc = 0;
    /* Get the max length that we can read. */
    if (!isWrite) {
        len = ECGP_MIN(len, mc->dataSize - offset);
    }
    block = (struct ECGP_memchainBlock*)((void *)mc -
            offsetof(struct ECGP_memchainBlock, data));
    /* Calculate the actual data offset and get the pointer of valid data */
    offset += sizeof(struct ECGP_memchain) + mc->leadingBytes;
    while (offset >= block->availableSize) {
        offset -= block->availableSize;
        block   = SLIST_NEXT(block, entry);
        if (!block) {
            return 0;
        }
    }

    while (len && block) {
        pData = &block->data[offset];

        copyLen = ECGP_MIN(len, block->availableSize - offset);
        if (isWrite) {
            memcpy(pData, buffer, copyLen);
        } else {
            memcpy(buffer, pData, copyLen);
        }

        len -= copyLen;
        rc  += copyLen;
        buffer += copyLen;
        offset = 0;
        block = SLIST_NEXT(block, entry);
    }

    return rc;
}

/**
 * @brief This is called to initialize memchainPool.
 * Must invoke before using corresponding memchainPool.
 * 
 * @param mcp Pointer of memchainPool
 */
void ECGP_memchainPoolInit(struct ECGP_memchainPool *mcp)
{
    ECGP_ASSERT(mcp);

    memset(mcp, 0, sizeof(*mcp));
    SLIST_INIT(&mcp->memblocksHeader);
}

/**
 * @brief This is called to add memory buffer to memchainPool.
 * Must invoke after initializing memchainPool. These buffer will be 
 * used in ECGP_memchianMalloc().
 * 
 * @param mcp Pointer of memchainPool
 * @param blockSize Size of every block. 
 *                  Use ECGP_MEMCHAIN_BLOCK_SIZE() to define.
 * @param blockNum Number of blocks
 * @param buffer Memory buffer
 * @return ECGP_error 
 */
ECGP_error ECGP_memchainPoolAddBuffer(struct ECGP_memchainPool *mcp, 
                                      u32 blockSize, u32 blockNum, u8 *buffer)
{
    u32 i;
    u32 availableSize;
    struct ECGP_memchainBlock *block, *after;

    ECGP_ASSERT(mcp);
    ECGP_ASSERT(buffer);

    if(!blockNum || (blockSize < ECGP_MEMCHAIN_BLOCK_SIZE(0))) {
        return ECGP_EPARAM;
    }

    availableSize = blockSize - sizeof(struct ECGP_memchainBlock);
    /* According to the size of memblock, check where new 
     * blocks should be inserted.
     */
    after = NULL;
    SLIST_FOREACH(block, &mcp->memblocksHeader, entry) {
        if(availableSize <= block->availableSize) {
            break;
        }
        after = block;
    }

    for(i = 0; i < blockNum; i++) {
        block = (struct ECGP_memchainBlock*)buffer;
        block->availableSize = availableSize;

        if(after) {
            SLIST_INSERT_AFTER(after, block, entry);
        } else {
            SLIST_INSERT_HEAD(&mcp->memblocksHeader, block, entry);
        }
        buffer += blockSize;
    }

    /* If Malloc is called, we need to reserve some space for memchain header.
     * Therefor, totalSize isn't the actual length that we can use.
     */
    mcp->totalSize += blockNum * availableSize;

    return ECGP_ENONE;
}

/**
 * @brief This is called to allocate memchain from given pool.
 * The total size of allocated memchain may be larger than input length.
 * 
 * @param mcp Pointer of memchainPool
 * @param len Expected Length
 * @param leading Length of leading bytes
 * @return struct ECGP_memchain* 
 *  Pointer of allocated memchain
 */
struct ECGP_memchain* ECGP_memchainAlloc(struct ECGP_memchainPool *mcp, 
                                          u32 len, u32 leading)
{
    struct ECGP_memchainBlock *block;
    struct ECGP_memchainBlock *usedBlock;
    struct ECGP_memchainBlock *lastBlock;
    struct ECGP_memchain *mc;
    u32 remainingLen;

    remainingLen = len + leading;
    if(!remainingLen) {
        return NULL;
    }
    /* Leading bytes can be inserted before valid buffer. 
     * We need to check if have enough memory. 
     */
    remainingLen += sizeof(struct ECGP_memchain);
    if (remainingLen > mcp->totalSize) {
        return NULL;
    }

    lastBlock = NULL;
    while((i32)remainingLen > 0) {
        /* Try to get the most suitable block. If the input length is too large,
         * we will use the last block.
         */
        SLIST_FOREACH(block, &mcp->memblocksHeader, entry) {
            usedBlock = block;
            if(block->availableSize >= remainingLen) {
                break;
            }
        }

        ECGP_ASSERT(usedBlock);

        /* TODO: This may not efficient. Need to be optimized */
        SLIST_REMOVE(&mcp->memblocksHeader, usedBlock, ECGP_memchainBlock, entry);

        SLIST_NEXT(usedBlock, entry) = NULL;

        if(!lastBlock) {
            /* If this is the first memblock, initialize memchain header. 
             * TotalSize doesn't contain leading data.
             */
            mc = (struct ECGP_memchain*)usedBlock->data;
            mc->mcp = mcp;
            mc->leadingBytes = leading;
            mc->dataSize = 0;
            mc->totalSize = 0;
            SLIST_NEXT(mc, nextMc) = NULL;
        } else {
            SLIST_NEXT(lastBlock, entry) = usedBlock;
        }
        lastBlock = usedBlock;
        
        /* Allocated length may be larger than input len */
        remainingLen -= usedBlock->availableSize;
        mcp->totalSize -= usedBlock->availableSize;
    }

    return mc;
}

/**
 * @brief This is called to copy data into given memchain.
 * 
 * @param mc Pointer of allocated memchain
 * @param buffer Pointer of data
 * @param offset The offset that we will write from
 * @param len Data length
 * @return u32 
 *  Length of actual written data
 */
u32 ECGP_memchainWrite(struct ECGP_memchain *mc, u8 *buffer, u32 offset, u32 len)
{
    u32 rc;
    u32 sum;
    u32 copiedOffset;
    u32 copiedLen;
    u32 totalSize;
    struct ECGP_memchain *cur;
    struct ECGP_memchain *next;

    ECGP_ASSERT(mc);
    ECGP_ASSERT(buffer);
    
    if (!len) {
        return 0;
    }

    /* We support memory catenating. If the offset is too large, check if we
     * need to put the new data into the next memchain. Only the last chain's
     * dataSize can be increased.
     */
    copiedOffset = offset;
    cur = mc;
    while (copiedOffset >= cur->dataSize) {
        next = SLIST_NEXT(cur, nextMc);
        if (next) {
            copiedOffset -= cur->dataSize;
            cur = next;
        } else {
            break;
        }
    }
    
    sum = 0;
    while (len) {
        next = SLIST_NEXT(cur, nextMc);
        if (next) {
            copiedLen = ECGP_MIN(len, cur->dataSize - copiedOffset);
        } else {
            copiedLen = len;
        }
        rc = ECGP_memchainCopy(cur, &buffer[sum], copiedOffset, 
                               copiedLen, ECGP_TRUE);
        sum += rc;
        if (next) {
            ECGP_ASSERT(rc == copiedLen);
        } else {
            /* Only the last chain's dataSize can be increased. If rc is 
             * non-zero, it means the writing takes effect. Go to check if the
             * dataSize needs to be updated.
             */
            if (rc && ((rc += copiedOffset) > cur->dataSize)) {
                cur->dataSize = rc;
            }
            break;
        }
        cur = next;
        copiedOffset = 0;
        len -= rc;
    }

    totalSize = sum + offset;
    if (totalSize > mc->totalSize) {
        /* If is writing, we need to set total size of written data */
        mc->totalSize = totalSize;
    }

    return sum;
}

/**
 * @brief This is called to copy data from given memchain.
 * 
 * @param mc Pointer of allocated memchain
 * @param buffer Pointer of buffer
 * @param offset The offset that we will read from
 * @param len Length of buffer
 * @return u32 
 *  Length of actual read data
 */
u32 ECGP_memchainRead(struct ECGP_memchain *mc, u8 *buffer, u32 offset, u32 len)
{
    u32 rc;
    u32 sum;
    u32 copiedOffset;
    u32 copiedLen;
    struct ECGP_memchain *cur;
    struct ECGP_memchain *next;

    ECGP_ASSERT(mc);
    ECGP_ASSERT(buffer);

    if (!len || (offset >= mc->totalSize)) {
        return 0;
    }

    /* We support memory catenating. If the offset is too large, check if we
     * need to read from the next memchain.
     */
    copiedOffset = offset;
    cur = mc;
    while (copiedOffset >= cur->dataSize) {
        copiedOffset -= cur->dataSize;
        cur = SLIST_NEXT(cur, nextMc);
        ECGP_ASSERT(cur);
    }

    sum = 0;
    while (1) {
        copiedLen = ECGP_MIN(len - sum, cur->dataSize - copiedOffset);
        rc = ECGP_memchainCopy(cur, &buffer[sum], copiedOffset, 
                               copiedLen, ECGP_FALSE);
        sum += rc;
        if (sum >= len) {
            break;
        }
        next = SLIST_NEXT(cur, nextMc);
        if (!next) {
            break;
        }
        cur = next;
        copiedOffset = 0;
    }

    return sum;
}

/**
 * @brief This is called to free allocated memchain.
 * 
 * @param mc Pointer of memchain
 */
void ECGP_memchainFree(struct ECGP_memchain *mc)
{
    struct ECGP_memchainBlock *block, *after;
    struct ECGP_memchainBlock *pendingBlock;
    struct ECGP_memchainBlock *nextBlock;
    struct ECGP_memchainPool *mcp;
    int rc;

    ECGP_ASSERT(mc);

    mcp = mc->mcp;
    ECGP_ASSERT(mcp);

    pendingBlock = (struct ECGP_memchainBlock *)((void *)mc -
                   offsetof(struct ECGP_memchainBlock, data));
    
    while(pendingBlock) {
        /* According to the size of memblock, check where new 
         * blocks should be inserted.
         */
        after = NULL;
        SLIST_FOREACH(block, &mcp->memblocksHeader, entry) {
            if(pendingBlock->availableSize <= block->availableSize) {
                break;
            }
            after = block;
        }

        nextBlock = SLIST_NEXT(pendingBlock, entry);

        if(after) {
            SLIST_INSERT_AFTER(after, pendingBlock, entry);
        } else {
            SLIST_INSERT_HEAD(&mcp->memblocksHeader, pendingBlock, entry);
        }
        mcp->totalSize += pendingBlock->availableSize;

        pendingBlock = nextBlock;
    }
}

/**
 * @brief This is called to free multiple memchain at the same time.
 * We support memory catenating via ECGP_memchainCat(). Free all of memery that
 * has been used by this multiple memchain.
 * 
 * @param mc Pointer of multiple memchain
 */
void ECGP_memchainMultiFree(struct ECGP_memchain *mc)
{
    struct ECGP_memchain *next;

    do {
        next = SLIST_NEXT(mc, nextMc);
        ECGP_memchainFree(mc);
        mc = next;
    } while (mc);
}

/**
 * @brief This is called to insert data before valid data.
 * 
 * @param mc Pointer of memory chain
 * @param data Pointer of data
 * @param len Length to insert
 * @return ECGP_error
 */
ECGP_error ECGP_memchainInsertLeading(struct ECGP_memchain *mc, u8 *data, u32 len)
{
    u32 rc;

    ECGP_ASSERT(mc);
    ECGP_ASSERT(data);

    if(len > mc->leadingBytes) {
        return ECGP_EPARAM;
    }

    mc->leadingBytes -= len;

    rc = ECGP_memchainCopy(mc, data, 0, len, ECGP_TRUE);

    ECGP_ASSERT(rc == len);
    mc->dataSize += rc;
    mc->totalSize += rc;

    return ECGP_ENONE;
}

/**
 * @brief This is called to adjust leading. Move the data start point forward.
 * 
 * @param mc 
 * @param len 
 * @return ECGP_error 
 */
ECGP_error ECGP_memchainAdjustLeading(struct ECGP_memchain *mc, u32 len)
{
    if (len > mc->dataSize) {
        return ECGP_EPARAM;
    }
    
    mc->dataSize -= len;
    mc->totalSize -= len;
    mc->leadingBytes += len;

    return ECGP_ENONE;
}

/**
 * @brief This is called to get the free size left that we can allocate.
 * 
 * @param mcp
 * @return u32 
 */
u32 ECGP_memchainGetFreeSize(struct ECGP_memchainPool *mcp)
{
    ECGP_ASSERT(mcp);

    if (mcp->totalSize) {
        return mcp->totalSize - sizeof(struct ECGP_memchain);
    } else {
        return 0;
    }
}

/**
 * @brief Catenate two memchain.
 * 
 * @param dest 
 * @param src 
 * @return struct ECGP_memchain* 
 */
struct ECGP_memchain *ECGP_memchainCat(struct ECGP_memchain *dest,
                                       struct ECGP_memchain *src)
{
    u32 len;
    struct ECGP_memchain *last;
    struct ECGP_memchain *next;

    ECGP_ASSERT(dest);
    ECGP_ASSERT(src);

    next = dest;
    while (next) {
        last = next;
        next = SLIST_NEXT(next, nextMc);
    }

    SLIST_NEXT(last, nextMc) = src;
    dest->totalSize += src->totalSize;

    return dest;
}