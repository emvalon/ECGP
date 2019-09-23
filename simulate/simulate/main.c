#include "stdio.h"
#include <windows.h>
#include "ECGP.h"


DWORD WINAPI recv_thread(LPVOID lpParameter);


void rx_cb(u16 num)
{
   // printf("\nrecv %d\n", num);
}

int main()
{
    HANDLE thread;
    u8* p;
    ECGP_error res; 

	ECGP_init();
    ECGP_setRxCallback(rx_cb);
    p = malloc(102);
   
   
    thread = CreateThread(NULL, 0, recv_thread, NULL, 0, NULL);
    CloseHandle(thread);

    while (1) 
    {
        memset(p, 0, 102);
        printf("input your message :");
        scanf_s("%s",p,102);
        res = ECGP_send(p,strlen(p));
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
    p = malloc(101);
    while (1) {
		res = ECGP_timeElapsed(10);
		if (res != ECGP_ENONE) {
            printf("\ntime error:%d\n", res);
			ECGP_init();
			//while (1);
        }
		do
		{
			res = ECGP_recv(p, 100);
			if (res < 0) {
				printf("recv error:%d\r\n", res);
			}
			else if(res > 0 ){
				printf("\nread length %d : ",res);
				for (int i = 0; i < res; i++) {
					printf("%c", p[i]);
				}
				printf("\n");
			}
		} while (res>0);
        
        
       
        Sleep(10);
    }
    
}





