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
#include "fcntl.h"

int TestPipeSingleProcessFcntl()
{
    int pipefd[2]; // 2, array subscript
    pid_t pid;
    int retValue = -1;
    retValue = pipe(pipefd);
    ICUNIT_ASSERT_EQUAL(retValue, 0, retValue);

    int *readFd = &pipefd[0];
    int *writeFd = &pipefd[1];

    char readbuffer[100];
    int status, ret;

    int flag = fcntl(*readFd, F_GETFL);
    fcntl(*readFd, F_SETFL, flag | O_NONBLOCK);

    pid = fork();
    if (pid == -1) {
        printf("Fork Error!\n");
        return -1;
    } else if (pid == 0) {
        for (int i = 0; i < 3; i++) { // 3, Number of cycles
            errno = 0;
            char sentence1[15] = "Hello World";
            char a[4] = {0};
            (void)sprintf_s(a, sizeof(a), "%d", i);
            (void)strcat_s(sentence1, 15, a); // 15, sizeof sentence1
            int ret = write(*writeFd, sentence1, strlen(sentence1) + 1);
            usleep(100000); // 100000, Used to calculate the delay time.
        }
        char sentence1[15] = "Hello World";
        char a[4] = {0};
        usleep(10000); // 10000, Used to calculate the delay time.
        printf("read\n");
        (void)memset_s(readbuffer, sizeof(readbuffer), 0, sizeof(readbuffer));
        retValue = read(*readFd, readbuffer, sizeof(readbuffer));
        printf("Receive %d bytes data : %s,%d\n", retValue, readbuffer, errno);
        printf("Receive %d bytes data : %s,%d\n", retValue, readbuffer + 13, errno); // 13, readbuffer offset.
        printf("Receive %d bytes data : %s,%d\n", retValue, readbuffer + 26, errno); // 26, readbuffer offset.
        ICUNIT_ASSERT_SIZE_STRING_EQUAL(readbuffer, "Hello World0", strlen(sentence1), errno);
        ICUNIT_ASSERT_SIZE_STRING_EQUAL(readbuffer + 13, "Hello World1", strlen(sentence1), errno); // 13, readbuffer offset.
        ICUNIT_ASSERT_SIZE_STRING_EQUAL(readbuffer + 26, "Hello World2", strlen(sentence1), errno); // 26, readbuffer offset.
        usleep(100000); // 100000, Used to calculate the delay time.

        printf("read\n");
        (void)memset_s(readbuffer, sizeof(readbuffer), 0, sizeof(readbuffer));
        fcntl(*readFd, F_SETFL, O_NONBLOCK);
        ret = fcntl(*readFd, F_GETFL, O_NONBLOCK);
        printf("fctrl ret=%d,O_NONBLOCK=%d\n", ret, O_NONBLOCK);
        retValue = read(*readFd, readbuffer, sizeof(readbuffer));
        printf("Receive %d bytes data : %s,%d\n", retValue, readbuffer, errno);

        ICUNIT_ASSERT_EQUAL(retValue, -1, retValue);
        ICUNIT_ASSERT_EQUAL(errno, EAGAIN, errno);
        ICUNIT_ASSERT_SIZE_STRING_EQUAL(readbuffer, readbuffer, strlen(sentence1), errno);
        usleep(100000); // 100000, Used to calculate the delay time.
        exit(0);
    }
    ret = waitpid(pid, &status, 0);
    ICUNIT_ASSERT_EQUAL(ret, pid, ret);
    ret = close(pipefd[0]);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);
    ret = close(pipefd[1]);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);
    return 0;
}

void ItPosixPipe003(void)
{
    TEST_ADD_CASE(__FUNCTION__, TestPipeSingleProcessFcntl, TEST_POSIX, TEST_MEM, TEST_LEVEL0, TEST_FUNCTION);
}
