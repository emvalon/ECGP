#ifndef _TESTUTIL_CONFIG_H_
#define _TESTUTIL_CONFIG_H_

#include "stdlib.h"
#include "stdbool.h"

typedef unsigned int os_stack_t;
typedef void (*os_task_func_t)(void *);

#define SYSINIT_ASSERT_ACTIVE()     
#define MYNEWT_VAL(x)       (0)
#define OS_STACK_ALIGN(x)   ((x+3)/4)

#define os_malloc           malloc
#define os_free             free


#define OS_MAIN_STACK_SIZE      4000

#endif // _TESTUTIL_CONFIG_H_