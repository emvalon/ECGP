/**
 * @file main.c
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
#include <unistd.h>
#include <string.h>
#include "ECGP.h"
#include "ECGP_memchain.h"

#define MEMCHAIN_BLOCK_SIZE     (20)

struct ECGP_memchainPool mcp;
ECGP_MEMCHAIN_BUF_DEF(mc_buf, MEMCHAIN_BLOCK_SIZE, 10);

int main(void)
{
    u8 rc;
    u8 cnt = 0;
    u8 data[20];
    u8 readData[20];
    u8 leading[10];

    for(rc = 0; rc < sizeof(data); rc++) {
        data[rc] = rc;
    }

    for(rc = 0; rc < sizeof(leading); rc++) {
        leading[rc] = 100 + rc;
    }

    printf("-----------------------------------------------\n");
    printf("            ECGP Linux example\n");
    printf("-----------------------------------------------\n");

    // ECGP_init();

    ECGP_memchainPoolInit(&mcp);

    rc = ECGP_memchainPoolAddBuffer(&mcp, MEMCHAIN_BLOCK_SIZE, 10, mc_buf);
    if(rc) {
        printf("err:%d,%ld\n",rc,sizeof(struct ECGP_memchain));
    }

    struct ECGP_memchain *mc;

    while (ECGP_TRUE)
    {
        // ECGP_timeElapsed(10);

         mc = ECGP_memchainAlloc(&mcp, 10, 10);
        if(!mc) {
            printf("malloc fail\n");
        }
        else
        {
            printf("malloc success:%d\n",cnt++);
            printf("total size:%d, lending:%d\n",mc->totalSize,mc->leadingBytes);
        }
        

        rc = ECGP_memchainWrite(mc, data, 11);
        if(rc != 11) {
            printf("write len err:%d\n",rc);
        }
        else
        {
            printf("write ok, total:%d\n",mc->totalSize);
        }
        

        ECGP_memchainInsertLeading(mc, leading, 5);

        memset(readData, 0, sizeof(readData));
        rc = ECGP_memchainRead(mc, readData, 7);
        printf("***read: total %d, leading %d, read data ",mc->totalSize,mc->leadingBytes);
        for(uint8_t i = 0; i < rc; i++) {
            printf("%d:%d, ",i,readData[i]);
        }
        printf("\n");

        memset(readData, 0, sizeof(readData));
        rc = ECGP_memchainRead(mc, readData, 20);
        printf("***read:total %d, leading %d, read data ",mc->totalSize,mc->leadingBytes);
        for(uint8_t i = 0; i < rc; i++) {
            printf("%d:%d, ",i,readData[i]);
        }
        printf("\n");

        ECGP_memchainFree(mc);
        sleep(1);
    }
    
    return 0;
}