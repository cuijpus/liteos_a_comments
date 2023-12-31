/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2023 Huawei Device Co., Ltd. All rights reserved.
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

#ifndef _LOS_PROCESS_PRI_H
#define _LOS_PROCESS_PRI_H

#include "los_task_pri.h"
#include "sched.h"
#include "los_sem_pri.h"
#include "los_process.h"
#include "los_vm_map.h"
#ifdef LOSCFG_KERNEL_LITEIPC
#include "hm_liteipc.h"
#endif
#ifdef LOSCFG_SECURITY_CAPABILITY
#include "capability_type.h"
#endif
#ifdef LOSCFG_SECURITY_VID
#include "vid_type.h"
#endif
#include "sys/resource.h"
#ifdef LOSCFG_KERNEL_CONTAINER
#include "los_container_pri.h"
#endif
#ifdef LOSCFG_KERNEL_PLIMITS
#include "los_plimits.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define OS_PCB_NAME_LEN OS_TCB_NAME_LEN

#ifdef LOSCFG_SECURITY_CAPABILITY
#define OS_GROUPS_NUMBER_MAX 256

typedef struct {
    UINT32  userID;
    UINT32  effUserID;
    UINT32  gid;
    UINT32  effGid;
    UINT32  groupNumber;
    UINT32  groups[1];
} User;
#endif

typedef struct ProcessGroup {
    UINTPTR      pgroupLeader;    /**< Process group leader is the the process that created the group */
    LOS_DL_LIST  processList;     /**< List of processes under this process group */
    LOS_DL_LIST  exitProcessList; /**< List of closed processes (zombie processes) under this group */
    LOS_DL_LIST  groupList;       /**< Process group list */
} ProcessGroup;

typedef struct ProcessCB {
    CHAR                 processName[OS_PCB_NAME_LEN]; /**< Process name */
    UINT32               processID;                    /**< Process ID */
    UINT16               processStatus;                /**< [15:4] Process Status; [3:0] The number of threads currently
                                                            running in the process */
    UINT16               consoleID;                    /**< The console id of task belongs  */
    UINT16               processMode;                  /**< Kernel Mode:0; User Mode:1; */
    struct ProcessCB     *parentProcess;               /**< Parent process */
    UINT32               exitCode;                     /**< Process exit status */
    LOS_DL_LIST          pendList;                     /**< Block list to which the process belongs */
    LOS_DL_LIST          childrenList;                 /**< Children process list */
    LOS_DL_LIST          exitChildList;                /**< Exit children process list */
    LOS_DL_LIST          siblingList;                  /**< Linkage in parent's children list */
    ProcessGroup         *pgroup;                      /**< Process group to which a process belongs */
    LOS_DL_LIST          subordinateGroupList;         /**< Linkage in group list */
    LosTaskCB            *threadGroup;
    LOS_DL_LIST          threadSiblingList;            /**< List of threads under this process */
    volatile UINT32      threadNumber; /**< Number of threads alive under this process */
    UINT32               threadCount;  /**< Total number of threads created under this process */
    LOS_DL_LIST          waitList;     /**< The process holds the waitLits to support wait/waitpid */
#ifdef LOSCFG_KERNEL_SMP
    UINT32               timerCpu;     /**< CPU core number of this task is delayed or pended */
#endif
    UINTPTR              sigHandler;   /**< Signal handler */
    sigset_t             sigShare;     /**< Signal share bit */
#ifdef LOSCFG_KERNEL_LITEIPC
    ProcIpcInfo          *ipcInfo;      /**< Memory pool for lite ipc */
#endif
#ifdef LOSCFG_KERNEL_VM
    LosVmSpace           *vmSpace;     /**< VMM space for processes */
#endif
#ifdef LOSCFG_FS_VFS
    struct files_struct  *files;       /**< Files held by the process */
#endif
    timer_t              timerID;      /**< ITimer */

#ifdef LOSCFG_SECURITY_CAPABILITY
    User                *user;
    UINT32              capability;
#endif
#ifdef LOSCFG_SECURITY_VID
    TimerIdMap           timerIdMap;
#endif
#ifdef LOSCFG_DRIVERS_TZDRIVER
    struct Vnode        *execVnode;   /**< Exec bin of the process */
#endif
    mode_t               umask;
#ifdef LOSCFG_KERNEL_CPUP
    OsCpupBase           *processCpup; /**< Process cpu usage */
#endif
    struct rlimit        *resourceLimit;
#ifdef LOSCFG_KERNEL_CONTAINER
    Container            *container;
#ifdef LOSCFG_USER_CONTAINER
    struct Credentials   *credentials;
#endif
#endif
#ifdef LOSCFG_PROC_PROCESS_DIR
    struct ProcDirEntry *procDir;
#endif
#ifdef LOSCFG_KERNEL_PLIMITS
    ProcLimiterSet *plimits;
    LOS_DL_LIST    plimitsList;  /* plimit process list */
    PLimitsData    limitStat;
#endif
} LosProcessCB;

extern LosProcessCB *g_processCBArray;
extern UINT32 g_processMaxNum;

#define OS_PCB_FROM_RPID(processID)     (((LosProcessCB *)g_processCBArray) + (processID))
#ifdef LOSCFG_PID_CONTAINER
#define OS_PCB_FROM_PID(processID)      OsGetPCBFromVpid(processID)
#else
#define OS_PCB_FROM_PID(processID)      OS_PCB_FROM_RPID(processID)
#endif
#define OS_PCB_FROM_TCB(taskCB)         ((LosProcessCB *)((taskCB)->processCB))
#define OS_PCB_FROM_TID(taskID)         ((LosProcessCB *)(OS_TCB_FROM_TID(taskID)->processCB))
#define OS_GET_PGROUP_LEADER(pgroup)    ((LosProcessCB *)((pgroup)->pgroupLeader))
#define OS_PCB_FROM_SIBLIST(ptr)        LOS_DL_LIST_ENTRY((ptr), LosProcessCB, siblingList)
#define OS_PCB_FROM_PENDLIST(ptr)       LOS_DL_LIST_ENTRY((ptr), LosProcessCB, pendList)

/**
 * @ingroup los_process
 * Flag that indicates the process or process control block status.
 *
 * The process is created but does not participate in scheduling.
 */
#define OS_PROCESS_STATUS_INIT           OS_TASK_STATUS_INIT

/**
 * @ingroup los_process
 * Flag that indicates the process or process control block status.
 *
 * The process is ready.
 */
#define OS_PROCESS_STATUS_READY          OS_TASK_STATUS_READY

/**
 * @ingroup los_process
 * Flag that indicates the process or process control block status.
 *
 * The process is running.
 */
#define OS_PROCESS_STATUS_RUNNING        OS_TASK_STATUS_RUNNING

/**
 * @ingroup los_process
 * Flag that indicates the process or process control block status.
 *
 * The process is pending
 */
#define OS_PROCESS_STATUS_PENDING       (OS_TASK_STATUS_PENDING | OS_TASK_STATUS_DELAY | OS_TASK_STATUS_SUSPENDED)

/**
 * @ingroup los_process
 * Flag that indicates the process or process control block status.
 *
 * The process is run out but the resources occupied by the process are not recovered.
 */
#define OS_PROCESS_STATUS_ZOMBIES        0x0100U

/**
 * @ingroup los_process
 * Flag that indicates the process or process control block status.
 *
 * The process status equal this is process control block unused,
 * coexisting with OS_PROCESS_STATUS_ZOMBIES means that the control block is not recovered.
 */
#define OS_PROCESS_FLAG_UNUSED            0x0200U

/**
 * @ingroup los_process
 * Flag that indicates the process or process control block status.
 *
 * The process has been call exit, it only works with multiple cores.
 */
#define OS_PROCESS_FLAG_EXIT              0x0400U

/**
 * @ingroup los_process
 * Flag that indicates the process or process control block status.
 *
 * The process is the leader of the process group.
 */
#define OS_PROCESS_FLAG_GROUP_LEADER      0x0800U

/**
 * @ingroup los_process
 * Flag that indicates the process or process control block status.
 *
 * The process has performed the exec operation.
 */
#define OS_PROCESS_FLAG_ALREADY_EXEC      0x1000U

/**
 * @ingroup los_process
 * Flag that indicates the process or process control block status.
 *
 * The process is dying or already dying.
 */
#define OS_PROCESS_STATUS_INACTIVE       (OS_PROCESS_FLAG_EXIT | OS_PROCESS_STATUS_ZOMBIES)

/**
 * @ingroup los_process
 * Used to check if the process control block is unused.
 */
STATIC INLINE BOOL OsProcessIsUnused(const LosProcessCB *processCB)
{
    return ((processCB->processStatus & OS_PROCESS_FLAG_UNUSED) != 0);
}

/**
 * @ingroup los_process
 * Used to check if the process is inactive.
 */
STATIC INLINE BOOL OsProcessIsInactive(const LosProcessCB *processCB)
{
    return ((processCB->processStatus & (OS_PROCESS_FLAG_UNUSED | OS_PROCESS_STATUS_INACTIVE)) != 0);
}

/**
 * @ingroup los_process
 * Used to check if the process is dead.
 */
STATIC INLINE BOOL OsProcessIsDead(const LosProcessCB *processCB)
{
    return ((processCB->processStatus & OS_PROCESS_STATUS_ZOMBIES) != 0);
}

STATIC INLINE BOOL OsProcessIsInit(const LosProcessCB *processCB)
{
    return ((processCB->processStatus & OS_PROCESS_STATUS_INIT) != 0);
}

STATIC INLINE BOOL OsProcessIsPGroupLeader(const LosProcessCB *processCB)
{
    return ((processCB->processStatus & OS_PROCESS_FLAG_GROUP_LEADER) != 0);
}

/**
 * @ingroup los_process
 * The highest priority of a kernel mode process.
 */
#define OS_PROCESS_PRIORITY_HIGHEST      0

/**
 * @ingroup los_process
 * The lowest priority of a kernel mode process
 */
#define OS_PROCESS_PRIORITY_LOWEST       31

/**
 * @ingroup los_process
 * The highest priority of a user mode process.
 */
#define OS_USER_PROCESS_PRIORITY_HIGHEST 10

/**
 * @ingroup los_process
 * The lowest priority of a user mode process
 */
#define OS_USER_PROCESS_PRIORITY_LOWEST  OS_PROCESS_PRIORITY_LOWEST

/**
 * @ingroup los_process
 * User state root process default priority
 */
#define OS_PROCESS_USERINIT_PRIORITY     28

/**
 * @ingroup los_process
 * ID of the kernel idle process
 */
#define OS_KERNEL_IDLE_PROCESS_ID       0U

/**
 * @ingroup los_process
 * ID of the user root process
 */
#define OS_USER_ROOT_PROCESS_ID         1U

/**
 * @ingroup los_process
 * ID of the kernel root process
 */
#define OS_KERNEL_ROOT_PROCESS_ID       2U

#define OS_TASK_DEFAULT_STACK_SIZE      0x2000
#define OS_USER_TASK_SYSCALL_STACK_SIZE 0x3000
#define OS_USER_TASK_STACK_SIZE         0x100000

#define OS_KERNEL_MODE 0x0U
#define OS_USER_MODE   0x1U
STATIC INLINE BOOL OsProcessIsUserMode(const LosProcessCB *processCB)
{
    return (processCB->processMode == OS_USER_MODE);
}

#define LOS_PRIO_PROCESS  0U
#define LOS_PRIO_PGRP     1U
#define LOS_PRIO_USER     2U

#define OS_USER_PRIVILEGE_PROCESS_GROUP ((UINTPTR)OsGetUserInitProcess())
#define OS_KERNEL_PROCESS_GROUP         ((UINTPTR)OsGetKernelInitProcess())

/*
 * Process exit code
 * 31    15           8           7        0
 * |     | exit code  | core dump | signal |
 */
#define OS_PRO_EXIT_OK 0

STATIC INLINE VOID OsProcessExitCodeCoreDumpSet(LosProcessCB *processCB)
{
    processCB->exitCode |= 0x80U;
}

STATIC INLINE VOID OsProcessExitCodeSignalSet(LosProcessCB *processCB, UINT32 signal)
{
    processCB->exitCode |= signal & 0x7FU;
}

STATIC INLINE VOID OsProcessExitCodeSignalClear(LosProcessCB *processCB)
{
    processCB->exitCode &= (~0x7FU);
}

STATIC INLINE BOOL OsProcessExitCodeSignalIsSet(LosProcessCB *processCB)
{
    return (processCB->exitCode) & 0x7FU;
}

STATIC INLINE VOID OsProcessExitCodeSet(LosProcessCB *processCB, UINT32 code)
{
    processCB->exitCode |= ((code & 0x000000FFU) << 8U) & 0x0000FF00U; /* 8: Move 8 bits to the left, exitCode */
}

#define OS_PID_CHECK_INVALID(pid) (((UINT32)(pid)) >= g_processMaxNum)

STATIC INLINE BOOL OsProcessIDUserCheckInvalid(UINT32 pid)
{
    return ((pid >= g_processMaxNum) || (pid == 0));
}

STATIC INLINE LosProcessCB *OsCurrProcessGet(VOID)
{
    UINT32 intSave;

    intSave = LOS_IntLock();
    LosProcessCB *runProcess = OS_PCB_FROM_TCB(OsCurrTaskGet());
    LOS_IntRestore(intSave);
    return runProcess;
}

#ifdef LOSCFG_SECURITY_CAPABILITY
STATIC INLINE User *OsCurrUserGet(VOID)
{
    User *user = NULL;
    UINT32 intSave;

    intSave = LOS_IntLock();
    user = OsCurrProcessGet()->user;
    LOS_IntRestore(intSave);
    return user;
}

STATIC INLINE UINT32 OsProcessUserIDGet(const LosTaskCB *taskCB)
{
    UINT32 intSave = LOS_IntLock();
    UINT32 uid = OS_INVALID;

    LosProcessCB *process = OS_PCB_FROM_TCB(taskCB);
    if (process->user != NULL) {
        uid = process->user->userID;
    }
    LOS_IntRestore(intSave);
    return uid;
}
#endif

STATIC INLINE BOOL OsIsProcessThreadGroup(const LosTaskCB *taskCB)
{
    return (OS_PCB_FROM_TCB(taskCB)->threadGroup == taskCB);
}

STATIC INLINE UINT32 OsProcessThreadNumberGet(const LosTaskCB *taskCB)
{
    return OS_PCB_FROM_TCB(taskCB)->threadNumber;
}

#ifdef LOSCFG_KERNEL_VM
STATIC INLINE LosVmSpace *OsProcessVmSpaceGet(const LosProcessCB *processCB)
{
    return processCB->vmSpace;
}
#endif

#ifdef LOSCFG_DRIVERS_TZDRIVER
STATIC INLINE struct Vnode *OsProcessExecVnodeGet(const LosProcessCB *processCB)
{
    return processCB->execVnode;
}
#endif

STATIC INLINE UINT32 OsGetPid(const LosProcessCB *processCB)
{
#ifdef LOSCFG_PID_CONTAINER
    if (OS_PROCESS_CONTAINER_CHECK(processCB, OsCurrProcessGet())) {
        return OsGetVpidFromCurrContainer(processCB);
    }
#endif
    return processCB->processID;
}

STATIC INLINE UINT32 OsGetRootPid(const LosProcessCB *processCB)
{
#ifdef LOSCFG_PID_CONTAINER
    return OsGetVpidFromRootContainer(processCB);
#else
    return processCB->processID;
#endif
}

/*
 * return immediately if no child has exited.
 */
#define LOS_WAIT_WNOHANG   (1 << 0U)

/*
 * return if a child has stopped (but not traced via ptrace(2)).
 * Status for traced children which have stopped is provided even
 * if this option is not specified.
 */
#define LOS_WAIT_WUNTRACED (1 << 1U)
#define LOS_WAIT_WSTOPPED (1 << 1U)

/*
 * Wait for exited processes
 */
#define LOS_WAIT_WEXITED (1 << 2U)

/*
 * return if a stopped child has been resumed by delivery of SIGCONT.
 * (For Linux-only options, see below.)
 */
#define LOS_WAIT_WCONTINUED (1 << 3U)

/*
 * Leave the child in a waitable state;
 * a later wait call can be used to again retrieve the child status information.
 */
#define LOS_WAIT_WNOWAIT (1 << 24U)

/*
 * Indicates that you are already in a wait state
 */
#define OS_PROCESS_WAIT     (1 << 15U)

/*
 * Wait for any child process to finish
 */
#define OS_PROCESS_WAIT_ANY OS_TASK_WAIT_ANYPROCESS

/*
 * Wait for the child process specified by the pid to finish
 */
#define OS_PROCESS_WAIT_PRO OS_TASK_WAIT_PROCESS

/*
 * Waits for any child process in the specified process group to finish.
 */
#define OS_PROCESS_WAIT_GID OS_TASK_WAIT_GID

#define OS_PROCESS_INFO_ALL 1
#define OS_PROCESS_DEFAULT_UMASK 0022

extern UINTPTR __user_init_entry;
extern UINTPTR __user_init_bss;
extern UINTPTR __user_init_end;
extern UINTPTR __user_init_load_addr;
extern UINT32 OsProcessInit(VOID);
extern UINT32 OsSystemProcessCreate(VOID);
extern VOID OsProcessNaturalExit(LosProcessCB *processCB, UINT32 status);
extern VOID OsProcessCBRecycleToFree(VOID);
extern VOID OsProcessResourcesToFree(LosProcessCB *processCB);
extern UINT32 OsUserInitProcess(VOID);
extern INT32 OsClone(UINT32 flags, UINTPTR sp, UINT32 size);
extern VOID OsExecProcessVmSpaceRestore(LosVmSpace *oldSpace);
extern LosVmSpace *OsExecProcessVmSpaceReplace(LosVmSpace *newSpace, UINTPTR stackBase, INT32 randomDevFD);
extern UINT32 OsExecRecycleAndInit(LosProcessCB *processCB, const CHAR *name, LosVmSpace *oldAspace, UINTPTR oldFiles);
extern UINT32 OsExecStart(const TSK_ENTRY_FUNC entry, UINTPTR sp, UINTPTR mapBase, UINT32 mapSize);
extern UINT32 OsSetProcessName(LosProcessCB *processCB, const CHAR *name);
extern INT32 OsSetProcessScheduler(INT32 which, INT32 pid, UINT16 policy, const LosSchedParam *param);
extern INT32 OsGetProcessPriority(INT32 which, INT32 pid);
extern LosProcessCB *OsGetUserInitProcess(VOID);
extern LosProcessCB *OsGetIdleProcess(VOID);
extern INT32 OsSetProcessGroupID(UINT32 pid, UINT32 gid);
extern INT32 OsSetCurrProcessGroupID(UINT32 gid);
extern LosProcessCB *OsGetKernelInitProcess(VOID);
extern VOID OsSetSigHandler(UINTPTR addr);
extern UINTPTR OsGetSigHandler(VOID);
extern VOID OsWaitWakeTask(LosTaskCB *taskCB, UINT32 wakePID);
extern INT32 OsSendSignalToProcessGroup(INT32 pid, siginfo_t *info, INT32 permission);
extern INT32 OsSendSignalToAllProcess(siginfo_t *info, INT32 permission);
extern UINT32 OsProcessAddNewTask(UINTPTR processID, LosTaskCB *taskCB, SchedParam *param, UINT32 *numCount);
extern VOID OsDeleteTaskFromProcess(LosTaskCB *taskCB);
extern VOID OsProcessThreadGroupDestroy(VOID);
extern UINT32 OsGetProcessGroupCB(UINT32 pid, UINTPTR *ppgroupLeader);
extern LosProcessCB *OsGetDefaultProcessCB(VOID);
extern ProcessGroup *OsCreateProcessGroup(LosProcessCB *processCB);
INT32 OsSchedulerParamCheck(UINT16 policy, BOOL isThread, const LosSchedParam *param);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
