#include "ECGP_crc.h"
#include "ECGP_config.h"

#ifdef ECGP_USE_CRC8_TABLE 
static u8 ECGP_crc8_table[256];
#endif

#ifdef ECGP_USE_CRC16_TABLE 
static u16 ECGP_crc16_table[256];
#endif

#ifdef ECGP_USE_CRC32_TABLE 
static u32 ECGP_crc32_table[256];
#endif

void crc_init()
{

}


u8  ECGP_crc8(u8* data, u32 len , u8 init)
{
	u32 i;
	u8 j,crc;

    crc = init;
	for (i = 0; i < len; i++) {
        crc ^= data[i];
		for (j = 0; j < 8; j++) {
			if ( crc & 0x80u ) {
                crc <<= 1;
				crc ^= ECGP_CRC8_POLY;
			}
            else
            {
                crc <<= 1;
            }
		}

	}
    return crc;
}

u16 ECGP_crc16(u8* data, u32 len , u16 init) 
{
    u32 i;
    u8 j;
    u16 crc;

    crc = init;
    for (i = 0; i < len; i++) {
        crc ^= (data[i]<<8);
        for (j = 0; j < 8; j++) {
            if (crc & 0x8000u) {
                crc <<= 1;
                crc ^= ECGP_CRC16_POLY;
            }
            else
            {
                crc <<= 1;
            }
        }

    }
    return crc;
}

u32 ECGP_crc32(u8* data, u32 len ,u32 init)
{
    u32 i, crc;
    u8 j;

    crc = init;
    for (i = 0; i < len; i++) {
        crc ^= (data[i]<<24);
        for (j = 0; j < 8; j++) {
            if (crc & 0x80000000u) {
                crc <<= 1;
                crc ^= ECGP_CRC32_POLY;
            }
            else
            {
                crc <<= 1;
            }
        }

    }
    return crc;
}