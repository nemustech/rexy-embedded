/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                         EVENT FLAG  MANAGEMENT
*
*                            (c) Copyright 2001, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
* File : OS_FLAG.C
* By   : Jean J. Labrosse
*********************************************************************************************************
*/

#ifndef  OS_MASTER_FILE
#include "INCLUDES.H"
#endif

#if (OS_VERSION >= 251) && (OS_FLAG_EN > 0) && (OS_MAX_FLAGS > 0)
/*
*********************************************************************************************************
*                                            LOCAL PROTOTYPES
*********************************************************************************************************
*/

static  void     OS_FlagBlock(OS_FLAG_GRP *pgrp, OS_FLAG_NODE *pnode, OS_FLAGS flags, INT8U wait_type, INT16U timeout);
static  BOOLEAN  OS_FlagTaskRdy(OS_FLAG_NODE *pnode, OS_FLAGS flags_rdy);

/*$PAGE*/
/*
*********************************************************************************************************
*                              CHECK THE STATUS OF FLAGS IN AN EVENT FLAG GROUP
*
* Description: This function is called to check the status of a combination of bits to be set or cleared
*              in an event flag group.  Your application can check for ANY bit to be set/cleared or ALL 
*              bits to be set/cleared.
*
*              This call does not block if the desired flags are not present.
*
* Arguments  : pgrp          is a pointer to the desired event flag group.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to check.  
*                            The bits you want are specified by setting the corresponding bits in 
*                            'flags'.  e.g. if your application wants to wait for bits 0 and 1 then 
*                            'flags' would contain 0x03.
*
*              wait_type     specifies whether you want ALL bits to be set/cleared or ANY of the bits 
*                            to be set/cleared.
*                            You can specify the following argument:
*
*                            OS_FLAG_WAIT_CLR_ALL   You will check ALL bits in 'flags' to be clear (0)
*                            OS_FLAG_WAIT_CLR_ANY   You will check ANY bit  in 'flags' to be clear (0)
*                            OS_FLAG_WAIT_SET_ALL   You will check ALL bits in 'flags' to be set   (1)
*                            OS_FLAG_WAIT_SET_ANY   You will check ANY bit  in 'flags' to be set   (1)
*
*                            NOTE: Add OS_FLAG_CONSUME if you want the event flag to be 'consumed' by
*                                  the call.  Example, to wait for any flag in a group AND then clear
*                                  the flags that are present, set 'wait_type' to:
*
*                                  OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME
*
*              err           is a pointer to an error code and can be:
*                            OS_NO_ERR              No error
*                            OS_ERR_EVENT_TYPE      You are not pointing to an event flag group
*                            OS_FLAG_ERR_WAIT_TYPE  You didn't specify a proper 'wait_type' argument.
*                            OS_FLAG_INVALID_PGRP   You passed a NULL pointer instead of the event flag
*                                                   group handle.
*                            OS_FLAG_ERR_NOT_RDY    The desired flags you are waiting for are not 
*                                                   available.
*
* Returns    : The state of the flags in the event flag group.
*
* Called from: Task or ISR
*********************************************************************************************************
*/

#if OS_FLAG_ACCEPT_EN > 0
OS_FLAGS  OSFlagAccept (OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U wait_type, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR     cpu_sr;
#endif    
    OS_FLAGS      flags_cur;
    OS_FLAGS      flags_rdy;
    BOOLEAN       consume;


#if OS_ARG_CHK_EN > 0
    if (pgrp == (OS_FLAG_GRP *)0) {                        /* Validate 'pgrp'                          */
        *err = OS_FLAG_INVALID_PGRP;
        return ((OS_FLAGS)0);
    }
    if (pgrp->OSFlagType != OS_EVENT_TYPE_FLAG) {          /* Validate event block type                */
        *err = OS_ERR_EVENT_TYPE;
        return ((OS_FLAGS)0);
    }
#endif
    if (wait_type & OS_FLAG_CONSUME) {                     /* See if we need to consume the flags      */
        wait_type &= ~OS_FLAG_CONSUME;
        consume    = TRUE;
    } else {
        consume    = FALSE;
    }
    /*$PAGE*/
    *err = OS_NO_ERR;                                      /* Assume NO error until proven otherwise.  */
    OS_ENTER_CRITICAL();
    switch (wait_type) {
    case OS_FLAG_WAIT_SET_ALL:                         /* See if all required flags are set        */
        flags_rdy = pgrp->OSFlagFlags & flags;        /* Extract only the bits we want            */
        if (flags_rdy == flags) {                     /* Must match ALL the bits that we want     */
            if (consume == TRUE) {                    /* See if we need to consume the flags      */
                pgrp->OSFlagFlags &= ~flags_rdy;      /* Clear ONLY the flags that we wanted      */
            }
        } else {
            *err  = OS_FLAG_ERR_NOT_RDY;
        }
        flags_cur = pgrp->OSFlagFlags;                /* Will return the state of the group       */
        OS_EXIT_CRITICAL();
        break;

    case OS_FLAG_WAIT_SET_ANY:
        flags_rdy = pgrp->OSFlagFlags & flags;        /* Extract only the bits we want            */
        if (flags_rdy != (OS_FLAGS)0) {               /* See if any flag set                      */
            if (consume == TRUE) {                    /* See if we need to consume the flags      */
                pgrp->OSFlagFlags &= ~flags_rdy;      /* Clear ONLY the flags that we got         */
            }
        } else {
            *err  = OS_FLAG_ERR_NOT_RDY;
        }
        flags_cur = pgrp->OSFlagFlags;                /* Will return the state of the group       */
        OS_EXIT_CRITICAL();
        break;

#if OS_FLAG_WAIT_CLR_EN > 0
    case OS_FLAG_WAIT_CLR_ALL:                         /* See if all required flags are cleared    */
        flags_rdy = ~pgrp->OSFlagFlags & flags;       /* Extract only the bits we want            */
        if (flags_rdy == flags) {                     /* Must match ALL the bits that we want     */
            if (consume == TRUE) {                    /* See if we need to consume the flags      */
                pgrp->OSFlagFlags |= flags_rdy;       /* Set ONLY the flags that we wanted        */
            }
        } else {
            *err  = OS_FLAG_ERR_NOT_RDY;
        }
        flags_cur = pgrp->OSFlagFlags;                /* Will return the state of the group       */
        OS_EXIT_CRITICAL();
        break;

    case OS_FLAG_WAIT_CLR_ANY:
        flags_rdy = ~pgrp->OSFlagFlags & flags;       /* Extract only the bits we want            */
        if (flags_rdy != (OS_FLAGS)0) {               /* See if any flag cleared                  */
            if (consume == TRUE) {                    /* See if we need to consume the flags      */
                pgrp->OSFlagFlags |= flags_rdy;       /* Set ONLY the flags that we got           */
            }
        } else {
            *err  = OS_FLAG_ERR_NOT_RDY;
        }
        flags_cur = pgrp->OSFlagFlags;                /* Will return the state of the group       */
        OS_EXIT_CRITICAL();
        break;
#endif

    default:
        OS_EXIT_CRITICAL();
        flags_cur = (OS_FLAGS)0;
        *err      = OS_FLAG_ERR_WAIT_TYPE;
        break;
    }
    return (flags_cur);
}
#endif    

/*$PAGE*/
/*
*********************************************************************************************************
*                                           CREATE AN EVENT FLAG
*
* Description: This function is called to create an event flag group.
*
* Arguments  : flags         Contains the initial value to store in the event flag group.
*
*              err           is a pointer to an error code which will be returned to your application:
*                               OS_NO_ERR                if the call was successful.
*                               OS_ERR_CREATE_ISR        if you attempted to create an Event Flag from an 
*                                                        ISR.
*                               OS_FLAG_GRP_DEPLETED     if there are no more event flag groups
*
* Returns    : A pointer to an event flag group or a NULL pointer if no more groups are available.
*
* Called from: Task ONLY
*********************************************************************************************************
*/

OS_FLAG_GRP  *OSFlagCreate (OS_FLAGS flags, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                         /* Allocate storage for CPU status register        */
    OS_CPU_SR    cpu_sr;
#endif    
    OS_FLAG_GRP *pgrp;


    if (OSIntNesting > 0) {                         /* See if called from ISR ...                      */
        *err = OS_ERR_CREATE_ISR;                   /* ... can't CREATE from an ISR                    */
        return ((OS_FLAG_GRP *)0);
    }
    OS_ENTER_CRITICAL();
    pgrp = OSFlagFreeList;                          /* Get next free event flag                        */
    if (pgrp != (OS_FLAG_GRP *)0) {                 /* See if we have event flag groups available      */
        /* Adjust free list                                */
        OSFlagFreeList       = (OS_FLAG_GRP *)OSFlagFreeList->OSFlagWaitList;
        pgrp->OSFlagType     = OS_EVENT_TYPE_FLAG;  /* Set to event flag group type                    */
        pgrp->OSFlagFlags    = flags;               /* Set to desired initial value                    */
        pgrp->OSFlagWaitList = (void *)0;           /* Clear list of tasks waiting on flags            */
        OS_EXIT_CRITICAL();
        *err                 = OS_NO_ERR;
    } else {
        OS_EXIT_CRITICAL();
        *err                 = OS_FLAG_GRP_DEPLETED;
    }
    return (pgrp);                                  /* Return pointer to event flag group              */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                     DELETE AN EVENT FLAG GROUP
*
* Description: This function deletes an event flag group and readies all tasks pending on the event flag
*              group.
*
* Arguments  : pgrp          is a pointer to the desired event flag group.
*
*              opt           determines delete options as follows:
*                            opt == OS_DEL_NO_PEND   Deletes the event flag group ONLY if no task pending
*                            opt == OS_DEL_ALWAYS    Deletes the event flag group even if tasks are 
*                                                    waiting.  In this case, all the tasks pending will be 
*                                                    readied.
*
*              err           is a pointer to an error code that can contain one of the following values:
*                            OS_NO_ERR               The call was successful and the event flag group was 
*                                                    deleted
*                            OS_ERR_DEL_ISR          If you attempted to delete the event flag group from 
*                                                    an ISR
*                            OS_FLAG_INVALID_PGRP    If 'pgrp' is a NULL pointer.
*                            OS_ERR_EVENT_TYPE       You are not pointing to an event flag group
*                            OS_ERR_EVENT_TYPE       If you didn't pass a pointer to an event flag group
*                            OS_ERR_INVALID_OPT      An invalid option was specified
*                            OS_ERR_TASK_WAITING     One or more tasks were waiting on the event flag 
*                                                    group.
*
* Returns    : pevent        upon error
*              (OS_EVENT *)0 if the semaphore was successfully deleted.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of
*                 the event flag group MUST check the return code of OSFlagAccept() and OSFlagPend().
*              2) This call can potentially disable interrupts for a long time.  The interrupt disable
*                 time is directly proportional to the number of tasks waiting on the event flag group.
*********************************************************************************************************
*/

#if OS_FLAG_DEL_EN > 0
OS_FLAG_GRP  *OSFlagDel (OS_FLAG_GRP *pgrp, INT8U opt, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR     cpu_sr;
#endif    
    BOOLEAN       tasks_waiting;
    OS_FLAG_NODE *pnode;


    if (OSIntNesting > 0) {                                /* See if called from ISR ...               */
        *err = OS_ERR_DEL_ISR;                             /* ... can't DELETE from an ISR             */
        return (pgrp);
    }
#if OS_ARG_CHK_EN > 0
    if (pgrp == (OS_FLAG_GRP *)0) {                        /* Validate 'pgrp'                          */
        *err = OS_FLAG_INVALID_PGRP;
        return (pgrp);
    }
    if (pgrp->OSFlagType != OS_EVENT_TYPE_FLAG) {          /* Validate event group type                */
        *err = OS_ERR_EVENT_TYPE;
        return (pgrp);
    }
#endif
    OS_ENTER_CRITICAL();
    if (pgrp->OSFlagWaitList != (void *)0) {               /* See if any tasks waiting on event flags  */
        tasks_waiting = TRUE;                              /* Yes                                      */
    } else {
        tasks_waiting = FALSE;                             /* No                                       */
    }
    switch (opt) {
    case OS_DEL_NO_PEND:                               /* Delete group if no task waiting          */
        if (tasks_waiting == FALSE) {
            pgrp->OSFlagType     = OS_EVENT_TYPE_UNUSED;
            pgrp->OSFlagWaitList = (void *)OSFlagFreeList; /* Return group to free list           */
            OSFlagFreeList       = pgrp;
            OS_EXIT_CRITICAL();
            *err                 = OS_NO_ERR;
            return ((OS_FLAG_GRP *)0);                /* Event Flag Group has been deleted        */
        } else {
            OS_EXIT_CRITICAL();
            *err                 = OS_ERR_TASK_WAITING;
            return (pgrp);
        }

    case OS_DEL_ALWAYS:                                /* Always delete the event flag group       */
        pnode = pgrp->OSFlagWaitList;
        while (pnode != (OS_FLAG_NODE *)0) {          /* Ready ALL tasks waiting for flags        */
            OS_FlagTaskRdy(pnode, (OS_FLAGS)0);
            pnode = pnode->OSFlagNodeNext;
        }
        pgrp->OSFlagType     = OS_EVENT_TYPE_UNUSED;
        pgrp->OSFlagWaitList = (void *)OSFlagFreeList;/* Return group to free list                */
        OSFlagFreeList       = pgrp;
        OS_EXIT_CRITICAL();
        if (tasks_waiting == TRUE) {                  /* Reschedule only if task(s) were waiting  */
            OS_Sched();                               /* Find highest priority task ready to run  */
        }
        *err = OS_NO_ERR;
        return ((OS_FLAG_GRP *)0);                    /* Event Flag Group has been deleted        */

    default:
        OS_EXIT_CRITICAL();
        *err = OS_ERR_INVALID_OPT;
        return (pgrp);
    }
}
#endif
/*$PAGE*/
/*
*********************************************************************************************************
*                                        WAIT ON AN EVENT FLAG GROUP
*
* Description: This function is called to wait for a combination of bits to be set in an event flag 
*              group.  Your application can wait for ANY bit to be set or ALL bits to be set.
*
* Arguments  : pgrp          is a pointer to the desired event flag group.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to wait for.  
*                            The bits you want are specified by setting the corresponding bits in 
*                            'flags'.  e.g. if your application wants to wait for bits 0 and 1 then 
*                            'flags' would contain 0x03.
*
*              wait_type     specifies whether you want ALL bits to be set or ANY of the bits to be set.
*                            You can specify the following argument:
*
*                            OS_FLAG_WAIT_CLR_ALL   You will wait for ALL bits in 'mask' to be clear (0)
*                            OS_FLAG_WAIT_SET_ALL   You will wait for ALL bits in 'mask' to be set   (1)
*                            OS_FLAG_WAIT_CLR_ANY   You will wait for ANY bit  in 'mask' to be clear (0)
*                            OS_FLAG_WAIT_SET_ANY   You will wait for ANY bit  in 'mask' to be set   (1)
*
*                            NOTE: Add OS_FLAG_CONSUME if you want the event flag to be 'consumed' by
*                                  the call.  Example, to wait for any flag in a group AND then clear
*                                  the flags that are present, set 'wait_type' to:
*
*                                  OS_FLAG_WAIT_SET_ANY + OS_FLAG_CONSUME
*
*              timeout       is an optional timeout (in clock ticks) that your task will wait for the
*                            desired bit combination.  If you specify 0, however, your task will wait
*                            forever at the specified event flag group or, until a message arrives.
*
*              err           is a pointer to an error code and can be:
*                            OS_NO_ERR              The desired bits have been set within the specified
*                                                   'timeout'.
*                            OS_ERR_PEND_ISR        If you tried to PEND from an ISR
*                            OS_FLAG_INVALID_PGRP   If 'pgrp' is a NULL pointer.
*                            OS_ERR_EVENT_TYPE      You are not pointing to an event flag group
*                            OS_TIMEOUT             The bit(s) have not been set in the specified 
*                                                   'timeout'.
*                            OS_FLAG_ERR_WAIT_TYPE  You didn't specify a proper 'wait_type' argument.
*
* Returns    : The new state of the flags in the event flag group when the task is resumed or,
*              0 if a timeout or an error occurred.
*
* Called from: Task ONLY
*********************************************************************************************************
*/

OS_FLAGS  OSFlagPend (OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U wait_type, INT16U timeout, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR     cpu_sr;
#endif    
    OS_FLAG_NODE  node;
    OS_FLAGS      flags_cur;
    OS_FLAGS      flags_rdy;
    BOOLEAN       consume;


    if (OSIntNesting > 0) {                                /* See if called from ISR ...               */
        *err = OS_ERR_PEND_ISR;                            /* ... can't PEND from an ISR               */
        return ((OS_FLAGS)0);
    }
#if OS_ARG_CHK_EN > 0
    if (pgrp == (OS_FLAG_GRP *)0) {                        /* Validate 'pgrp'                          */
        *err = OS_FLAG_INVALID_PGRP;
        return ((OS_FLAGS)0);
    }
    if (pgrp->OSFlagType != OS_EVENT_TYPE_FLAG) {          /* Validate event block type                */
        *err = OS_ERR_EVENT_TYPE;
        return ((OS_FLAGS)0);
    }
#endif
    if (wait_type & OS_FLAG_CONSUME) {                     /* See if we need to consume the flags      */
        wait_type &= ~OS_FLAG_CONSUME;
        consume    = TRUE;
    } else {
        consume    = FALSE;
    }
    /*$PAGE*/
    OS_ENTER_CRITICAL();
    switch (wait_type) {
    case OS_FLAG_WAIT_SET_ALL:                         /* See if all required flags are set        */
        flags_rdy = pgrp->OSFlagFlags & flags;        /* Extract only the bits we want            */
        if (flags_rdy == flags) {                     /* Must match ALL the bits that we want     */
            if (consume == TRUE) {                    /* See if we need to consume the flags      */
                pgrp->OSFlagFlags &= ~flags_rdy;      /* Clear ONLY the flags that we wanted      */
            }
            flags_cur = pgrp->OSFlagFlags;            /* Will return the state of the group       */
            OS_EXIT_CRITICAL();                       /* Yes, condition met, return to caller     */
            *err      = OS_NO_ERR;
            return (flags_cur);
        } else {                                      /* Block task until events occur or timeout */
            OS_FlagBlock(pgrp, &node, flags, wait_type, timeout);
            OS_EXIT_CRITICAL();
        }
        break;

    case OS_FLAG_WAIT_SET_ANY:
        flags_rdy = pgrp->OSFlagFlags & flags;        /* Extract only the bits we want            */
        if (flags_rdy != (OS_FLAGS)0) {               /* See if any flag set                      */
            if (consume == TRUE) {                    /* See if we need to consume the flags      */
                pgrp->OSFlagFlags &= ~flags_rdy;      /* Clear ONLY the flags that we got         */
            }
            flags_cur = pgrp->OSFlagFlags;            /* Will return the state of the group       */
            OS_EXIT_CRITICAL();                       /* Yes, condition met, return to caller     */
            *err      = OS_NO_ERR;
            return (flags_cur);
        } else {                                      /* Block task until events occur or timeout */
            OS_FlagBlock(pgrp, &node, flags, wait_type, timeout);
            OS_EXIT_CRITICAL();
        }
        break;

#if OS_FLAG_WAIT_CLR_EN > 0
    case OS_FLAG_WAIT_CLR_ALL:                         /* See if all required flags are cleared    */
        flags_rdy = ~pgrp->OSFlagFlags & flags;       /* Extract only the bits we want            */
        if (flags_rdy == flags) {                     /* Must match ALL the bits that we want     */
            if (consume == TRUE) {                    /* See if we need to consume the flags      */
                pgrp->OSFlagFlags |= flags_rdy;       /* Set ONLY the flags that we wanted        */
            }
            flags_cur = pgrp->OSFlagFlags;            /* Will return the state of the group       */
            OS_EXIT_CRITICAL();                       /* Yes, condition met, return to caller     */
            *err      = OS_NO_ERR;
            return (flags_cur);
        } else {                                      /* Block task until events occur or timeout */
            OS_FlagBlock(pgrp, &node, flags, wait_type, timeout);
            OS_EXIT_CRITICAL();
        }
        break;

    case OS_FLAG_WAIT_CLR_ANY:
        flags_rdy = ~pgrp->OSFlagFlags & flags;       /* Extract only the bits we want            */
        if (flags_rdy != (OS_FLAGS)0) {               /* See if any flag cleared                  */
            if (consume == TRUE) {                    /* See if we need to consume the flags      */
                pgrp->OSFlagFlags |= flags_rdy;       /* Set ONLY the flags that we got           */
            }
            flags_cur = pgrp->OSFlagFlags;            /* Will return the state of the group       */
            OS_EXIT_CRITICAL();                       /* Yes, condition met, return to caller     */
            *err      = OS_NO_ERR;
            return (flags_cur);
        } else {                                      /* Block task until events occur or timeout */
            OS_FlagBlock(pgrp, &node, flags, wait_type, timeout);
            OS_EXIT_CRITICAL();
        }
        break;
#endif

    default:
        OS_EXIT_CRITICAL();
        flags_cur = (OS_FLAGS)0;
        *err      = OS_FLAG_ERR_WAIT_TYPE;
        return (flags_cur);
    }
    OS_Sched();                                            /* Find next HPT ready to run               */
    OS_ENTER_CRITICAL();
    if (OSTCBCur->OSTCBStat & OS_STAT_FLAG) {              /* Have we timed-out?                       */
        OS_FlagUnlink(&node);
        OSTCBCur->OSTCBStat = OS_STAT_RDY;                 /* Yes, make task ready-to-run              */
        OS_EXIT_CRITICAL();
        flags_cur           = (OS_FLAGS)0;
        *err                = OS_TIMEOUT;                  /* Indicate that we timed-out waiting       */
    } else {
        if (consume == TRUE) {                             /* See if we need to consume the flags      */
            switch (wait_type) {
            case OS_FLAG_WAIT_SET_ALL:
            case OS_FLAG_WAIT_SET_ANY:                 /* Clear ONLY the flags we got              */
                pgrp->OSFlagFlags &= ~OSTCBCur->OSTCBFlagsRdy;
                break;

#if OS_FLAG_WAIT_CLR_EN > 0
            case OS_FLAG_WAIT_CLR_ALL:
            case OS_FLAG_WAIT_CLR_ANY:                 /* Set   ONLY the flags we got              */
                pgrp->OSFlagFlags |= OSTCBCur->OSTCBFlagsRdy;
                break;
#endif
            }
        }
        flags_cur = pgrp->OSFlagFlags;
        OS_EXIT_CRITICAL();
        *err      = OS_NO_ERR;                             /* Event(s) must have occurred              */
    }
    return (flags_cur);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                         POST EVENT FLAG BIT(S)
*
* Description: This function is called to set or clear some bits in an event flag group.  The bits to 
*              set or clear are specified by a 'bit mask'.
*
* Arguments  : pgrp          is a pointer to the desired event flag group.
*
*              flags         If 'opt' (see below) is OS_FLAG_SET, each bit that is set in 'flags' will 
*                            set the corresponding bit in the event flag group.  e.g. to set bits 0, 4 
*                            and 5 you would set 'flags' to:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*                            If 'opt' (see below) is OS_FLAG_CLR, each bit that is set in 'flags' will 
*                            CLEAR the corresponding bit in the event flag group.  e.g. to clear bits 0, 
*                            4 and 5 you would specify 'flags' as:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*              opt           indicates whether the flags will be:
*                                set     (OS_FLAG_SET) or 
*                                cleared (OS_FLAG_CLR)
*
*              err           is a pointer to an error code and can be:
*                            OS_NO_ERR              The call was successfull
*                            OS_FLAG_INVALID_PGRP   You passed a NULL pointer
*                            OS_ERR_EVENT_TYPE      You are not pointing to an event flag group
*                            OS_FLAG_INVALID_OPT    You specified an invalid option
*
* Returns    : the new value of the event flags bits that are still set.
*
* Called From: Task or ISR
*
* WARNING(s) : 1) The execution time of this function depends on the number of tasks waiting on the event 
*                 flag group.
*              2) The amount of time interrupts are DISABLED depends on the number of tasks waiting on
*                 the event flag group.        
*********************************************************************************************************
*/
OS_FLAGS  OSFlagPost (OS_FLAG_GRP *pgrp, OS_FLAGS flags, INT8U opt, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                          /* Allocate storage for CPU status register       */
    OS_CPU_SR     cpu_sr;
#endif    
    OS_FLAG_NODE *pnode;
    BOOLEAN       sched;
    OS_FLAGS      flags_cur;
    OS_FLAGS      flags_rdy;


#if OS_ARG_CHK_EN > 0
    if (pgrp == (OS_FLAG_GRP *)0) {                  /* Validate 'pgrp'                                */
        *err = OS_FLAG_INVALID_PGRP;
        return ((OS_FLAGS)0);
    }
    if (pgrp->OSFlagType != OS_EVENT_TYPE_FLAG) {    /* Make sure we are pointing to an event flag grp */
        *err = OS_ERR_EVENT_TYPE;
        return ((OS_FLAGS)0);
    }
#endif
    /*$PAGE*/
    OS_ENTER_CRITICAL();
    switch (opt) {
    case OS_FLAG_CLR:
        pgrp->OSFlagFlags &= ~flags;            /* Clear the flags specified in the group         */
        break;

    case OS_FLAG_SET:
        pgrp->OSFlagFlags |=  flags;            /* Set   the flags specified in the group         */
        break;

    default:
        OS_EXIT_CRITICAL();                     /* INVALID option                                 */
        *err = OS_FLAG_INVALID_OPT;
        return ((OS_FLAGS)0);
    }
    sched = FALSE;                                   /* Indicate that we don't need rescheduling       */
    pnode = pgrp->OSFlagWaitList;
    while (pnode != (OS_FLAG_NODE *)0) {             /* Go through all tasks waiting on event flag(s)  */
        switch (pnode->OSFlagNodeWaitType) {
        case OS_FLAG_WAIT_SET_ALL:               /* See if all req. flags are set for current node */
            flags_rdy = pgrp->OSFlagFlags & pnode->OSFlagNodeFlags;
            if (flags_rdy == pnode->OSFlagNodeFlags) {
                if (OS_FlagTaskRdy(pnode, flags_rdy) == TRUE) { /* Make task RTR, event(s) Rx'd   */
                    sched = TRUE;                               /* When done we will reschedule   */
                }
            }
            break;

        case OS_FLAG_WAIT_SET_ANY:               /* See if any flag set                            */
            flags_rdy = pgrp->OSFlagFlags & pnode->OSFlagNodeFlags;
            if (flags_rdy != (OS_FLAGS)0) {
                if (OS_FlagTaskRdy(pnode, flags_rdy) == TRUE) { /* Make task RTR, event(s) Rx'd   */
                    sched = TRUE;                               /* When done we will reschedule   */
                }
            }
            break;

#if OS_FLAG_WAIT_CLR_EN > 0
        case OS_FLAG_WAIT_CLR_ALL:               /* See if all req. flags are set for current node */
            flags_rdy = ~pgrp->OSFlagFlags & pnode->OSFlagNodeFlags;
            if (flags_rdy == pnode->OSFlagNodeFlags) {
                if (OS_FlagTaskRdy(pnode, flags_rdy) == TRUE) { /* Make task RTR, event(s) Rx'd   */
                    sched = TRUE;                               /* When done we will reschedule   */
                }
            }
            break;

        case OS_FLAG_WAIT_CLR_ANY:               /* See if any flag set                            */
            flags_rdy = ~pgrp->OSFlagFlags & pnode->OSFlagNodeFlags;
            if (flags_rdy != (OS_FLAGS)0) {
                if (OS_FlagTaskRdy(pnode, flags_rdy) == TRUE) { /* Make task RTR, event(s) Rx'd   */
                    sched = TRUE;                               /* When done we will reschedule   */
                }
            }
            break;
#endif                 
        }
        pnode = pnode->OSFlagNodeNext;               /* Point to next task waiting for event flag(s)   */
    }
    OS_EXIT_CRITICAL();
    if (sched == TRUE) {
        OS_Sched();
    }
    OS_ENTER_CRITICAL();
    flags_cur = pgrp->OSFlagFlags;
    OS_EXIT_CRITICAL();
    *err      = OS_NO_ERR;
    return (flags_cur);
}
/*$PAGE*/
/*
*********************************************************************************************************
*                                           QUERY EVENT FLAG 
*
* Description: This function is used to check the value of the event flag group.
*
* Arguments  : pgrp         is a pointer to the desired event flag group.
*
*              err           is a pointer to an error code returned to the called:
*                            OS_NO_ERR              The call was successfull
*                            OS_FLAG_INVALID_PGRP   You passed a NULL pointer
*                            OS_ERR_EVENT_TYPE      You are not pointing to an event flag group
*
* Returns    : The current value of the event flag group.
*
* Called From: Task or ISR
*********************************************************************************************************
*/

#if OS_FLAG_QUERY_EN > 0
OS_FLAGS  OSFlagQuery (OS_FLAG_GRP *pgrp, INT8U *err)
{
#if OS_CRITICAL_METHOD == 3                       /* Allocate storage for CPU status register          */
    OS_CPU_SR  cpu_sr;
#endif    
    OS_FLAGS   flags;


#if OS_ARG_CHK_EN > 0
    if (pgrp == (OS_FLAG_GRP *)0) {               /* Validate 'pgrp'                                   */
        *err = OS_FLAG_INVALID_PGRP;
        return ((OS_FLAGS)0);
    }
    if (pgrp->OSFlagType != OS_EVENT_TYPE_FLAG) { /* Validate event block type                         */
        *err = OS_ERR_EVENT_TYPE;
        return ((OS_FLAGS)0);
    }
#endif    
    OS_ENTER_CRITICAL();
    flags = pgrp->OSFlagFlags;
    OS_EXIT_CRITICAL();
    *err = OS_NO_ERR;
    return (flags);                               /* Return the current value of the event flags       */
}
#endif

/*$PAGE*/
/*
*********************************************************************************************************
*                         SUSPEND TASK UNTIL EVENT FLAG(s) RECEIVED OR TIMEOUT OCCURS
*
* Description: This function is internal to uC/OS-II and is used to put a task to sleep until the desired
*              event flag bit(s) are set.
*
* Arguments  : pgrp          is a pointer to the desired event flag group.
*
*              pnode         is a pointer to a structure which contains data about the task waiting for 
*                            event flag bit(s) to be set.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to check.  
*                            The bits you want are specified by setting the corresponding bits in 
*                            'flags'.  e.g. if your application wants to wait for bits 0 and 1 then 
*                            'flags' would contain 0x03.
*
*              wait_type     specifies whether you want ALL bits to be set/cleared or ANY of the bits 
*                            to be set/cleared.
*                            You can specify the following argument:
*
*                            OS_FLAG_WAIT_CLR_ALL   You will check ALL bits in 'mask' to be clear (0)
*                            OS_FLAG_WAIT_CLR_ANY   You will check ANY bit  in 'mask' to be clear (0)
*                            OS_FLAG_WAIT_SET_ALL   You will check ALL bits in 'mask' to be set   (1)
*                            OS_FLAG_WAIT_SET_ANY   You will check ANY bit  in 'mask' to be set   (1)
*
*              timeout       is the desired amount of time that the task will wait for the event flag 
*                            bit(s) to be set.
*
* Returns    : none
*
* Called by  : OSFlagPend()  OS_FLAG.C
*
* Note(s)    : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

static  void  OS_FlagBlock (OS_FLAG_GRP *pgrp, OS_FLAG_NODE *pnode, OS_FLAGS flags, INT8U wait_type, INT16U timeout)
{
    OS_FLAG_NODE  *pnode_next;


    OSTCBCur->OSTCBStat      |= OS_STAT_FLAG;
    OSTCBCur->OSTCBDly        = timeout;              /* Store timeout in task's TCB                   */
#if OS_TASK_DEL_EN > 0
    OSTCBCur->OSTCBFlagNode   = pnode;                /* TCB to link to node                           */
#endif    
    pnode->OSFlagNodeFlags    = flags;                /* Save the flags that we need to wait for       */
    pnode->OSFlagNodeWaitType = wait_type;            /* Save the type of wait we are doing            */
    pnode->OSFlagNodeTCB      = (void *)OSTCBCur;     /* Link to task's TCB                            */
    pnode->OSFlagNodeNext     = pgrp->OSFlagWaitList; /* Add node at beginning of event flag wait list */
    pnode->OSFlagNodePrev     = (void *)0;
    pnode->OSFlagNodeFlagGrp  = (void *)pgrp;         /* Link to Event Flag Group                      */
    pnode_next                = pgrp->OSFlagWaitList;
    if (pnode_next != (void *)0) {                    /* Is this the first NODE to insert?             */
        pnode_next->OSFlagNodePrev = pnode;           /* No, link in doubly linked list                */
    }
    pgrp->OSFlagWaitList = (void *)pnode;
    /* Suspend current task until flag(s) received   */
    if ((OSRdyTbl[OSTCBCur->OSTCBY] &= ~OSTCBCur->OSTCBBitX) == 0) {
        OSRdyGrp &= ~OSTCBCur->OSTCBBitY;
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                    INITIALIZE THE EVENT FLAG MODULE
*
* Description: This function is called by uC/OS-II to initialize the event flag module.  Your application
*              MUST NOT call this function.  In other words, this function is internal to uC/OS-II.
*
* Arguments  : none
*
* Returns    : none
*
* WARNING    : You MUST NOT call this function from your code.  This is an INTERNAL function to uC/OS-II.
*********************************************************************************************************
*/

void  OS_FlagInit (void)
{
#if OS_MAX_FLAGS == 1
    OSFlagFreeList                 = (OS_FLAG_GRP *)&OSFlagTbl[0];  /* Only ONE event flag group!      */
    OSFlagFreeList->OSFlagType     = OS_EVENT_TYPE_UNUSED;
    OSFlagFreeList->OSFlagWaitList = (void *)0;
#endif

#if OS_MAX_FLAGS >= 2
    INT8U        i;
    OS_FLAG_GRP *pgrp1;
    OS_FLAG_GRP *pgrp2;


    pgrp1 = &OSFlagTbl[0];
    pgrp2 = &OSFlagTbl[1];
    for (i = 0; i < (OS_MAX_FLAGS - 1); i++) {                      /* Init. list of free EVENT FLAGS  */
        pgrp1->OSFlagType     = OS_EVENT_TYPE_UNUSED;
        pgrp1->OSFlagWaitList = (void *)pgrp2;
        pgrp1++;
        pgrp2++;
    }
    pgrp1->OSFlagWaitList = (void *)0;
    OSFlagFreeList        = (OS_FLAG_GRP *)&OSFlagTbl[0];
#endif    
}

/*$PAGE*/
/*
*********************************************************************************************************
*                              MAKE TASK READY-TO-RUN, EVENT(s) OCCURRED
*
* Description: This function is internal to uC/OS-II and is used to make a task ready-to-run because the
*              desired event flag bits have been set.
*
* Arguments  : pnode         is a pointer to a structure which contains data about the task waiting for 
*                            event flag bit(s) to be set.
*
*              flags_rdy     contains the bit pattern of the event flags that cause the task to become
*                            ready-to-run.
*
* Returns    : none
*
* Called by  : OSFlagsPost() OS_FLAG.C
*
* Note(s)    : 1) This function assumes that interrupts are disabled.
*              2) This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

static  BOOLEAN  OS_FlagTaskRdy (OS_FLAG_NODE *pnode, OS_FLAGS flags_rdy)
{
    OS_TCB   *ptcb;
    BOOLEAN   sched;


    ptcb                = (OS_TCB *)pnode->OSFlagNodeTCB;  /* Point to TCB of waiting task             */
    ptcb->OSTCBDly      = 0;
    ptcb->OSTCBFlagsRdy = flags_rdy;
    ptcb->OSTCBStat    &= ~OS_STAT_FLAG;
    if (ptcb->OSTCBStat == OS_STAT_RDY) {                  /* Put task into ready list                 */
        OSRdyGrp               |= ptcb->OSTCBBitY;
        OSRdyTbl[ptcb->OSTCBY] |= ptcb->OSTCBBitX;
        sched                   = TRUE;
    } else {
        sched                   = FALSE;
    }
    OS_FlagUnlink(pnode);
    return (sched);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                  UNLINK EVENT FLAG NODE FROM WAITING LIST
*
* Description: This function is internal to uC/OS-II and is used to unlink an event flag node from a
*              list of tasks waiting for the event flag.
*
* Arguments  : pnode         is a pointer to a structure which contains data about the task waiting for 
*                            event flag bit(s) to be set.
*
* Returns    : none
*
* Called by  : OS_FlagTaskRdy() OS_FLAG.C
*              OSFlagPend()     OS_FLAG.C
*              OSTaskDel()      OS_TASK.C
*
* Note(s)    : 1) This function assumes that interrupts are disabled.
*              2) This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

void  OS_FlagUnlink (OS_FLAG_NODE *pnode)
{
    OS_TCB       *ptcb;
    OS_FLAG_GRP  *pgrp;
    OS_FLAG_NODE *pnode_prev;
    OS_FLAG_NODE *pnode_next;


    pnode_prev = pnode->OSFlagNodePrev;
    pnode_next = pnode->OSFlagNodeNext;
    if (pnode_prev == (OS_FLAG_NODE *)0) {                      /* Is it first node in wait list?      */
        pgrp                 = pnode->OSFlagNodeFlagGrp;        /* Yes, Point to event flag group      */
        pgrp->OSFlagWaitList = (void *)pnode_next;              /*      Update list for new 1st node   */
        if (pnode_next != (OS_FLAG_NODE *)0) {
            pnode_next->OSFlagNodePrev = (OS_FLAG_NODE *)0;     /*      Link new 1st node PREV to NULL */
        }
    } else {                                                    /* No,  A node somewhere in the list   */
        pnode_prev->OSFlagNodeNext = pnode_next;                /*      Link around the node to unlink */
        if (pnode_next != (OS_FLAG_NODE *)0) {                  /*      Was this the LAST node?        */
            pnode_next->OSFlagNodePrev = pnode_prev;            /*      No, Link around current node   */
        }
    }
    ptcb                = (OS_TCB *)pnode->OSFlagNodeTCB;
#if OS_TASK_DEL_EN > 0
    ptcb->OSTCBFlagNode = (void *)0;
#endif
}
#endif
