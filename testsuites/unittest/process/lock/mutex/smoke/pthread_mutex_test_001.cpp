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
#include "it_mutex_test.h"

static pthread_mutex_t g_mutexLock;

static const unsigned int TEST_COUNT = 5;
static int g_testInfo[5] = { 0 };
static int g_testToCount = 0;
static int g_testBackCount = 0;

static void *ThreadFuncTest3(void *a)
{
    int ret;
    int tid = Gettid();
    pthread_t thread = pthread_self();

    ret = pthread_detach(thread);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

    g_testInfo[g_testToCount] = tid;
    g_testToCount++;

    ret = pthread_mutex_lock(&g_mutexLock);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

    ICUNIT_GOTO_EQUAL(g_testInfo[g_testBackCount], tid, tid, EXIT);
    g_testBackCount++;

    ret = pthread_mutex_unlock(&g_mutexLock);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

EXIT:
    return nullptr;
}

static int Testcase(void)
{
    int ret;
    pthread_t newPthread;
    pthread_mutexattr_t mutex;
    int index = 0;
    pthread_mutexattr_settype(&mutex, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&g_mutexLock, &mutex);

    while (index < 1) {
        g_testToCount = 0;
        g_testBackCount = 0;

        ret = pthread_mutex_lock(&g_mutexLock);
        ICUNIT_ASSERT_EQUAL(ret, 0, ret);

        for (int count = 0; count < TEST_COUNT; count++) {
            ret = pthread_create(&newPthread, nullptr, ThreadFuncTest3, 0);
            ICUNIT_ASSERT_EQUAL(ret, 0, ret);

            SLEEP_AND_YIELD(1);
        }

        ret = pthread_mutex_unlock(&g_mutexLock);
        ICUNIT_ASSERT_EQUAL(ret, 0, ret);

        SLEEP_AND_YIELD(1);

        ICUNIT_ASSERT_EQUAL(g_testBackCount, TEST_COUNT, g_testBackCount);
        ICUNIT_ASSERT_EQUAL(g_testToCount, TEST_COUNT, g_testToCount);

        index++;
    }

    pthread_mutex_destroy(&g_mutexLock);
    return 0;
}

void ItTestPthreadMutex001(void)
{
    TEST_ADD_CASE("IT_POSIX_PTHREAD_MUTEX_001", Testcase, TEST_POSIX, TEST_MEM, TEST_LEVEL0, TEST_FUNCTION);
}
