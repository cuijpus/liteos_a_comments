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
#include "los_time_container_pri.h"
#include "los_process_pri.h"

#ifdef LOSCFG_TIME_CONTAINER
STATIC UINT32 g_currentTimeContainerNum;

STATIC TimeContainer *CreateNewTimeContainer(TimeContainer *parent)
{
    UINT32 size = sizeof(TimeContainer);
    TimeContainer *timeContainer = (TimeContainer *)LOS_MemAlloc(m_aucSysMem1, size);
    if (timeContainer == NULL) {
        return NULL;
    }
    (VOID)memset_s(timeContainer, size, 0, size);

    timeContainer->containerID = OsAllocContainerID();
    if (parent != NULL) {
        timeContainer->frozenOffsets = FALSE;
        LOS_AtomicSet(&timeContainer->rc, 1);
    } else {
        timeContainer->frozenOffsets = TRUE;
        LOS_AtomicSet(&timeContainer->rc, 3); /* 3: Three system processes */
    }
    return timeContainer;
}

STATIC UINT32 CreateTimeContainer(LosProcessCB *child, LosProcessCB *parent)
{
    UINT32 intSave;
    TimeContainer *parentContainer = parent->container->timeContainer;
    if (parentContainer == parent->container->timeForChildContainer) {
        TimeContainer *newTimeContainer = CreateNewTimeContainer(parentContainer);
        if (newTimeContainer == NULL) {
            return ENOMEM;
        }

        SCHEDULER_LOCK(intSave);
        g_currentTimeContainerNum++;
        (VOID)memcpy_s(&newTimeContainer->monotonic, sizeof(struct timespec64),
                       &parentContainer->monotonic, sizeof(struct timespec64));
        child->container->timeContainer = newTimeContainer;
        child->container->timeForChildContainer = newTimeContainer;
        SCHEDULER_UNLOCK(intSave);
        return LOS_OK;
    }

    SCHEDULER_LOCK(intSave);
    TimeContainer *newTimeContainer = parent->container->timeForChildContainer;
    g_currentTimeContainerNum++;
    LOS_AtomicSet(&newTimeContainer->rc, 1);
    if (!newTimeContainer->frozenOffsets) {
        newTimeContainer->frozenOffsets = TRUE;
    }
    child->container->timeContainer = newTimeContainer;
    child->container->timeForChildContainer = newTimeContainer;
    SCHEDULER_UNLOCK(intSave);
    return LOS_OK;
}

UINT32 OsInitRootTimeContainer(TimeContainer **timeContainer)
{
    UINT32 intSave;
    TimeContainer *newTimeContainer = CreateNewTimeContainer(NULL);
    if (newTimeContainer == NULL) {
        return ENOMEM;
    }

    SCHEDULER_LOCK(intSave);
    *timeContainer = newTimeContainer;
    g_currentTimeContainerNum++;
    SCHEDULER_UNLOCK(intSave);
    return LOS_OK;
}

UINT32 OsCopyTimeContainer(UINTPTR flags, LosProcessCB *child, LosProcessCB *parent)
{
    UINT32 intSave;
    TimeContainer *currTimeContainer = parent->container->timeContainer;

    if (!(flags & CLONE_NEWTIME) && (currTimeContainer == parent->container->timeForChildContainer)) {
        SCHEDULER_LOCK(intSave);
        LOS_AtomicInc(&currTimeContainer->rc);
        child->container->timeContainer = currTimeContainer;
        child->container->timeForChildContainer = currTimeContainer;
        SCHEDULER_UNLOCK(intSave);
        return LOS_OK;
    }

    return CreateTimeContainer(child, parent);
}

UINT32 OsUnshareTimeContainer(UINTPTR flags, LosProcessCB *curr, Container *newContainer)
{
    UINT32 intSave;
    if (!(flags & CLONE_NEWTIME)) {
        SCHEDULER_LOCK(intSave);
        newContainer->timeContainer = curr->container->timeContainer;
        newContainer->timeForChildContainer = curr->container->timeForChildContainer;
        SCHEDULER_UNLOCK(intSave);
        return LOS_OK;
    }

    TimeContainer *timeForChild = CreateNewTimeContainer(curr->container->timeContainer);
    if (timeForChild == NULL) {
        return ENOMEM;
    }

    SCHEDULER_LOCK(intSave);
    if (curr->container->timeContainer != curr->container->timeForChildContainer) {
        SCHEDULER_UNLOCK(intSave);
        (VOID)LOS_MemFree(m_aucSysMem1, timeForChild);
        return EINVAL;
    }

    newContainer->timeContainer = curr->container->timeContainer;
    newContainer->timeForChildContainer = timeForChild;
    LOS_AtomicSet(&timeForChild->rc, 0);
    SCHEDULER_UNLOCK(intSave);
    return LOS_OK;
}

VOID OsTimeContainersDestroy(LosProcessCB *curr)
{
    UINT32 intSave;
    if (curr->container == NULL) {
        return;
    }

    SCHEDULER_LOCK(intSave);
    TimeContainer *timeContainer = curr->container->timeContainer;
    if (timeContainer != NULL) {
        LOS_AtomicDec(&timeContainer->rc);
        if (LOS_AtomicRead(&timeContainer->rc) <= 0) {
            g_currentTimeContainerNum--;
            curr->container->timeContainer = NULL;
            SCHEDULER_UNLOCK(intSave);
            if ((timeContainer != curr->container->timeForChildContainer) &&
                (LOS_AtomicRead(&curr->container->timeForChildContainer->rc) <= 0)) {
                (VOID)LOS_MemFree(m_aucSysMem1, curr->container->timeForChildContainer);
            }
            curr->container->timeForChildContainer = NULL;
            (VOID)LOS_MemFree(m_aucSysMem1, timeContainer);
            return;
        }
    }
    SCHEDULER_UNLOCK(intSave);
    return;
}

UINT32 OsGetTimeContainerID(TimeContainer *timeContainer)
{
    if (timeContainer == NULL) {
        return OS_INVALID_VALUE;
    }

    return timeContainer->containerID;
}

TimeContainer *OsGetCurrTimeContainer(VOID)
{
    return OsCurrProcessGet()->container->timeContainer;
}

UINT32 OsGetTimeContainerMonotonic(LosProcessCB *processCB, struct timespec64 *offsets)
{
    if ((processCB == NULL) || (offsets == NULL)) {
        return EINVAL;
    }

    if (OsProcessIsInactive(processCB)) {
        return ESRCH;
    }

    TimeContainer *timeContainer = processCB->container->timeForChildContainer;
    (VOID)memcpy_s(offsets, sizeof(struct timespec64), &timeContainer->monotonic, sizeof(struct timespec64));
    return LOS_OK;
}

UINT32 OsSetTimeContainerMonotonic(LosProcessCB *processCB, struct timespec64 *offsets)
{
    if ((processCB == NULL) || (offsets == NULL)) {
        return EINVAL;
    }

    if (OsProcessIsInactive(processCB)) {
        return ESRCH;
    }

    TimeContainer *timeContainer = processCB->container->timeForChildContainer;
    if (timeContainer->frozenOffsets) {
        return EACCES;
    }

    timeContainer->monotonic.tv_sec = offsets->tv_sec;
    timeContainer->monotonic.tv_nsec = offsets->tv_nsec;
    return LOS_OK;
}
#endif