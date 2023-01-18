/**
 * @file test_memchain.c
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
#include <testutil/testutil.h>
#include <misc/ECGP_memchain.h>
#include <ext/queue.h>
#include <string.h>

static ECGP_error error;
static u8 *buffer1 = NULL;
static u8 *buffer2 = NULL;
static u8 *buffer3 = NULL;
static u8 readBuf[200];
static u32 freeSize;
static struct ECGP_memchainPool mcp;
static u8 rawData[] = {
    0x55, 0xc0, 0x34, 0xff, 0x00, 0xe5, 0x5c, 0xc5, 0xee, 0xf2,
    0x1e, 0xd8, 0xe2, 0x5c, 0xab, 0xb1, 0xc4, 0x5e, 0x89, 0x90,
    0x55, 0xc0, 0x34, 0xff, 0x00, 0xe5, 0x5c, 0xc5, 0xee, 0xf2,
    0x1e, 0xd8, 0xe2, 0x5c, 0xab, 0xb1, 0xc4, 0x5e, 0x89, 0x90,
    0x55, 0xc0, 0x34, 0xff, 0x00, 0xe5, 0x5c, 0xc5, 0xee, 0xf2,
    0x1e, 0xd8, 0xe2, 0x5c, 0xab, 0xb1, 0xc4, 0x5e, 0x89, 0x90,
    0x55, 0xc0, 0x34, 0xff, 0x00, 0xe5, 0x5c, 0xc5, 0xee, 0xf2,
    0x1e, 0xd8, 0xe2, 0x5c, 0xab, 0xb1, 0xc4, 0x5e, 0x89, 0x90,
    0x55, 0xc0, 0x34, 0xff, 0x00, 0xe5, 0x5c, 0xc5, 0xee, 0xf2,
    0x1e, 0xd8, 0xe2, 0x5c, 0xab, 0xb1, 0xc4, 0x5e, 0x89, 0x90,
    0x55, 0xc0, 0x34, 0xff, 0x00, 0xe5, 0x5c, 0xc5, 0xee, 0xf2,
    0x1e, 0xd8, 0xe2, 0x5c, 0xab, 0xb1, 0xc4, 0x5e, 0x89, 0x90,
    0x55, 0xc0, 0x34, 0xff, 0x00, 0xe5, 0x5c, 0xc5, 0xee, 0xf2,
    0x1e, 0xd8, 0xe2, 0x5c, 0xab, 0xb1, 0xc4, 0x5e, 0x89, 0x90,
    0x55, 0xc0, 0x34, 0xff, 0x00, 0xe5, 0x5c, 0xc5, 0xee, 0xf2,
    0x1e, 0xd8, 0xe2, 0x5c, 0xab, 0xb1, 0xc4, 0x5e, 0x89, 0x90,
    0x55, 0xc0, 0x34, 0xff, 0x00, 0xe5, 0x5c, 0xc5, 0xee, 0xf2,
    0x1e, 0xd8, 0xe2, 0x5c, 0xab, 0xb1, 0xc4, 0x5e, 0x89, 0x90,
    0x55, 0xc0, 0x34, 0xff, 0x00, 0xe5, 0x5c, 0xc5, 0xee, 0xf2,
    0x1e, 0xd8, 0xe2, 0x5c, 0xab, 0xb1, 0xc4, 0x5e, 0x89, 0x90,
};

#define CHECK_FREE_SIZE(s)                                                  \
    do {                                                                    \
        freeSize = ECGP_memchainGetFreeSize(&mcp);                          \
        TEST_ASSERT_FATAL(freeSize == (s), "free size:%d", freeSize);       \
    } while (0)

TEST_CASE(memchain_testInit)
{
    ECGP_memchainPoolInit(&mcp);
}

TEST_CASE(memchain_testAdd)
{
    u8 index;
    struct ECGP_memchainBlock *block;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(0) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    freeSize = ECGP_memchainGetFreeSize(&mcp);
    TEST_ASSERT_FATAL(freeSize == 0, "free size:%d", freeSize);

    error = ECGP_memchainPoolAddBuffer(&mcp, 0, 10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_EPARAM, "error:%d", error);

    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(0) - 1, 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_EPARAM, "error:%d", error);

    /* We allow the actual buffer length set to 0. Only the 1st block needs 
     * a memchain header. The remaining blocks can be used to store data.
     */
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(0), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);    
    freeSize = ECGP_memchainGetFreeSize(&mcp);
    TEST_ASSERT_FATAL(freeSize == 9 * sizeof(struct ECGP_memchain), 
                "free size:%d", freeSize);

    /* Add another buffer. */
    buffer2 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(100) * 10);
    TEST_ASSERT_FATAL(buffer2);

    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(100), 
                                       10, buffer2);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);    

    freeSize = ECGP_memchainGetFreeSize(&mcp);
    TEST_ASSERT_FATAL(freeSize == ((19 * sizeof(struct ECGP_memchain)) + 1000), 
                "free size:%d", freeSize);

    index = 0;
    SLIST_FOREACH(block, &mcp.memblocksHeader, entry) {
        if (++index <= 10) {
            TEST_ASSERT_FATAL(block->availableSize == 
                              (ECGP_MEMCHAIN_BLOCK_SIZE(0) - 
                              sizeof(struct ECGP_memchainBlock)));
        } else {
            TEST_ASSERT_FATAL(block->availableSize == 
                              (ECGP_MEMCHAIN_BLOCK_SIZE(100) - 
                              sizeof(struct ECGP_memchainBlock)));
        }
    }
}

TEST_CASE(memchain_testAdd2)
{
    u8 index;
    struct ECGP_memchainBlock *block;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer1);
    buffer2 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(1) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(11), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error); 
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(1), 
                                       10, buffer2);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);   

    index = 0;
    SLIST_FOREACH(block, &mcp.memblocksHeader, entry) {
        if (++index <= 10) {
            TEST_ASSERT_FATAL(block->availableSize == (
                              ECGP_MEMCHAIN_BLOCK_SIZE(1) -
                              sizeof(struct ECGP_memchainBlock)));
        } else {
            TEST_ASSERT_FATAL(block->availableSize == 
                              (ECGP_MEMCHAIN_BLOCK_SIZE(11) - 
                              sizeof(struct ECGP_memchainBlock)));
        }
    }    
}

TEST_CASE(memchain_testMalloc_00)
{
    void *addr;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 1);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       1, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE(10);

    addr = ECGP_memchainAlloc(&mcp, 0, 0);
    TEST_ASSERT_FATAL(addr == NULL);
    CHECK_FREE_SIZE(10);

    addr = ECGP_memchainAlloc(&mcp, 11, 0);
    TEST_ASSERT_FATAL(addr == NULL);
    CHECK_FREE_SIZE(10);

    addr = ECGP_memchainAlloc(&mcp, 1, 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE(0);

    addr = ECGP_memchainAlloc(&mcp, 1, 0);
    TEST_ASSERT_FATAL(addr == NULL);
    CHECK_FREE_SIZE(0);
}

TEST_CASE(memchain_testMalloc_01)
{
    void *addr;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(11) * 2);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(11), 
                                       2, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);

    freeSize = ECGP_memchainGetFreeSize(&mcp);
    CHECK_FREE_SIZE((1 * sizeof(struct ECGP_memchain)) + (11 * 2));

    addr = ECGP_memchainAlloc(&mcp, 501, 0);
    TEST_ASSERT_FATAL(addr == NULL);
    CHECK_FREE_SIZE((1 * sizeof(struct ECGP_memchain)) + (11 * 2));

    addr = ECGP_memchainAlloc(&mcp, 11, 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE(11);

    addr = ECGP_memchainAlloc(&mcp, 12, 0);
    TEST_ASSERT_FATAL(addr == NULL);
    CHECK_FREE_SIZE(11);

    addr = ECGP_memchainAlloc(&mcp, 0, 0);
    TEST_ASSERT_FATAL(addr == NULL);
    CHECK_FREE_SIZE(11);

    addr = ECGP_memchainAlloc(&mcp, 2, 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE(0);
}

TEST_CASE(memchain_testMalloc_10)
{
    void *addr;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(321) * 21);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(321), 
                                       21, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE((20 * sizeof(struct ECGP_memchain)) + (321 * 21));

    addr = ECGP_memchainAlloc(&mcp,
                               (20 * sizeof(struct ECGP_memchain)) + 
                               (321 * 21) + 1, 
                               0);
    TEST_ASSERT_FATAL(addr == NULL);
    CHECK_FREE_SIZE((20 * sizeof(struct ECGP_memchain)) + (321 * 21));

    addr = ECGP_memchainAlloc(&mcp, 
                               (20 * sizeof(struct ECGP_memchain)) + (321 * 21), 
                               0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE(0);
}

TEST_CASE(memchain_testMalloc_20)
{
    void *addr;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(1) * 10);
    TEST_ASSERT_FATAL(buffer1);
    buffer2 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(1), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error); 
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       10, buffer2);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE((19 * sizeof(struct ECGP_memchain)) + 10 + 100);

    addr = ECGP_memchainAlloc(&mcp, (19 * sizeof(struct ECGP_memchain)) + 
                               10 + 100, 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE(0);
}

TEST_CASE(memchain_testMalloc_21)
{
    void *addr;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(1) * 10);
    TEST_ASSERT_FATAL(buffer1);
    buffer2 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(1), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error); 
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       10, buffer2);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE((19 * sizeof(struct ECGP_memchain)) + 10 + 100);

    addr = ECGP_memchainAlloc(&mcp, 1, 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE((18 * sizeof(struct ECGP_memchain)) + 9 + 100);

    addr = ECGP_memchainAlloc(&mcp, 2, 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE((17 * sizeof(struct ECGP_memchain)) + 9 + 90);

    addr = ECGP_memchainAlloc(&mcp, 10, 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE((16 * sizeof(struct ECGP_memchain)) + 9 + 80);

    addr = ECGP_memchainAlloc(&mcp, 11, 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE((14 * sizeof(struct ECGP_memchain)) + 8 + 70);

    addr = ECGP_memchainAlloc(&mcp, 10 + ECGP_MEMCHAIN_BLOCK_SIZE(1) - 
                               sizeof(struct ECGP_memchainBlock), 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE((12 * sizeof(struct ECGP_memchain)) + 7 + 60);

    addr = ECGP_memchainAlloc(&mcp, 11 + ECGP_MEMCHAIN_BLOCK_SIZE(1) - 
                               sizeof(struct ECGP_memchainBlock), 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE((10 * sizeof(struct ECGP_memchain)) + 7 + 40);

    addr = ECGP_memchainAlloc(&mcp, 12 + ECGP_MEMCHAIN_BLOCK_SIZE(10) - 
                               sizeof(struct ECGP_memchainBlock), 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE((7 * sizeof(struct ECGP_memchain)) + 6 + 20);

    addr = ECGP_memchainAlloc(&mcp, (7 * sizeof(struct ECGP_memchain)) + 
                               6 + 20, 0);
    TEST_ASSERT_FATAL(addr);
    CHECK_FREE_SIZE(0);
}

TEST_CASE(memchain_testMalloc_30)
{
    struct ECGP_memchain *mc;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(1) * 10);
    TEST_ASSERT_FATAL(buffer1);
    buffer2 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(1), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error); 
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       10, buffer2);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE((19 * sizeof(struct ECGP_memchain)) + 10 + 100);

    mc = ECGP_memchainAlloc(&mcp, 0, 1);
    TEST_ASSERT_FATAL(mc);
    CHECK_FREE_SIZE((18 * sizeof(struct ECGP_memchain)) + 9 + 100);
    TEST_ASSERT_FATAL(mc->leadingBytes == 1);

    mc = ECGP_memchainAlloc(&mcp, 0, 
                             (18 * sizeof(struct ECGP_memchain)) + 9 + 100);
    TEST_ASSERT_FATAL(mc);
    CHECK_FREE_SIZE(0);
    TEST_ASSERT_FATAL(mc->leadingBytes == 
                      (18 * sizeof(struct ECGP_memchain)) + 9 + 100);
}

TEST_CASE(memchain_testFree_00)
{
    struct ECGP_memchain *mc;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(50) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(50), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE((9 * sizeof(struct ECGP_memchain)) + 500);

    mc = ECGP_memchainAlloc(&mcp, 1, 0);
    TEST_ASSERT_FATAL(mc);
    CHECK_FREE_SIZE((8 * sizeof(struct ECGP_memchain)) + 450);

    ECGP_memchainFree(mc);
    CHECK_FREE_SIZE((9 * sizeof(struct ECGP_memchain)) + 500);

    mc = ECGP_memchainAlloc(&mcp, (9 * sizeof(struct ECGP_memchain)) + 500, 0);
    TEST_ASSERT_FATAL(mc);
    CHECK_FREE_SIZE(0);

    ECGP_memchainFree(mc);
    CHECK_FREE_SIZE((9 * sizeof(struct ECGP_memchain)) + 500);
}

TEST_CASE(memchain_testFree_10)
{
    struct ECGP_memchain *mc;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(1) * 10);
    TEST_ASSERT_FATAL(buffer1);
    buffer2 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(1), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error); 
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       10, buffer2);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE((19 * sizeof(struct ECGP_memchain)) + 10 + 100);

    mc = ECGP_memchainAlloc(&mcp, 1, 0);
    TEST_ASSERT_FATAL(mc);
    CHECK_FREE_SIZE((18 * sizeof(struct ECGP_memchain)) + 9 + 100);
    ECGP_memchainFree(mc);
    CHECK_FREE_SIZE((19 * sizeof(struct ECGP_memchain)) + 10 + 100);

    mc = ECGP_memchainAlloc(&mcp, 3, 0);
    TEST_ASSERT_FATAL(mc);
    CHECK_FREE_SIZE((18 * sizeof(struct ECGP_memchain)) + 10 + 90);
    ECGP_memchainFree(mc);
    CHECK_FREE_SIZE((19 * sizeof(struct ECGP_memchain)) + 10 + 100);

    mc = ECGP_memchainAlloc(&mcp, 11, 0);
    TEST_ASSERT_FATAL(mc);
    CHECK_FREE_SIZE((17 * sizeof(struct ECGP_memchain)) + 9 + 90);
    ECGP_memchainFree(mc);
    CHECK_FREE_SIZE((19 * sizeof(struct ECGP_memchain)) + 10 + 100);

    mc = ECGP_memchainAlloc(&mcp, 0, 
                             (19 * sizeof(struct ECGP_memchain)) + 10 + 100);
    TEST_ASSERT_FATAL(mc);
    CHECK_FREE_SIZE(0);
    ECGP_memchainFree(mc);
    CHECK_FREE_SIZE((19 * sizeof(struct ECGP_memchain)) + 10 + 100);
}

TEST_CASE(memchain_testWrite_00)
{
    u32 rc;
    u8 *data;
    struct ECGP_memchain *mc;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE((9 * sizeof(struct ECGP_memchain)) + 100);

    mc = ECGP_memchainAlloc(&mcp, 10, 0);
    TEST_ASSERT_FATAL(mc);
    data = (u8 *)((void *)mc + sizeof(struct ECGP_memchain));
    memset(data, 0, 10);

    rc = ECGP_memchainWrite(mc, rawData, 0, 0);
    TEST_ASSERT_FATAL(rc == 0, "rc = %d", rc);

    rc = ECGP_memchainWrite(mc, rawData, 10, 1);
    TEST_ASSERT_FATAL(rc == 0, "rc = %d", rc);

    rc = ECGP_memchainWrite(mc, rawData, 0, 11);
    TEST_ASSERT_FATAL(rc == 10, "rc = %d", rc);
    TEST_ASSERT_FATAL(mc->dataSize == 10);
    TEST_ASSERT_FATAL(memcmp(data, rawData, 10) == 0);

    memset(data, 0, 10);
    rc = ECGP_memchainWrite(mc, rawData, 1, 5);
    TEST_ASSERT_FATAL(mc->dataSize == 10);
    TEST_ASSERT_FATAL(rc == 5, "rc = %d", rc);
    TEST_ASSERT_FATAL(data[0] == 0);
    TEST_ASSERT_FATAL(data[6] == 0);
    TEST_ASSERT_FATAL(memcmp(&data[1], rawData, 5) == 0);

    memset(data, 0, 10);
    rc = ECGP_memchainWrite(mc, rawData, 9, 10);
    TEST_ASSERT_FATAL(mc->dataSize == 10);
    TEST_ASSERT_FATAL(rc == 1, "rc = %d", rc);
    TEST_ASSERT_FATAL(data[8] == 0);
    TEST_ASSERT_FATAL(data[9] == rawData[0]);
}

TEST_CASE(memchain_testWrite_10)
{
    u32 rc;
    u8 *data;
    struct ECGP_memchain *mc;
    struct ECGP_memchainBlock *bk;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(2) * 10);
    TEST_ASSERT_FATAL(buffer1);

    buffer2 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer2);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(2), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       10, buffer2);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE((19 * sizeof(struct ECGP_memchain)) + 100 + 20);

    mc = ECGP_memchainAlloc(&mcp, 11, 0);
    CHECK_FREE_SIZE((17 * sizeof(struct ECGP_memchain)) + 90 + 18);
    data = (u8 *)((void *)mc + sizeof(struct ECGP_memchain));
    rc = ECGP_memchainWrite(mc, rawData, 0, 12);
    TEST_ASSERT_FATAL(rc == 12, "rc == %d", rc);
    bk = (void *)mc - offsetof(struct ECGP_memchainBlock, data);

    TEST_ASSERT_FATAL(memcmp(data, rawData, 10) == 0);
    bk = SLIST_NEXT(bk, entry);
    TEST_ASSERT_FATAL(memcmp(bk->data, &rawData[10], 2) == 0);
}

TEST_CASE(memchain_testRead_00)
{
    u32 rc;
    u8 *data;
    struct ECGP_memchain *mc;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE((9 * sizeof(struct ECGP_memchain)) + 100);

    mc = ECGP_memchainAlloc(&mcp, 10, 0);
    TEST_ASSERT_FATAL(mc);
    data = (u8 *)((void *)mc + sizeof(struct ECGP_memchain));

    rc = ECGP_memchainRead(mc, readBuf, 1, 10);
    TEST_ASSERT_FATAL(rc == 0, "rc = %d", rc);

    rc = ECGP_memchainWrite(mc, rawData, 0, 9);
    TEST_ASSERT_FATAL(mc->dataSize == 9);
    TEST_ASSERT_FATAL(rc == 9, "rc = %d", rc);

    rc = ECGP_memchainRead(mc, readBuf, 9, 5);
    TEST_ASSERT_FATAL(rc == 0, "rc = %d", rc);

    memset(readBuf, 0, sizeof(readBuf));
    rc = ECGP_memchainRead(mc, readBuf, 0, 20);
    TEST_ASSERT_FATAL(rc == 9, "rc = %d", rc);
    TEST_ASSERT_FATAL(memcmp(readBuf, rawData, 9) == 0);

    memset(readBuf, 0, sizeof(readBuf));
    rc = ECGP_memchainRead(mc, readBuf, 2, 20);
    TEST_ASSERT_FATAL(rc == 7, "rc = %d", rc);
    TEST_ASSERT_FATAL(memcmp(readBuf, &rawData[2], 7) == 0);

    memset(readBuf, 0, sizeof(readBuf));
    rc = ECGP_memchainRead(mc, readBuf, 8, 5);
    TEST_ASSERT_FATAL(rc == 1, "rc = %d", rc);
    TEST_ASSERT_FATAL(memcmp(readBuf, &rawData[8], 1) == 0);
}


TEST_CASE(memchain_testRead_10)
{
    u32 rc;
    struct ECGP_memchain *mc;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(2) * 10);
    TEST_ASSERT_FATAL(buffer1);

    buffer2 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer2);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(2), 
                                       10, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       10, buffer2);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    CHECK_FREE_SIZE((19 * sizeof(struct ECGP_memchain)) + 100 + 20);

    mc = ECGP_memchainAlloc(&mcp, 33, 0);
    rc = ECGP_memchainWrite(mc, rawData, 0, 33);
    TEST_ASSERT_FATAL(rc == 33, "rc == %d", rc);
    
    memset(readBuf, 0, sizeof(readBuf));
    rc = ECGP_memchainRead(mc, readBuf, 0, 40);
    TEST_ASSERT_FATAL(rc == 33, "rc == %d", rc);

    TEST_ASSERT_FATAL(memcmp(readBuf, rawData, 33) == 0);
    TEST_ASSERT_FATAL(readBuf[33] == 0);
}

TEST_CASE(memchain_testRead_11)
{
    u32 rc;
    u32 total;
    struct ECGP_memchain *mc;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(1) * 1);
    TEST_ASSERT_FATAL(buffer1);

    buffer2 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(2) * 2);
    TEST_ASSERT_FATAL(buffer2);

    buffer3 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(3) * 1);
    TEST_ASSERT_FATAL(buffer3);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(1), 
                                       1, buffer1);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(2), 
                                       2, buffer2);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(3), 
                                       1, buffer3);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:%d", error);
    total = (3 * sizeof(struct ECGP_memchain)) + 1 + 4 + 3;
    TEST_ASSERT_FATAL(total <= sizeof(rawData));
    CHECK_FREE_SIZE(total);

    mc = ECGP_memchainAlloc(&mcp, total, 0);
    CHECK_FREE_SIZE(0);

    rc = ECGP_memchainWrite(mc, rawData, 0, total);
    TEST_ASSERT_FATAL(rc == total, "rc == %d", rc);
    
    memset(readBuf, 0, sizeof(readBuf));
    rc = ECGP_memchainRead(mc, readBuf, 0, total);
    TEST_ASSERT_FATAL(rc == total, "rc == %d", rc);

    TEST_ASSERT_FATAL(memcmp(readBuf, rawData, total) == 0);
}

TEST_CASE(memchain_testInsertLeading_00)
{
    ECGP_error err;
    u32 rc;
    u8 *data;
    struct ECGP_memchain *mc;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(22) * 11);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(22), 
                                       11, buffer1);
    CHECK_FREE_SIZE((10 * sizeof(struct ECGP_memchain)) + (22 * 11));

    mc = ECGP_memchainAlloc(&mcp, 6, 5);
    CHECK_FREE_SIZE((9 * sizeof(struct ECGP_memchain)) + (22 * 10));
    TEST_ASSERT_FATAL(mc->leadingBytes == 5);
    TEST_ASSERT_FATAL(mc->dataSize == 0);
    data = (u8 *)((void *)mc + sizeof(struct ECGP_memchain));
    err = ECGP_memchainInsertLeading(mc, rawData, 5);
    TEST_ASSERT_FATAL(err == ECGP_ENONE);
    TEST_ASSERT_FATAL(mc->leadingBytes == 0);
    TEST_ASSERT_FATAL(mc->dataSize == 5);
    TEST_ASSERT_FATAL(memcmp(data, rawData, 5) == 0);

    /* Test leading bytes occupying 2 blocks */
    mc = ECGP_memchainAlloc(&mcp, 0, 30);
    CHECK_FREE_SIZE((7 * sizeof(struct ECGP_memchain)) + (22 * 8));
    TEST_ASSERT_FATAL(mc->leadingBytes == 30);
    TEST_ASSERT_FATAL(mc->dataSize == 0);
    err = ECGP_memchainInsertLeading(mc, rawData, 29);
    TEST_ASSERT_FATAL(err == ECGP_ENONE);
    TEST_ASSERT_FATAL(mc->leadingBytes == 1);
    TEST_ASSERT_FATAL(mc->dataSize == 29);
    memset(readBuf, 0, sizeof(readBuf));
    rc = ECGP_memchainRead(mc, readBuf, 0, sizeof(readBuf));
    TEST_ASSERT_FATAL(rc == 29);
    TEST_ASSERT_FATAL(memcmp(readBuf, rawData, 29) == 0);
    TEST_ASSERT_FATAL(readBuf[29] == 0);

    memset(readBuf, 0, sizeof(readBuf));
    rc = ECGP_memchainRead(mc, readBuf, 1, sizeof(readBuf));
    TEST_ASSERT_FATAL(rc == 28);
    TEST_ASSERT_FATAL(memcmp(readBuf, &rawData[1], 28) == 0);
    TEST_ASSERT_FATAL(readBuf[28] == 0);
}

TEST_CASE(memchain_testInsertLeading_01)
{
    ECGP_error err;
    struct ECGP_memchain *mc;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       10, buffer1);
    CHECK_FREE_SIZE((9 * sizeof(struct ECGP_memchain)) + 100);

    mc = ECGP_memchainAlloc(&mcp, 0, 8);
    TEST_ASSERT_FATAL(mc);
    CHECK_FREE_SIZE((8 * sizeof(struct ECGP_memchain)) + 90);

    err = ECGP_memchainInsertLeading(mc, rawData, 9);
    TEST_ASSERT_FATAL(err == ECGP_EPARAM);
}

TEST_CASE(memchain_testAdjustLeading)
{
    ECGP_error err;
    struct ECGP_memchain *mc;

    buffer1 = os_malloc(ECGP_MEMCHAIN_BLOCK_SIZE(10) * 10);
    TEST_ASSERT_FATAL(buffer1);

    ECGP_memchainPoolInit(&mcp);
    error = ECGP_memchainPoolAddBuffer(&mcp, ECGP_MEMCHAIN_BLOCK_SIZE(10), 
                                       10, buffer1);
    CHECK_FREE_SIZE((9 * sizeof(struct ECGP_memchain)) + 100);

    mc = ECGP_memchainAlloc(&mcp, 10, 0);
    TEST_ASSERT_FATAL(mc);
    TEST_ASSERT_FATAL(mc->leadingBytes == 0);
    err = ECGP_memchainAdjustLeading(mc ,1);
    TEST_ASSERT_FATAL(err == ECGP_EPARAM);

    ECGP_memchainWrite(mc, rawData, 0, 6);
    err = ECGP_memchainAdjustLeading(mc ,6);
    TEST_ASSERT_FATAL(err == ECGP_ENONE);
    TEST_ASSERT_FATAL(mc->leadingBytes == 6);

    err = ECGP_memchainAdjustLeading(mc ,1);
    TEST_ASSERT_FATAL(err == ECGP_EPARAM);

    /* large buffer */
    mc = ECGP_memchainAlloc(&mcp, 60, 1);
    TEST_ASSERT_FATAL(mc);
    ECGP_memchainWrite(mc, rawData, 0, 58);
    err = ECGP_memchainAdjustLeading(mc ,59);
    TEST_ASSERT_FATAL(err == ECGP_EPARAM);
    TEST_ASSERT_FATAL(mc->leadingBytes == 1);
    err = ECGP_memchainAdjustLeading(mc ,58);
    TEST_ASSERT_FATAL(err == ECGP_ENONE);
    TEST_ASSERT_FATAL(mc->leadingBytes == 59);
}

TEST_SUITE(test_memchain)
{
    memchain_testInit();
    memchain_testAdd();
    memchain_testAdd2();
    memchain_testMalloc_00();
    memchain_testMalloc_01();
    memchain_testMalloc_10();
    memchain_testMalloc_20();
    memchain_testMalloc_21();
    memchain_testMalloc_30();
    memchain_testFree_00();
    memchain_testFree_10();
    memchain_testWrite_00();
    memchain_testWrite_10();
    memchain_testRead_00();
    memchain_testRead_10();
    memchain_testRead_11();
    memchain_testInsertLeading_00();
    memchain_testInsertLeading_01();
    memchain_testAdjustLeading();
}

static void 
test_memchainPreTestCb(void *arg)
{
    ECGP_ASSERT(!buffer1);
    ECGP_ASSERT(!buffer2);
    ECGP_ASSERT(!buffer3);
}

static void 
test_memchainPostTestCb(void *arg)
{
    if (buffer1) {
        os_free(buffer1);
        buffer1 = NULL;
    }
    if (buffer2) {
        os_free(buffer2);
        buffer2 = NULL;
    }
    if (buffer3) {
        os_free(buffer3);
        buffer3 = NULL;
    }   
}

void
test_mempchainRegister(void)
{
    ECGP_ASSERT(sizeof(readBuf) >= sizeof(rawData));
    TEST_SUITE_REGISTER(test_memchain, test_memchainPreTestCb, NULL,
                        test_memchainPostTestCb, NULL);
}