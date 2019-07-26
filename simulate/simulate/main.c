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
    int len;
    link_init();

    p[0] = 1;
    p[1] = 2;
    p[2] = 3;
   
    thread = CreateThread(NULL, 0, recv_thread, NULL, 0, NULL);
    CloseHandle(thread);

    //while (1) 
    {
        ECGP_send(p,3);
    }
    


    getch();
}


DWORD WINAPI recv_thread(LPVOID lpParameter)
{
  
    printf("recv_thread正在运行!\r\n");
    while (1) {
        
        Sleep(10);
    }
    
}





