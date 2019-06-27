#ifndef _ECGP_COMMON_H_
#define _ECGP_COMMON_H_


typedef  unsigned long  u32;
typedef  unsigned short u16;
typedef  unsigned char  u8;


#define ECGP_POINT_U8(d)            ((u8*)d)

#define ECGP_GET_U16(a)         ( *ECGP_POINT_U8(a)) | (*(ECGP_POINT_U8(a)+1)<<8) )
#define ECGP_SET_U16(a,d)       {                                               \
                                    *ECGP_POINT_U8(a)       = d&0x00ffu;        \
                                    *(ECGP_POINT_U8(a)+1)   = (d>>8)&0x00ffu;   \
                                                                                }





#endif
