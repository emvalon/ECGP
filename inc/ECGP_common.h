/**
 * @file ECGP_common.h
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
#ifndef _ECGP_COMMON_H_
#define _ECGP_COMMON_H_

#include "ECGP_config.h"
#include <stddef.h>
#include <stdint.h>

typedef int32_t     i32;
typedef int16_t     i16;
typedef int8_t      i8;
typedef uint32_t    u32;
typedef uint16_t    u16;
typedef uint8_t     u8;
typedef int32_t     ECGP_error;


#define ECGP_ENONE         (0)  /*!< No error                         */
#define ECGP_EMEMOUT       (-1) /*!< Memory access is out of bounds   */
#define ECGP_EEND          (-2) /*!< Wrong end descriptor             */
#define ECGP_EFULL         (-3) /*!< Memory is full                   */ 
#define ECGP_EPARAM        (-4) /*!< Wrong parameters                 */
#define ECGP_ESEQ          (-5) /*!< Sequence error                   */
#define ECGP_ESENDACK      (-6) /*!< Send ack error                   */
#define ECGP_ELENGTH       (-7) /*!< Larger then maximum length       */
#define ECGP_ECOMM         (-8) /*!< Communication failure            */

typedef int (*link_callback_typedef)(u32 len);

typedef enum {
    ECGP_FALSE = 0,
    ECGP_TRUE
}ECGP_Bool;

#define ECGP_MIN(a,b)           ((a) > (b) ? (b) : (a))

#define ECGP_POINT_U8(d)            ((u8*)d)

#define ECGP_GET_U16(a)         ( ( *ECGP_POINT_U8(a)) | (*(ECGP_POINT_U8(a)+1)<<8) )
#define ECGP_SET_U16(add,data)  do{                                                \
                                    *ECGP_POINT_U8(add)       = data&0xffu;        \
                                    *(ECGP_POINT_U8(add)+1)   = (data>>8)&0xffu;   \
                                }while(0)
#endif
