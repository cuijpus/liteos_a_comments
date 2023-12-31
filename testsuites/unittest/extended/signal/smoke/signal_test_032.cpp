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
#include "it_test_signal.h"
#include "signal.h"

static int g_sigCount = 0;
static void SigPrint(int sig)
{
    (void)sig;
    g_sigCount++;
    printf("signal receive success\n");
}

static UINT32 TestCase(VOID)
{
    int pid = 0;
    int ret = 0;
    void (*retptr)(int) = NULL;
    sigset_t newset, oldset;
    pid = fork();
    if (pid < 0) {
        printf("Fork error\n");
        return LOS_NOK;
    } else if (pid == 0) {
        printf("test032 raise before\n");
        sigemptyset(&newset);
        sigaddset(&newset, SIGUSR1);
        ret = sigprocmask(SIG_BLOCK, &newset, &oldset);
        if (ret != 0) {
            printf("error:sigprocmask\n");
            exit(LOS_NOK);
        }
        retptr = signal(SIGUSR1, SigPrint);
        if (retptr == SIG_ERR) {
            printf("error:signal\n");
            exit(LOS_NOK);
        }
        raise(SIGUSR1);
        printf("test032 raise after\n");
        exit(g_sigCount);
    }

    wait(&ret);
    ICUNIT_ASSERT_EQUAL(WEXITSTATUS(ret), 0, WEXITSTATUS(ret));
    return LOS_OK;
}

void ItPosixSignal032(void)
{
    TEST_ADD_CASE(__FUNCTION__, TestCase, TEST_POSIX, TEST_SIGNAL, TEST_LEVEL0, TEST_FUNCTION);
}
