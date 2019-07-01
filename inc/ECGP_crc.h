#ifndef _ECGP_CRC_H_
#define _ECGP_CRC_H_

#include "ECGP_common.h"

#define ECGP_CRC8_POLY       0x32u
#define ECGP_CRC16_POLY      0x1021u        //ISO HDLC£¬ITU X.25£¬V.34/V.41/V.42£¬PPP-FCS
#define ECGP_CRC32_POLY      0x04C11DB7u    //ZIP£¬RAR£¬IEEE 802 LAN/FDDI£¬IEEE1394£¬PPP-FCS

u8  ECGP_crc8(u8* data, u32 len, u8 init);
u16 ECGP_crc16(u8* data, u32 len , u16 init);
u32 ECGP_crc32(u8* data, u32 len , u32 init);




#endif // !_ECGP_CRC_H_


