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


static UINT32 Testcase(VOID)
{
    char ch;
    unsigned int nPos;
    int ret;
    char pathname[50]; // 50, enough space.
    char *filename = "/crt_getc_unlocked_test1";
    FILE *testFile = NULL;

    ret = strncpy_s(pathname, sizeof(pathname), g_ioTestPath, sizeof(pathname));
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    ret = strcat_s(pathname, 50, filename); // 50, path name buffer size
    ICUNIT_ASSERT_EQUAL(ret, EOK, ret);

    testFile = fopen(pathname, "w+");
    ICUNIT_ASSERT_NOT_EQUAL(testFile, NULL, LOS_NOK);
    ret = fputs("0123456789abcdefghijklmnopqrstuvwxyz", testFile);
    ICUNIT_GOTO_NOT_EQUAL(ret, EOF, ret, EXIT);
    fclose(testFile);

    testFile = fopen(pathname, "r");
    ICUNIT_GOTO_NOT_EQUAL(testFile, NULL, testFile, EXIT);

    nPos = ftell(testFile);

    ch = getc_unlocked(testFile);
    ICUNIT_GOTO_EQUAL(ch, '0', ch, EXIT);

    if ((nPos + sizeof(char)) != (unsigned int)ftell(testFile)) {
        ICUNIT_GOTO_EQUAL(1, 0, 1, EXIT);
    }

    fclose(testFile);
    remove(pathname);
    return LOS_OK;
EXIT:
    if (testFile != NULL) {
        fclose(testFile);
        remove(pathname);
    }
    return LOS_NOK;
}

VOID ItStdioGetcUnlocked001(void)
{
    TEST_ADD_CASE(__FUNCTION__, Testcase, TEST_LIB, TEST_LIBC, TEST_LEVEL1, TEST_FUNCTION);
}
