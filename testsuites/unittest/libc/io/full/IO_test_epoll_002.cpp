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

#include "It_test_IO.h"
#include <sys/epoll.h>
#include "signal.h"
#include "pthread.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static void SigPrint(int sig)
{
    return;
}

static UINT32 testcase(VOID)
{
    fd_set rfds;
    struct timespec tv;
    int retval;
    pid_t pid;
    int pipeFd[2]; /* 2, pipe id num */
    char buffer[40]; /* 40, buffer size */
    int i = 0;
    int status;

    int epFd = 0;
    sigset_t mask;
    void (*retSig)(int);
    struct epoll_event ev;
    struct epoll_event evWait[2];

    retSig = signal(SIGALRM, SigPrint);
    ICUNIT_ASSERT_NOT_EQUAL(retSig, NULL, retSig);

    retSig = signal(SIGUSR1, SigPrint);
    ICUNIT_ASSERT_NOT_EQUAL(retSig, NULL, retSig);

    retval = sigemptyset(&mask);
    ICUNIT_ASSERT_EQUAL(retval, 0, retval);

    retval = sigaddset(&mask, SIGALRM);
    ICUNIT_ASSERT_EQUAL(retval, 0, retval);

    retval = sigaddset(&mask, SIGUSR1);
    ICUNIT_ASSERT_EQUAL(retval, 0, retval);

    retval = pipe(pipeFd);
    ICUNIT_GOTO_EQUAL(retval, 0, retval, OUT3);

    /* Watch fd to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(pipeFd[0], &rfds);

    /* Wait up to three seconds. */
    tv.tv_sec = 3; /* 3, wait timer, second */
    tv.tv_nsec = 5; /* 5, wait timer, nano second */

    epFd = epoll_create1(100); /* 100, cretae input, */
    ICUNIT_GOTO_NOT_EQUAL(epFd, -1, epFd, OUT2);

    ev.events = EPOLLRDNORM;
    retval = epoll_ctl(epFd, EPOLL_CTL_ADD, pipeFd[0], &ev);

    ICUNIT_GOTO_NOT_EQUAL(retval, -1, retval, OUT1);

    pid = fork();
    if (pid == 0) {
        close(pipeFd[1]);

        (void)memset_s(evWait, sizeof(struct epoll_event) * 2, 0, sizeof(struct epoll_event) * 2); /* 2, evs num */
        evWait[0].data.fd = pipeFd[0];

        retval = epoll_pwait(epFd, evWait, 2, 3000, &mask); /* 2, num of wait fd. 3000, wait time */
        close(pipeFd[0]);

        if (retval) {
            exit(LOS_OK);
        } else {
            exit(LOS_NOK);
        }
    } else {
        sleep(1);
        close(pipeFd[0]);

        retval = kill(pid, SIGALRM);
        ICUNIT_ASSERT_EQUAL(retval, 0, retval);

        retval = kill(pid, SIGUSR1);
        ICUNIT_ASSERT_EQUAL(retval, 0, retval);
        close(pipeFd[1]);

        wait(&status);
        status = WEXITSTATUS(status);
        ICUNIT_ASSERT_EQUAL(status, LOS_OK, status);
    }

    return LOS_OK;
OUT1:
    close(epFd);
OUT2:
    close(pipeFd[0]);
    close(pipeFd[1]);
OUT3:
    return LOS_NOK;
}


VOID IO_TEST_EPOLL_002(VOID)
{
    TEST_ADD_CASE(__FUNCTION__, testcase, TEST_LIB, TEST_LIBC, TEST_LEVEL1, TEST_FUNCTION);
}
