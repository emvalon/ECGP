/**
 * @file main.c
 * @author Weilong Shen (valonshen@foxmail.com)
 * @brief Linux example
 * @version 0.1
 * @date 2021-07-06
 * 
 * Copyright © 2019-2021 Weilong Shen (valonshen@foxmail.com).
 * All rights reserved.
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include "ECGP.h"

int main(void)
{
    printf("-----------------------------------------------\n");
    printf("            ECGP Linux example\n");
    printf("-----------------------------------------------\n");

    while (ECGP_TRUE)
    {
        ECGP_timeElapsed(10);
        usleep(10);
    }
    
    return 0;
}