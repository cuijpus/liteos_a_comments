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

#include "It_posix_mutex.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/* pthread_mutex_unlock 1-1.c
 * Test that pthread_mutex_unlock()
 *   shall release the mutex object 'mutex'.

 * Steps:
 *   -- initialize a mutex object
 *   -- Get the mutex using pthread_mutex_lock()
 *   -- Release the mutex using pthread_mutex_unlock()
 *   -- Try to get the mutex using pthread_mutex_trylock()
 *   -- Release the mutex using pthread_mutex_unlock()
 *
 */
static UINT32 Testcase(VOID)
{
    int rc;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    /* Get the mutex using pthread_mutex_lock() */
    rc = pthread_mutex_lock(&mutex);
    ICUNIT_ASSERT_EQUAL(rc, ENOERR, rc);

    /* Release the mutex using pthread_mutex_unlock() */
    rc = pthread_mutex_unlock(&mutex);
    ICUNIT_ASSERT_EQUAL(rc, ENOERR, rc);

    /* Get the mutex using pthread_mutex_trylock() */
    rc = pthread_mutex_trylock(&mutex);
    ICUNIT_ASSERT_EQUAL(rc, ENOERR, rc);

    /* Release the mutex using pthread_mutex_unlock() */
    rc = pthread_mutex_unlock(&mutex);
    ICUNIT_ASSERT_EQUAL(rc, ENOERR, rc);

    rc = pthread_mutex_destroy(&mutex);
    ICUNIT_GOTO_EQUAL(rc, ENOERR, rc, EXIT);

    return LOS_OK;

EXIT:
    pthread_mutex_destroy(&mutex);
    return LOS_OK;
}


VOID ItPosixMux039(void)
{
    TEST_ADD_CASE("ItPosixMux039", Testcase, TEST_POSIX, TEST_MUX, TEST_LEVEL2, TEST_FUNCTION);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */