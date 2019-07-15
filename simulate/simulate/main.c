#include "stdio.h"
#include <windows.h>
#include "ECGP_link.h"


DWORD WINAPI recv_thread(LPVOID lpParameter);
ECGP_error link_frame(u8* src, u16 len);
ECGP_error link_verfy(u8* dst, u16 len);

int main()
{
    HANDLE thread;
    u8* p;
    int len;
    link_init();

    p = malloc(1024);
    thread = CreateThread(NULL, 0, recv_thread, NULL, 0, NULL);
    CloseHandle(thread);
    //while (1) 
    {
        printf("input your message\n");
        p[0] = 1;
        p[1] = 2;
        len = 10;
       // len = scanf_s("%s",p);
        if (link_frame(p,len) != ECGP_ENONE) {
            printf("send fault!\n");
        }
        link_test();
    }
    


    getch();
}


DWORD WINAPI recv_thread(LPVOID lpParameter)
{
    u8* p = malloc(1024);
    ECGP_error r;
    printf("recv_thread正在运行!\r\n");
    while (1) {
        r = link_verfy(p, 1024);
        if (r > 0) {
            for (int i = 0; i < r; i++) {
                printf("%02x ", p[i]);
            }
            printf("\n");
        }
        else if(r < 0){
            printf("read error:%d\n", r);
        }
        Sleep(10);
    }
    
}





