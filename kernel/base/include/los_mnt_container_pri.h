/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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

#ifndef _LOS_MNT_CONTAINER_PRI_H
#define _LOS_MNT_CONTAINER_PRI_H

#include "fs/mount.h"
#include "sched.h"
#include "los_atomic.h"
#include "vnode.h"
#include "stdlib.h"

#ifdef LOSCFG_MNT_CONTAINER
typedef struct ProcessCB LosProcessCB;
struct Container;

typedef struct MntContainer {
    Atomic rc;
    UINT32 containerID;
    LIST_HEAD mountList;
} MntContainer;

LIST_HEAD *GetContainerMntList(VOID);

UINT32 OsInitRootMntContainer(MntContainer **mntContainer);

UINT32 OsCopyMntContainer(UINTPTR flags, LosProcessCB *child, LosProcessCB *parent);

UINT32 OsUnshareMntContainer(UINTPTR flags, LosProcessCB *curr, struct Container *newContainer);

UINT32 OsSetNsMntContainer(UINT32 flags, struct Container *container, struct Container *newContainer);

VOID OsMntContainerDestroy(struct Container *container);

UINT32 OsGetMntContainerID(MntContainer *mntContainer);

UINT32 OsGetMntContainerCount(VOID);
#endif
#endif
