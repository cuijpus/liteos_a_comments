/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd. All rights reserved.
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

static UINT32 Testcase(VOID)
{
    CHAR qname[MQUEUE_STANDARD_NAME_LENGTH] = "";
    const CHAR *msgptr = MQUEUE_SEND_STRING_TEST;
    mqd_t queue;
    INT32 ret;

    ret = snprintf_s(qname, MQUEUE_STANDARD_NAME_LENGTH, MQUEUE_STANDARD_NAME_LENGTH - 1, \
                     "/mq046-1_%d", LosCurTaskIDGet());
    ICUNIT_GOTO_NOT_EQUAL(ret, MQUEUE_IS_ERROR, ret, EXIT);

    queue = mq_open(qname, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, NULL);
    ICUNIT_GOTO_NOT_EQUAL(queue, (mqd_t)-1, queue, EXIT);

    ret = mq_send((mqd_t)(&queue + 1), msgptr, strlen(msgptr), 0);
    ICUNIT_GOTO_EQUAL(ret, -1, ret, EXIT2);
    ICUNIT_GOTO_EQUAL(errno, EBADF, errno, EXIT2);

    ret = mq_close(queue);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT1);

    ret = mq_unlink(qname);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

    return MQUEUE_NO_ERROR;
EXIT2:
    mq_close(queue);
EXIT1:
    mq_unlink(qname);
EXIT:
    return MQUEUE_IS_ERROR;
}

VOID ItPosixQueue046(VOID)
{
    TEST_ADD_CASE("IT_POSIX_QUEUE_046", Testcase, TEST_POSIX, TEST_QUE, TEST_LEVEL2, TEST_FUNCTION);
}
