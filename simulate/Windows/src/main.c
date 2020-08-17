/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : show how to use ECGP
  ******************************************************************************

  * Copyright(c) 2019 Valon Shen (valonshen@foxmail.com)
  * All rights reserved.
*/

#include "stdio.h"
#include <windows.h>
#include "ECGP.h"
#include <stdbool.h>


DWORD WINAPI recv_thread(LPVOID lpParameter);
DWORD WINAPI timeManagement_thread(LPVOID lpParameter);

void rx_cb(u16 num)
{
    //printf("\nrecv %d\n", num);
}
void tx_cb(u16 num)
{
    //printf("tx num: %d\n", num);
}

int main()
{
    HANDLE thread;
    u8* p;
    ECGP_error res; 

	ECGP_init();
    ECGP_setRxCallback(rx_cb);
    ECGP_setTxCallback(tx_cb);
    p = malloc(102);
   
   
    thread = CreateThread(NULL, 0, recv_thread, NULL, 0, NULL);
    CloseHandle(thread);

    thread = CreateThread(NULL, 0, timeManagement_thread, NULL, 0, NULL);
    CloseHandle(thread);

    Sleep(20);
    while (true)
    {
        memset(p, 0, 102);
        printf("\ninput your message :");
        scanf_s("%s",p,102);
        res = ECGP_send(p,(u16)strlen(p));
        if (res != ECGP_ENONE) {
            printf("send fault :%d\n", res);
        }
        Sleep(20);
    }
}


DWORD WINAPI recv_thread(LPVOID lpParameter)
{
    u8 *p;
    ECGP_error res;
    p = malloc(101);

    Sleep(20);
    while (true) 
    {
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


DWORD WINAPI timeManagement_thread(LPVOID lpParameter)
{
    ECGP_error res;
    while (true)
    {
        res = ECGP_timeElapsed(10);
        if (res != ECGP_ENONE) {
            printf("\ntime management error:%d\n", res);
            ECGP_init();
        }
        Sleep(10);
    }
    
}





