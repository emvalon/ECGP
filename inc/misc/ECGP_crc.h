/**
 * @file ECGP_crc.h
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
#ifndef _ECGP_CRC_H_
#define _ECGP_CRC_H_

#include "ECGP_common.h"

#define ECGP_CRC8_POLY       0x32u
#define ECGP_CRC16_POLY      0x1021u        //ISO HDLC��ITU X.25��V.34/V.41/V.42��PPP-FCS
#define ECGP_CRC32_POLY      0x04C11DB7u    //ZIP��RAR��IEEE 802 LAN/FDDI��IEEE1394��PPP-FCS


u8  ECGP_crc8(u8* data, u32 len, u8 init);
u16 ECGP_crc16(u8* data, u32 len , u16 init);
u32 ECGP_crc32(u8* data, u32 len , u32 init);




#endif // !_ECGP_CRC_H_


