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

static UINT32 Testcase(VOID)
{
    INT32 ret, i;
    INT32 count = MQUEUE_MAX_NUM_TEST;
    const CHAR *msgptr = MQUEUE_SEND_STRING_TEST;
    CHAR msgrcd[MQUEUE_STANDARD_NAME_LENGTH] = {0};
    CHAR mqname[LOSCFG_BASE_IPC_QUEUE_CONFIG + 1][MQUEUE_STANDARD_NAME_LENGTH];
    mqd_t mqueue[LOSCFG_BASE_IPC_QUEUE_CONFIG + 1];
    struct mq_attr attr = { 0 };

    attr.mq_msgsize = MQUEUE_STANDARD_NAME_LENGTH;
    attr.mq_maxmsg = 20;

    for (i = 0; i < count; i++) {
        (void)snprintf_s(mqname[i], MQUEUE_STANDARD_NAME_LENGTH, MQUEUE_STANDARD_NAME_LENGTH - 1, \
                         "mq158_%d_%d", LosCurTaskIDGet(), i);

        mqueue[i] = mq_open(mqname[i], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);
        ICUNIT_GOTO_NOT_EQUAL(mqueue[i], (mqd_t)-1, mqueue[i], EXIT);
    }

#ifndef LOSCFG_DEBUG_QUEUE
    (void)snprintf_s(mqname[i], MQUEUE_STANDARD_NAME_LENGTH, MQUEUE_STANDARD_NAME_LENGTH - 1, \
                     "/mq158_%d", i);
    mqueue[i] = mq_open(mqname[i], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &attr);
    ICUNIT_GOTO_EQUAL(mqueue[i], (mqd_t)(-1), mqueue[i], EXIT);
#else
    mqueue[i] = (mqd_t)(-1);
#endif

    ret = mq_send(mqueue[i], msgptr, strlen(msgptr), 0);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_IS_ERROR, ret, EXIT);
    ICUNIT_GOTO_EQUAL(errno, EBADF, errno, EXIT);

    ret = mq_receive(mqueue[i], msgrcd, MQUEUE_STANDARD_NAME_LENGTH, NULL);
    ICUNIT_GOTO_EQUAL(ret, MQUEUE_IS_ERROR, ret, EXIT);
    ICUNIT_GOTO_EQUAL(errno, EBADF, errno, EXIT);

    for (i = 0; i < count; i++) {
        ret = mq_close(mqueue[i]);
        ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);

        ret = mq_unlink(mqname[i]);
        ICUNIT_GOTO_EQUAL(ret, MQUEUE_NO_ERROR, ret, EXIT);
    }

    return MQUEUE_NO_ERROR;

EXIT:
    for (i = 0; i < count; i++) {
        mq_close(mqueue[i]);
        mq_unlink(mqname[i]);
    }
    return MQUEUE_NO_ERROR;
}



VOID ItPosixQueue159(VOID) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("IT_POSIX_QUEUE_159", Testcase, TEST_POSIX, TEST_QUE, TEST_LEVEL2, TEST_FUNCTION);
}
