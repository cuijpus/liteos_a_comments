/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "It_posix_queue.h"

static VOID *PthreadF01(VOID *argument)
{
    INT32 ret = 0;
    const CHAR *msgptr = "test message ";

    g_testCount = 1;

    ret = LosTaskDelay(2); // 2, Set delay time.
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);

    ret = mq_send(g_gqueue, msgptr, strlen(msgptr), 0);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);

    g_testCount = 2; // 2, Init test count value.

    return NULL;
EXIT:
    g_testCount = 0;
    return NULL;
}

static VOID *PthreadF02(VOID *argument)
{
    INT32 ret = 0, len;
    CHAR  msgrv[MQUEUE_STANDARD_NAME_LENGTH] = {0};
    const CHAR *msgptr = "test message ";
    struct timespec ts;

    g_testCount = 3; // 3, Init test count value.

    ts.tv_sec = INT32_MAX;
    ts.tv_nsec = 0;

    len = mq_timedreceive(g_gqueue, msgrv, MQUEUE_STANDARD_NAME_LENGTH, NULL, &ts);

    ret = LosTaskDelay(3); // 3, Set delay time.
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);
    ICUNIT_GOTO_EQUAL(len, strlen(msgptr), len, EXIT);

    ret = mq_close(g_gqueue);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);

    ret = mq_unlink(g_gqname);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);

    g_testCount = 4; // 4, Init test count value.

    return NULL;
EXIT:
    mq_close(g_gqueue);
    mq_unlink(g_gqname);
    g_testCount = 0;
    return NULL;
}

static UINT32 Testcase(VOID)
{
    INT32 ret;
    struct mq_attr attr = { 0 };
    pthread_attr_t attr1;
    pthread_t newTh, newTh2;

    ret = snprintf_s(g_gqname, MQUEUE_STANDARD_NAME_LENGTH, MQUEUE_STANDARD_NAME_LENGTH - 1, \
                     "/mq097_%d", LosCurTaskIDGet());
    ICUNIT_GOTO_NOT_EQUAL(ret, MQUEUE_IS_ERROR, ret, EXIT3);

    attr.mq_msgsize = MQUEUE_STANDARD_NAME_LENGTH;
    attr.mq_maxmsg = MQUEUE_STANDARD_NAME_LENGTH;
    g_gqueue = mq_open(g_gqname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);
    ICUNIT_GOTO_NOT_EQUAL(g_gqueue, (mqd_t)-1, g_gqueue, EXIT);

    g_testCount = 0;

    ret = PosixPthreadInit(&attr1, MQUEUE_PTHREAD_PRIORITY_TEST1);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);

    ret = pthread_create(&newTh, &attr1, PthreadF01, NULL);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT1);

    ret = PosixPthreadInit(&attr1, MQUEUE_PTHREAD_PRIORITY_TEST2);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT2);

    ret = pthread_create(&newTh2, &attr1, PthreadF02, NULL);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT2);

    ret = LosTaskDelay(10); // 10, Set delay time.
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT2);

    TestAssertWaitDelay(&g_testCount, 4); // 4, Here, assert the g_testCount.

    ret = PosixPthreadDestroy(&attr1, newTh2);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT2);

    ret = PosixPthreadDestroy(&attr1, newTh);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT1);

    return MQUEUE_NO_ERROR;

EXIT2:
    PosixPthreadDestroy(&attr1, newTh2);
EXIT1:
    PosixPthreadDestroy(&attr1, newTh);
EXIT:
    mq_close(g_gqueue);
    mq_unlink(g_gqname);
EXIT3:
    return MQUEUE_NO_ERROR;
}

VOID ItPosixQueue097(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("IT_POSIX_QUEUE_097", Testcase, TEST_POSIX, TEST_QUE, TEST_LEVEL2, TEST_FUNCTION);
}
