#include "stdio.h"
#include "ECGP_crc.h"

#define INIT            0
u8 d[] = { 0,0,1,2,3,4,5,6,7,8,9};
int main()
{
    printf("crc8:%02x\n", ECGP_crc8(d, sizeof(d), INIT));
    printf("crc16:%04x\n", ECGP_crc16(d, sizeof(d), INIT));
    printf("crc32:%08x\n", ECGP_crc32(d, sizeof(d), INIT));
    getch();
}
