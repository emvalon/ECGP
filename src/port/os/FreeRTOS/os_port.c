/**
 * @file os_port.c
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
#include "ECGP_porting.h"

/**
 * @brief This is called to create semaphore.
 * This semaphore is used to indicate data has been sent or recived.
 * When new packet has been enqueued, this will be also used to inform
 * task/thread to process. It's better to use a binary semaphore here.
 * 
 * @return void* Pointer of semaphore
 */
void *ECGP_createSemaphore(void)
{
    //Implement me!!!
    return NULL;
}

/**
 * @brief This is called to get semaphore.
 * If based on some OS, it may get blocked here.
 * The waiting time is inputted, because we need to process timeout.
 * 
 * @param semaphore Pointer of semaphore
 * @param waitTime  Time to wait
 */
void ECGP_takeSemaphore(void *semaphore, u32 waitTime)
{
    //Implement me!!!
}

/**
 * @brief This is called to put semaphore.
 * 
 * @param semaphore Pointer of semaphore
 */
void ECGP_giveSemaphore(void *semaphore)
{
    //Implement me!!!
}

/**
 * @brief This is called to get current time.
 * This time will be used through whole stack.
 * 
 * @return u32 Current time
 */
u32 ECGP_getTime(void)
{
    //Implement me!!!
    return 0;
}

/**
 * @brief This is called to send raw data.
 * This function needs to configure physical device and start to transmit.
 * 
 * @param data Tx buffer
 * @param len  Length of data
 * @return ECGP_error 
 */
ECGP_error ECGP_physicalSend(u8* data, u16 len)
{
    //Implement me!!!
    return ECGP_ENONE;
}

/**
 * @brief This is called to receive raw data.
 * This function needs to configure physical device and start to receive.
 * All of received data need to be saved in rx buffer. Before buffer is full,
 * need to inform task/thread.
 * 
 * @param data Rx buffer
 * @param len  Length of buffer
 * @return ECGP_error 
 */
ECGP_error ECGP_physicalRecv(u8* data, u16 len)
{
    //Implement me!!!
    return ECGP_ENONE;
}