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

static mqd_t g_mqueue;
static CHAR g_mqname[MQUEUE_STANDARD_NAME_LENGTH] = "";
static int g_signo;

static void SigUsr1(int signo)
{
    CHAR msgrcd[MQUEUE_STANDARD_NAME_LENGTH] = {0};
    int ret;

    g_signo = 1;

    ret = mq_receive(g_mqueue, msgrcd, MQUEUE_STANDARD_NAME_LENGTH, nullptr);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_SHORT_ARRAY_LENGTH, ret, EXIT);
    ICUNIT_GOTO_STRING_EQUAL(msgrcd, MQUEUE_SEND_STRING_TEST, msgrcd, EXIT);

    return;
EXIT:
    exit(255); // 255, Set a exit status.
}
static void Child(void)
{
    int ret;
    struct sigevent sigev = {0};
    const CHAR *msgptr = MQUEUE_SEND_STRING_TEST;
    struct mq_attr attr;

    attr.mq_msgsize = MQUEUE_STANDARD_NAME_LENGTH;
    attr.mq_maxmsg = 1;

    ret = snprintf_s(g_mqname, MQUEUE_STANDARD_NAME_LENGTH - 1, MQUEUE_STANDARD_NAME_LENGTH, "/mq207_%d", getpid());
    ICUNIT_GOTO_NOT_EQUAL(ret, MQUEUE_IS_ERROR, ret, EXIT1);

    g_mqueue = mq_open(g_mqname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);
    ICUNIT_GOTO_NOT_EQUAL(g_mqueue, (mqd_t)-1, g_mqueue, EXIT);

    signal(SIGUSR1, SigUsr1);

    sigev.sigev_notify = SIGEV_NONE;
    g_signo = 0;

    ret = mq_notify(g_mqueue, &sigev);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);
    ret = mq_send(g_mqueue, msgptr, strlen(msgptr), 0);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);

    for (int i = 0; i < 5; i++) { // 5, The loop frequency.
        sleep(1);
    }
    ICUNIT_GOTO_EQUAL(g_signo, 0, g_signo, EXIT);

    mq_close(g_mqueue);
    mq_unlink(g_mqname);

    exit(10); // 10, Set a exit status.
EXIT:
    mq_close(g_mqueue);
    mq_unlink(g_mqname);
EXIT1:
    return;
}

static UINT32 Testcase(VOID)
{
    int ret;
    int status;

    pid_t pid = fork();
    ICUNIT_GOTO_WITHIN_EQUAL(pid, 0, 100000, pid, EXIT); // 100000, Valid range value of pid.

    if (pid == 0) {
        Child();
        exit(255); // 255, Set a exit status.
    }

    ret = waitpid(pid, &status, 0);
    ICUNIT_ASSERT_EQUAL(ret, pid, ret);

    status = WEXITSTATUS(status);
    ICUNIT_ASSERT_EQUAL(status, 10, status); // 10, Here, assert the ret.

    return 0;
EXIT:
    return 1;
}

VOID ItPosixQueue207(VOID)
{
    TEST_ADD_CASE("IT_POSIX_QUEUE_207", Testcase, TEST_POSIX, TEST_QUE, TEST_LEVEL2, TEST_FUNCTION);
}