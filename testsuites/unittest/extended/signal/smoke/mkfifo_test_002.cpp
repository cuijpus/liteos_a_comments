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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static const int NAME_BUF_SIZE = 500;
static const int FAULT1 = 6;
static const int FAULT2 = 7;

static int TestMkfifoReturn()
{
    int retValue, i;
    int status = 0;
    pid_t pid;

    retValue = mkfifo("/dev/fifo0", 0777); // 0777, mkfifo config.
    ICUNIT_ASSERT_EQUAL(retValue, 0, retValue);
    retValue = mkfifo("/dev/fifo0", 0777); // 0777, mkfifo config.
    ICUNIT_GOTO_EQUAL(retValue, -1, retValue, EXIT);
    ICUNIT_GOTO_EQUAL(errno, EEXIST, errno, EXIT);

    unlink("/usr/fifo0");
    unlink("/dev/fifo0/fifo1");
    unlink("/dev/usr/fifo0");
    rmdir("/dev/usr");
    unlink("/dev/fifo0");
    return LOS_OK;

EXIT:
    unlink("/usr/fifo0");
    unlink("/dev/fifo0/fifo1");
    unlink("/dev/usr/fifo0");
    rmdir("/dev/usr");
    unlink("/dev/fifo0");
    return LOS_NOK;
}

void ItPosixMkfifo002(void)
{
    TEST_ADD_CASE(__FUNCTION__, TestMkfifoReturn, TEST_POSIX, TEST_MEM, TEST_LEVEL0, TEST_FUNCTION);
}
