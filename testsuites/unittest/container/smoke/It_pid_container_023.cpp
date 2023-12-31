/*
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
#include "It_container_test.h"

static int ChildFunc(void *arg)
{
    int value = *((int*)arg);
    if (value != CHILD_FUNC_ARG) {
        return EXIT_CODE_ERRNO_5;
    }

    if (getpid() != CONTAINER_FIRST_PID) {
        return EXIT_CODE_ERRNO_1;
    }

    if (getppid() != 0) {
        return EXIT_CODE_ERRNO_2;
    }

    sleep(1);
    return 0;
}

void ItPidContainer023(void)
{
    int ret;
    void *stack = malloc(STACK_SIZE);
    ASSERT_TRUE(stack != NULL);

    int arg = CHILD_FUNC_ARG;
    auto pid = clone(ChildFunc, (char *)stack + STACK_SIZE, CLONE_NEWPID, &arg);
    free(stack);
    ASSERT_NE(pid, -1);

    int status;
    ret = waitpid(pid, &status, 0);
    ASSERT_EQ(ret, pid);

    ret = WIFEXITED(status);
    ASSERT_NE(ret, 0);

    int exitCode = WEXITSTATUS(status);
    ASSERT_EQ(exitCode, 0);
}
