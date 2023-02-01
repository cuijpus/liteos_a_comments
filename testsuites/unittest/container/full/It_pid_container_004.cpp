/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd. All rights reserved.
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

static int ChildFun(void *p)
{
    (void)p;
    return getpid();
}

static int ChildFunClone2(void *p)
{
    (void)p;
    void *pstk = malloc(STACK_SIZE);
    if (pstk == NULL) {
        return EXIT_CODE_ERRNO_1;
    }
    int childPid = clone(ChildFun, (char *)pstk + STACK_SIZE, CLONE_NEWPID | SIGCHLD, NULL);
    if (childPid != -1) {
        free(pstk);
        return EXIT_CODE_ERRNO_2;
    }

    free(pstk);
    return 0;
}

static int ChildFunClone1(void *p)
{
    (void)p;
    int ret = 0;
    int status;
    pid_t pid = getpid();
    int childFunRet = (int)pid;
    void *pstk = malloc(STACK_SIZE);
    if (pstk == NULL) {
        return EXIT_CODE_ERRNO_2;
    }
    int childPid = clone(ChildFunClone2, (char *)pstk + STACK_SIZE, CLONE_NEWPID | SIGCHLD, NULL);
    if (childPid == -1) {
        free(pstk);
        return EXIT_CODE_ERRNO_3;
    }

    ret = waitpid(childPid, &status, 0);
    if (ret != childPid) {
        return EXIT_CODE_ERRNO_4;
    }
    ret = WIFEXITED(status);
    if (ret == 0) {
        return EXIT_CODE_ERRNO_5;
    }
    ret = WEXITSTATUS(status);
    if (ret != 0) {
        free(pstk);
        return EXIT_CODE_ERRNO_6;
    }

    free(pstk);
    return childFunRet;
}

void ItPidContainer004(void)
{
    int ret = 0;
    int status;
    void *pstk = malloc(STACK_SIZE);
    ASSERT_TRUE(pstk != NULL);
    int childPid = clone(ChildFunClone1, (char *)pstk + STACK_SIZE, CLONE_NEWPID | SIGCHLD, NULL);
    free(pstk);
    ASSERT_NE(childPid, -1);

    ret = waitpid(childPid, &status, 0);
    ASSERT_EQ(ret, childPid);
    ret = WIFEXITED(status);
    ASSERT_NE(ret, 0);
    ret = WEXITSTATUS(status);
    ASSERT_EQ(ret, CONTAINER_FIRST_PID);
}
