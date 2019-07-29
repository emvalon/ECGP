#include "stdio.h"
#include <windows.h>
#include "ECGP.h"


DWORD WINAPI recv_thread(LPVOID lpParameter);
ECGP_error link_frame(u8* src, u16 len);
ECGP_error link_verfy(u8* dst, u16 len);

int main()
{
    HANDLE thread;
    u8* p;
    ECGP_error res;

    link_init();

    p = malloc(100);
   
   
    thread = CreateThread(NULL, 0, recv_thread, NULL, 0, NULL);
    CloseHandle(thread);

    while (1) 
    {
        printf("input your message : ");
        scanf_s("%s",p,100);
        res = ECGP_send(p,sizeof(p));
        if (res != ECGP_ENONE) {
            printf("send fault :%d\n", res);
        }
    }
    


    getch();
    free(p);
}


DWORD WINAPI recv_thread(LPVOID lpParameter)
{
    u8 *p;
    ECGP_error res;
    p = malloc(100);
    printf("recv_thread正在运行!\r\n");
    while (1) {
        res = ECGP_recv(p, 100);
        if (res < 0) {
            printf("recv error:%d\r\n", res);
        }
        else if(res > 0 ){
            for (int i = 0; i < res; i++) {
                printf("0x%02x ", p[i]);
            }
            printf("\n");
        }
        Sleep(10);
    }
    
}





