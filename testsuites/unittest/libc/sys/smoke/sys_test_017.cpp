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
#include "It_test_sys.h"

const int password_length = 10;

static UINT32 TestCase(VOID)
{
    char *passwd1 = NULL;
    char *passwd2 = NULL;
    char *key = NULL;
    char slat[2];
    int ret;
    char test_password[password_length];
    srand(time(NULL));
    for (int i = 0, r = 0; i < password_length; i++) {
        r = rand() % 36; // 36: 0-9 and a-z
        if (r < 10) {    // 10: 0-9
            test_password[i] = '0' + r;
        } else {
            test_password[i] = 'a' + r;
        }
    }

    key = test_password;
    slat[0] = key[0];
    slat[1] = key[1];
    passwd1 = crypt(key, slat);
    ICUNIT_GOTO_NOT_EQUAL(passwd1, NULL, passwd1, EXIT);

    key = test_password;
    slat[0] = passwd1[0];
    slat[1] = passwd1[1];
    passwd2 = crypt(key, slat);
    ICUNIT_GOTO_NOT_EQUAL(passwd2, NULL, passwd2, EXIT);

    ret = strcmp(passwd1, passwd2);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

    return 0;
EXIT:
    return -1;
}

VOID ItTestSys017(VOID)
{
    TEST_ADD_CASE(__FUNCTION__, TestCase, TEST_POSIX, TEST_MEM, TEST_LEVEL0, TEST_FUNCTION);
}
