/**
 * @file test_mempool.c
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
#include <testutil/testutil.h>
#include <misc/ECGP_mempool.h>
#include <string.h>

static u8 *t_buffer = NULL;
static struct ECGP_mempool mp;
static ECGP_error error;

TEST_CASE(mempool_checkParam)
{
    t_buffer = os_malloc(10);

    error = ECGP_mempoolInit(&mp, 0, 10, t_buffer);
    TEST_ASSERT(error == ECGP_EPARAM, "error:0x%x\n", error);

    error = ECGP_mempoolInit(&mp, 10, 0, t_buffer);
    TEST_ASSERT(error == ECGP_EPARAM, "error:0x%x\n", error);

    error = ECGP_mempoolInit(&mp, 10, 1, t_buffer);
    TEST_ASSERT(error == ECGP_EPARAM, "error:0x%x\n", error);
}

TEST_CASE(mempool_testInit)
{
    ECGP_error error;

    t_buffer = os_malloc(50);
    TEST_ASSERT_FATAL(t_buffer);

    error = ECGP_mempoolInit(&mp, 5, 10, t_buffer);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:0x%x\n", error);
}

TEST_CASE(mempool_testMalloc)
{
    void * addr;
    u32 blockNum;

    t_buffer = os_malloc(50);
    TEST_ASSERT_FATAL(t_buffer);

    error = ECGP_mempoolInit(&mp, 5, 10, t_buffer);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:0x%x\n", error);
    blockNum = ECGP_mempoolGetBlockNum(&mp);
    TEST_ASSERT_FATAL(blockNum == 5, "num:%d\n", blockNum);

    for (u8 i = 0; i < 5; i++) {
        addr = ECGP_mempoolMalloc(&mp);
        TEST_ASSERT_FATAL(addr);
        blockNum = ECGP_mempoolGetBlockNum(&mp);
        TEST_ASSERT_FATAL(blockNum == (4 - i), "num:%d\n", blockNum);
    }
    addr = ECGP_mempoolMalloc(&mp);
    TEST_ASSERT_FATAL(!addr);
}

TEST_CASE(mempool_testFree)
{
    void * addr[6];
    u32 blockNum;
    
    t_buffer = os_malloc(50);
    TEST_ASSERT_FATAL(t_buffer);

    error = ECGP_mempoolInit(&mp, 5, 10, t_buffer);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:0x%x\n", error);

    // Get one block, Then free
    addr[0] = ECGP_mempoolMalloc(&mp);
    ECGP_mempoolFree(&mp, addr[0]);
    addr[0] = NULL;
    blockNum = ECGP_mempoolGetBlockNum(&mp);
    TEST_ASSERT_FATAL(blockNum == 5, "num:%d\n", blockNum);

    // Get two block, Then free
    addr[0] = ECGP_mempoolMalloc(&mp);
    addr[1] = ECGP_mempoolMalloc(&mp);
    ECGP_mempoolFree(&mp, addr[0]);
    addr[0] = NULL;
    blockNum = ECGP_mempoolGetBlockNum(&mp);
    TEST_ASSERT_FATAL(blockNum == 4, "num:%d\n", blockNum);

    addr[0] = ECGP_mempoolMalloc(&mp);
    blockNum = ECGP_mempoolGetBlockNum(&mp);
    TEST_ASSERT_FATAL(blockNum == 3, "num:%d\n", blockNum);

    ECGP_mempoolFree(&mp, addr[0]);
    addr[0] = NULL;
    ECGP_mempoolFree(&mp, addr[1]);
    addr[1] = NULL;
    blockNum = ECGP_mempoolGetBlockNum(&mp);
    TEST_ASSERT_FATAL(blockNum == 5, "num:%d\n", blockNum);
    
    // Get all of blocks. Then free
    for (uint8_t i = 0; i < 5; i++) {
        addr[i] = ECGP_mempoolMalloc(&mp);
    }
    blockNum = ECGP_mempoolGetBlockNum(&mp);
    TEST_ASSERT_FATAL(blockNum == 0, "num:%d\n", blockNum);

    for (uint8_t i = 0; i < 5; i++) {
        blockNum = ECGP_mempoolGetBlockNum(&mp);
        TEST_ASSERT_FATAL(blockNum == i, "i:%d, num:%d\n", i, blockNum);
       
        ECGP_mempoolFree(&mp, addr[i]);
        addr[i] = NULL;
    }
    blockNum = ECGP_mempoolGetBlockNum(&mp);
    TEST_ASSERT_FATAL(blockNum == 5, "num:%d\n", blockNum);
       
    addr[0] = ECGP_mempoolMalloc(&mp);
    TEST_ASSERT_FATAL(addr[0]);
}

TEST_CASE(mempool_testFrom)
{
    void *addr;
    ECGP_Bool from;

    t_buffer = os_malloc(50);
    TEST_ASSERT_FATAL(t_buffer);

    error = ECGP_mempoolInit(&mp, 5, 10, t_buffer);
    TEST_ASSERT_FATAL(error == ECGP_ENONE, "error:0x%x\n", error);

    addr = ECGP_mempoolMalloc(&mp);
    from = ECGP_mempoolIsFrom(&mp, addr);
    TEST_ASSERT(from);

    from = ECGP_mempoolIsFrom(&mp, (void *)0xff);
    TEST_ASSERT(!from);
}

TEST_SUITE(test_mempool)
{
    mempool_checkParam();
    mempool_testInit();
    mempool_testMalloc();
    mempool_testFree();
    mempool_testFrom();
}

static void 
test_mempoolPreTestCb(void *arg)
{
    ECGP_ASSERT(!t_buffer);
    memset(&mp, 0, sizeof(mp));
}

static void 
test_mempoolPostTestCb(void *arg)
{
    if (t_buffer) {
        os_free(t_buffer);
        t_buffer = NULL;
    }   
}

void
test_mempoolRegister(void)
{
    TEST_SUITE_REGISTER(test_mempool, test_mempoolPreTestCb, NULL,
                        test_mempoolPostTestCb, NULL);
}