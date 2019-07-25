/**
  ******************************************************************************
  * File Name          : ECGP_common.h
  * Description        : ECGP common macro
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#ifndef _ECGP_COMMON_H_
#define _ECGP_COMMON_H_


typedef  unsigned long  u32;
typedef  unsigned short u16;
typedef  unsigned char  u8;
typedef  long     ECGP_error;


enum{
  ECGP_ENONE  = 0,
  ECGP_EMEMOUT,         //内存越界
  ECGP_EEND,            //错误的结束符   
  ECGP_EFULL,           //缓存已满
  ECGP_EPARA,           //输入参数错误
  ECGP_ESEQ,            //sequence error
  ECGP_ESENDACK,        //send ack error
  ECGP_ELENGTH,         //larger then maximum length
}ECGP_ERROE;


typedef enum {
    ECGP_FALSE = 0,
    ECGP_TRUE
}ECGP_Bool;



#define ECGP_POINT_U8(d)            ((u8*)d)

#define ECGP_GET_U16(a)         ( ( *ECGP_POINT_U8(a)) | (*(ECGP_POINT_U8(a)+1)<<8) )
#define ECGP_SET_U16(a,d)       while(0){                                               \
                                    *ECGP_POINT_U8(a)       = d&0xffu;        \
                                    *(ECGP_POINT_U8(a)+1)   = (d>>8)&0xffu;   \
                                }





#endif
