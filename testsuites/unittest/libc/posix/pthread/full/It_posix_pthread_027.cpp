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

static void PthreadCleanF01(void *arg)
{
    if (g_testCount == 1)
        ICUNIT_ASSERT_EQUAL_VOID((int)arg, 8, (int)arg);
    if (g_testCount == 2)
        ICUNIT_ASSERT_EQUAL_VOID((int)arg, 9, (int)arg);

    g_testCount++;

    return;
}

static VOID *pthread_f01(void *argument)
{
    g_testCount++;

    pthread_cleanup_push(PthreadCleanF01, static_cast<void *>(9)); // 9: arg that routine is called with
    pthread_cleanup_push(PthreadCleanF01, static_cast<void *>(8)); // 8: arg that routine is called with
    pthread_exit(static_cast<void *>(8)); // 8: exit value for testing
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);

    return static_cast<void *>(9); // 9: return value for testing
}

static UINT32 Testcase(VOID)
{
    pthread_t newTh;

    UINT32 ret;
    UINTPTR uwtemp = 1;

    g_testCount = 0;

    ret = pthread_create(&newTh, NULL, pthread_f01, NULL);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);
    LosTaskDelay(1);
    ICUNIT_ASSERT_EQUAL(g_testCount, 3, g_testCount);

    ret = pthread_join(newTh, (void **)&uwtemp);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);
    ICUNIT_ASSERT_EQUAL(uwtemp, 8, uwtemp);

    return PTHREAD_NO_ERROR;
}

VOID ItPosixPthread027(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("IT_POSIX_PTHREAD_027", Testcase, TEST_POSIX, TEST_PTHREAD, TEST_LEVEL2, TEST_FUNCTION);
}
