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

#define BUF_SIZE 256

static UINT32 TestCase(VOID)
{
    char ebuff[BUF_SIZE];
    int ret;
    int cflags;
    regex_t reg;
    char *a = NULL;
    char *b = const_cast<char *>("No error");
    char *testStr = const_cast<char *>("Hello World");
    char *regStr = const_cast<char *>("H.*");

    cflags = REG_EXTENDED | REG_ICASE | REG_NOSUB;

    ret = regcomp(&reg, regStr, cflags);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

    ret = regexec(&reg, testStr, 0, NULL, 0);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

    regerror(ret, &reg, ebuff, BUF_SIZE);
    a = ebuff;
    ret = strcmp(a, b);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

    regfree(&reg);

    return 0;
EXIT:
    regerror(ret, &reg, ebuff, BUF_SIZE);
    fprintf(stderr, "%s\n", ebuff);
    regfree(&reg);

    return -1;
}


VOID ItTestSys008(VOID)
{
    TEST_ADD_CASE(__FUNCTION__, TestCase, TEST_POSIX, TEST_MEM, TEST_LEVEL0, TEST_FUNCTION);
}
