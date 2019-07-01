#include "ECGP_link.h"
#include "ECGP_config.h"
#include "ECGP_common.h" 





static void link_code(u8 data);
/*
    功能：  将上层数据进行封装
    参数：  sec     输入数据
            dst     输出数据，最大长度为2*len+2
            len     输入数据长度
    返回：  封装后的数据长度
*/
static u16 link_frame( u8* src, u8* dst, u16 len )
{
    u16 i,index=1,crc;

    crc = ECGP_CRC8(src,len);
    dst[0]  =   ECGP_END;
    for( i=0; i<len; i++ ){
        if( src[i] == ECGP_END ){
            dst[index++] = ECGP_ESC;
            dst[index++] = ECGP_ESC_END;
        }
        else if( src[i] == ECGP_ESC ){
            dst[index++] = ECGP_ESC;
            dst[index++] = ECGP_ESC_ESC;
        }
        else
        {
            dst[index++] = src[i];
        }
    }

    if( crc == ECGP_END ){
            dst[index++] = ECGP_ESC;
            dst[index++] = ECGP_ESC_END;
    }
    else if( crc == ECGP_ESC ){
        dst[index++] = ECGP_ESC;
        dst[index++] = ECGP_ESC_ESC;
    }
    else
    {
        dst[index++] = crc;
    }

    dst[index++]  =   ECGP_END;
    return index;
}

































