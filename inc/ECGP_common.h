/**
 * @file ECGP_common.h
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-07-05
 * 
 * Copyright © 2019-2021 Weilong Shen (valonshen@foxmail.com).
 * All rights reserved.
 * 
 */

#ifndef _ECGP_COMMON_H_
#define _ECGP_COMMON_H_


typedef  unsigned long  u32;
typedef  unsigned short u16;
typedef  unsigned char  u8;
typedef  int     ECGP_error;


enum{
  ECGP_ENONE  = 0,
  ECGP_EMEMOUT,         //memory access is out of bounds    1
  ECGP_EEND,            //wrong end descriptor              2
  ECGP_EFULL,           //memory is full                    3 
  ECGP_EPARA,           //worng parameters                  4
  ECGP_ESEQ,            //sequence error                    5
  ECGP_ESENDACK,        //send ack error                    6
  ECGP_ELENGTH,         //larger then maximum length        7
  ECGP_ECOMM,			//communication failure             8
}ECGP_ERROE;


typedef enum {
    ECGP_FALSE = 0,
    ECGP_TRUE
}ECGP_Bool;

typedef void(*link_callback_typedef)(u16 num);

#define ECGP_POINT_U8(d)            ((u8*)d)

#define ECGP_GET_U16(a)         ( ( *ECGP_POINT_U8(a)) | (*(ECGP_POINT_U8(a)+1)<<8) )
#define ECGP_SET_U16(add,data)  do{                                                \
                                    *ECGP_POINT_U8(add)       = data&0xffu;        \
                                    *(ECGP_POINT_U8(add)+1)   = (data>>8)&0xffu;   \
                                }while(0)
#endif
