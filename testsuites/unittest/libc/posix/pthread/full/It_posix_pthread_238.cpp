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
#include "It_posix_pthread.h"

static VOID *pthread_f01(VOID *argument)
{
    INT32 ret = 0;

    ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    ICUNIT_TRACK_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    ret = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    ICUNIT_TRACK_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    g_testCount = 1;

    g_pthreadSem = PTHREAD_INMAIN_TEST;

    while (g_pthreadSem == PTHREAD_INMAIN_TEST) {
        sleep(1);
    }

    pthread_testcancel();

    g_testCount = -1;
    pthread_exit(0);
    return NULL;
}


static UINT32 Testcase(VOID)
{
    pthread_t newTh;
    INT32 ret = 0;

    g_pthreadSem = PTHREAD_INTHREAD_TEST;
    g_testCount = 0;

    ret = pthread_create(&newTh, NULL, pthread_f01, NULL);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    while (g_pthreadSem == PTHREAD_INTHREAD_TEST) {
        sleep(1);
    }

    ret = pthread_cancel(newTh);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);

    g_pthreadSem = PTHREAD_INTHREAD_TEST;

    ret = pthread_join(newTh, NULL);
    ICUNIT_ASSERT_EQUAL(ret, PTHREAD_NO_ERROR, ret);
    ICUNIT_ASSERT_EQUAL(g_testCount, 1, g_testCount);

    return PTHREAD_NO_ERROR;
}


VOID ItPosixPthread238(VOID)
{
    TEST_ADD_CASE("IT_POSIX_PTHREAD_238", Testcase, TEST_POSIX, TEST_PTHREAD, TEST_LEVEL2, TEST_FUNCTION);
}
