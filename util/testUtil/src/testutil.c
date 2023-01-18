/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 * Modified by 2022 Shen Weilong (valonshen@foxmail.com)
 */

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include "testutil/testutil.h"
#include "testutil_priv.h"

/* The test task runs at a lower priority (greater number) than the default
 * task.  This allows the test task to assume events get processed as soon as
 * they are initiated.  The test code can then immediately assert the expected
 * result of event processing.
 */
#define TU_TEST_TASK_PRIO   (MYNEWT_VAL(OS_MAIN_TASK_PRIO) + 1)
#define TU_TEST_STACK_SIZE  1024

struct tu_config tu_config;

int tu_any_failed;

struct ts_testsuite_list *ts_suites;

void
tu_set_pass_cb(tu_case_report_fn_t *cb, void *cb_arg)
{
    tu_config.pass_cb = cb;
    tu_config.pass_arg = cb_arg;
}

void
tu_set_fail_cb(tu_case_report_fn_t *cb, void *cb_arg)
{
    tu_config.fail_cb = cb;
    tu_config.fail_arg = cb_arg;
}

void 
tu_set_pre_test_cb(tu_pre_test_fn_t *cb, void *cb_arg)
{
    tu_config.pre_test_cb = cb;
    tu_config.pre_test_arg = cb_arg;
}

void 
tu_set_post_test_cb(tu_post_test_fn_t *cb, void *cb_arg)
{
    tu_config.post_test_cb = cb;
    tu_config.post_test_arg = cb_arg;
}

void
tu_pre_test(void)
{
    if (tu_config.pre_test_cb != NULL) {
        tu_config.pre_test_cb(tu_config.pre_test_arg);
    }
}

void
tu_post_test(void)
{
    if (tu_config.post_test_cb != NULL) {
        tu_config.post_test_cb(tu_config.post_test_cb);
    }
}


static void
tu_pass_cb_self(const char *msg, void *arg)
{
    printf("\e[32;1m[Pass]\e[0m %s/%s %s\n", tu_config.ts_suite_name, tu_case_name, msg);
    fflush(stdout);
}

static void
tu_fail_cb_self(const char *msg, void *arg)
{
    printf("\e[31;1m[Fail]\e[0m %s/%s %s\n", tu_config.ts_suite_name, tu_case_name, msg);
    fflush(stdout);
}

void
tu_init(void)
{
    tu_set_pass_cb(tu_pass_cb_self, NULL);
    tu_set_fail_cb(tu_fail_cb_self, NULL);
}