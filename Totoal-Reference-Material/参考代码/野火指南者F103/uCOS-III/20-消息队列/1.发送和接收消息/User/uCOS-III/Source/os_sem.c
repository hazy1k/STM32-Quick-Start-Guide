/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                 SEMAPHORE MANAGEMENT
*
* File    : OS_SEM.C
* By      : JJL
* Version : V3.03.01
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#define  MICRIUM_SOURCE
#include <os.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_sem__c = "$Id: $";
#endif


#if OS_CFG_SEM_EN > 0u
/*
************************************************************************************************************************
*                                                  CREATE A SEMAPHORE
*
* Description: This function creates a semaphore.
*
* Arguments  : p_sem         is a pointer to the semaphore to initialize.  Your application is responsible for
*                            allocating storage for the semaphore.
*
*              p_name        is a pointer to the name you would like to give the semaphore.
*
*              cnt           is the initial value for the semaphore.
*                            If used to share resources, you should initialize to the number of resources available.
*                            If used to signal the occurrence of event(s) then you should initialize to 0.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                    if the call was successful
*                                OS_ERR_CREATE_ISR              if you called this function from an ISR
*                                OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the semaphore after you
*                                                                 called OSSafetyCriticalStart().
*                                OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                OS_ERR_OBJ_CREATED             if the semaphore has already been created
*                                OS_ERR_OBJ_PTR_NULL            if 'p_sem'  is a NULL pointer
*                                OS_ERR_OBJ_TYPE                if 'p_sem' has already been initialized to a different
*                                                               object type
*
* Returns    : none
************************************************************************************************************************
*/

void  OSSemCreate (OS_SEM      *p_sem,  //多值信号量控制块指针
                   CPU_CHAR    *p_name, //多值信号量名称
                   OS_SEM_CTR   cnt,    //资源数目或事件是否发生标志
                   OS_ERR      *p_err)  //返回错误类型
{
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和定义一个局部变
                    //量，用于保存关中断前的 CPU 状态寄存器 SR（临界段关中断只需保存SR）
                    //，开中断时将该值还原。 
	
#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return;                         //返回，不继续执行
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508                //如果使能（默认禁用）了安全关键
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {  //如果是在调用 OSSafetyCriticalStart() 后创建该多值信号量
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;   //错误类型为“非法创建内核对象”
        return;                                   //返回，不继续执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u            //如果使能（默认使能）了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {    //如果该函数是在中断中被调用
       *p_err = OS_ERR_CREATE_ISR;                //错误类型为“在中断函数中定时”
        return;                                   //返回，不继续执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u            //如果使能（默认使能）了参数检测
    if (p_sem == (OS_SEM *)0) {       //如果参数 p_sem 为空  
       *p_err = OS_ERR_OBJ_PTR_NULL;  //错误类型为“多值信号量对象为空”
        return;                       //返回，不继续执行
    }
#endif

    OS_CRITICAL_ENTER();               //进入临界段
    p_sem->Type    = OS_OBJ_TYPE_SEM;  //初始化多值信号量指标  
    p_sem->Ctr     = cnt;                                 
    p_sem->TS      = (CPU_TS)0;
    p_sem->NamePtr = p_name;                               
    OS_PendListInit(&p_sem->PendList); //初始化该多值信号量的等待列表     

#if OS_CFG_DBG_EN > 0u       //如果使能（默认使能）了调试代码和变量 
    OS_SemDbgListAdd(p_sem); //将该信号量添加到多值信号量双向调试链表
#endif
    OSSemQty++;              //多值信号量个数加1

    OS_CRITICAL_EXIT_NO_SCHED();     //退出临界段（无调度）
   *p_err = OS_ERR_NONE;             //错误类型为“无错误”
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                  DELETE A SEMAPHORE
*
* Description: This function deletes a semaphore.
*
* Arguments  : p_sem         is a pointer to the semaphore to delete
*
*              opt           determines delete options as follows:
*
*                                OS_OPT_DEL_NO_PEND          Delete semaphore ONLY if no task pending
*                                OS_OPT_DEL_ALWAYS           Deletes the semaphore even if tasks are waiting.
*                                                            In this case, all the tasks pending will be readied.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                 The call was successful and the semaphore was deleted
*                                OS_ERR_DEL_ISR              If you attempted to delete the semaphore from an ISR
*                                OS_ERR_OBJ_PTR_NULL         If 'p_sem' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE             If 'p_sem' is not pointing at a semaphore
*                                OS_ERR_OPT_INVALID          An invalid option was specified
*                                OS_ERR_TASK_WAITING         One or more tasks were waiting on the semaphore
*
* Returns    : == 0          if no tasks were waiting on the semaphore, or upon error.
*              >  0          if one or more tasks waiting on the semaphore are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the semaphore
*                 MUST check the return code of OSSemPend().
*              2) OSSemAccept() callers will not know that the intended semaphore has been deleted.
*              3) Because ALL tasks pending on the semaphore will be readied, you MUST be careful in applications where
*                 the semaphore is used for mutual exclusion because the resource(s) will no longer be guarded by the
*                 semaphore.
************************************************************************************************************************
*/

#if OS_CFG_SEM_DEL_EN > 0u             //如果使能了 OSSemDel() 函数 
OS_OBJ_QTY  OSSemDel (OS_SEM  *p_sem,  //多值信号量指针
                      OS_OPT   opt,    //选项
                      OS_ERR  *p_err)  //返回错误类型
{
    OS_OBJ_QTY     cnt;
    OS_OBJ_QTY     nbr_tasks;
    OS_PEND_DATA  *p_pend_data;
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL                      //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {                //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();        //执行安全检测异常函数
        return ((OS_OBJ_QTY)0);                //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
       *p_err = OS_ERR_DEL_ISR;                //返回错误类型为“在中断中删除”
        return ((OS_OBJ_QTY)0);                //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                    //如果使能了参数检测
    if (p_sem == (OS_SEM *)0) {               //如果 p_sem 为空
       *p_err = OS_ERR_OBJ_PTR_NULL;          //返回错误类型为“内核对象为空”
        return ((OS_OBJ_QTY)0);               //返回0（有错误），不继续执行
    }
    switch (opt) {                            //根据选项分类处理
        case OS_OPT_DEL_NO_PEND:              //如果选项在预期之内
        case OS_OPT_DEL_ALWAYS:
             break;                           //直接跳出

        default:                              //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;      //返回错误类型为“选项非法”
             return ((OS_OBJ_QTY)0);          //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u              //如果使能了对象类型检测
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {    //如果 p_sem 不是多值信号量类型
       *p_err = OS_ERR_OBJ_TYPE;             //返回错误类型为“内核对象类型错误”
        return ((OS_OBJ_QTY)0);              //返回0（有错误），不继续执行
    }
#endif

    CPU_CRITICAL_ENTER();                      //关中断
    p_pend_list = &p_sem->PendList;            //获取信号量的等待列表到 p_pend_list
    cnt         = p_pend_list->NbrEntries;     //获取等待该信号量的任务数
    nbr_tasks   = cnt;
    switch (opt) {                             //根据选项分类处理
        case OS_OPT_DEL_NO_PEND:               //如果只在没有任务等待的情况下删除信号量
             if (nbr_tasks == (OS_OBJ_QTY)0) { //如果没有任务在等待该信号量
#if OS_CFG_DBG_EN > 0u                         //如果使能了调试代码和变量   
                 OS_SemDbgListRemove(p_sem);   //将该信号量从信号量调试列表移除
#endif
                 OSSemQty--;                   //信号量数目减1
                 OS_SemClr(p_sem);             //清除信号量内容
                 CPU_CRITICAL_EXIT();          //开中断
                *p_err = OS_ERR_NONE;          //返回错误类型为“无错误”
             } else {                          //如果有任务在等待该信号量
                 CPU_CRITICAL_EXIT();          //开中断
                *p_err = OS_ERR_TASK_WAITING;  //返回错误类型为“有任务在等待该信号量”
             }
             break;

        case OS_OPT_DEL_ALWAYS:                             //如果必须删除信号量
             OS_CRITICAL_ENTER_CPU_EXIT();                  //锁调度器，并开中断
             ts = OS_TS_GET();                              //获取时间戳
             while (cnt > 0u) {                             //逐个移除该信号量等待列表中的任务
                 p_pend_data = p_pend_list->HeadPtr;
                 p_tcb       = p_pend_data->TCBPtr;
                 OS_PendObjDel((OS_PEND_OBJ *)((void *)p_sem),
                               p_tcb,
                               ts);
                 cnt--;
             }
#if OS_CFG_DBG_EN > 0u                                      //如果使能了调试代码和变量 
             OS_SemDbgListRemove(p_sem);                    //将该信号量从信号量调试列表移除
#endif
             OSSemQty--;                                    //信号量数目减1
             OS_SemClr(p_sem);                              //清除信号量内容
             OS_CRITICAL_EXIT_NO_SCHED();                   //减锁调度器，但不进行调度
             OSSched();                                     //任务调度，执行最高优先级的就绪任务
            *p_err = OS_ERR_NONE;                           //返回错误类型为“无错误”
             break;

        default:                                            //如果选项超出预期
             CPU_CRITICAL_EXIT();                           //开中断
            *p_err = OS_ERR_OPT_INVALID;                    //返回错误类型为“选项非法”
             break;
    }
    return ((OS_OBJ_QTY)nbr_tasks);                         //返回删除信号量前等待其的任务数
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                  PEND ON SEMAPHORE
*
* Description: This function waits for a semaphore.
*
* Arguments  : p_sem         is a pointer to the semaphore
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for the
*                            resource up to the amount of time (in 'ticks') specified by this argument.  If you specify
*                            0, however, your task will wait forever at the specified semaphore or, until the resource
*                            becomes available (or the event occurs).
*
*              opt           determines whether the user wants to block if the semaphore is not available or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the semaphore was posted
*                            or pend aborted or the semaphore deleted.  If you pass a NULL pointer (i.e. (CPU_TS*)0)
*                            then you will not get the timestamp.  In other words, passing a NULL pointer is valid
*                            and indicates that you don't need the timestamp.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE               The call was successful and your task owns the resource
*                                                          or, the event you are waiting for occurred.
*                                OS_ERR_OBJ_DEL            If 'p_sem' was deleted
*                                OS_ERR_OBJ_PTR_NULL       If 'p_sem' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE           If 'p_sem' is not pointing at a semaphore
*                                OS_ERR_OPT_INVALID        If you specified an invalid value for 'opt'
*                                OS_ERR_PEND_ABORT         If the pend was aborted by another task
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                                          would lead to a suspension.
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the semaphore was not
*                                                          available.
*                                OS_ERR_SCHED_LOCKED       If you called this function when the scheduler is locked
*                                OS_ERR_STATUS_INVALID     Pend status is invalid
*                                OS_ERR_TIMEOUT            The semaphore was not received within the specified
*                                                          timeout.
*
*
* Returns    : The current value of the semaphore counter or 0 if not available.
************************************************************************************************************************
*/

OS_SEM_CTR  OSSemPend (OS_SEM   *p_sem,   //多值信号量指针
                       OS_TICK   timeout, //等待超时时间
                       OS_OPT    opt,     //选项
                       CPU_TS   *p_ts,    //等到信号量时的时间戳
                       OS_ERR   *p_err)   //返回错误类型
{
    OS_SEM_CTR    ctr;
    OS_PEND_DATA  pend_data;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL                       //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {                 //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();         //执行安全检测异常函数
        return ((OS_SEM_CTR)0);                 //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //如果该函数在中断中被调用
       *p_err = OS_ERR_PEND_ISR;                //返回错误类型为“在中断中等待”
        return ((OS_SEM_CTR)0);                 //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                     //如果使能了参数检测
    if (p_sem == (OS_SEM *)0) {                //如果 p_sem 为空
       *p_err = OS_ERR_OBJ_PTR_NULL;           //返回错误类型为“内核对象为空”
        return ((OS_SEM_CTR)0);                //返回0（有错误），不继续执行
    }
    switch (opt) {                             //根据选项分类处理
        case OS_OPT_PEND_BLOCKING:             //如果选择“等待不到对象进行堵塞”
        case OS_OPT_PEND_NON_BLOCKING:         //如果选择“等待不到对象不进行堵塞”
             break;                            //直接跳出，不处理

        default:                               //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;       //返回错误类型为“选项非法”
             return ((OS_SEM_CTR)0);           //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u               //如果使能了对象类型检测
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {     //如果 p_sem 不是多值信号量类型
       *p_err = OS_ERR_OBJ_TYPE;              //返回错误类型为“内核对象类型错误”
        return ((OS_SEM_CTR)0);               //返回0（有错误），不继续执行
    }
#endif

    if (p_ts != (CPU_TS *)0) {                //如果 p_ts 非空
       *p_ts  = (CPU_TS)0;                    //初始化（清零）p_ts，待用于返回时间戳
    }
    CPU_CRITICAL_ENTER();                     //关中断
    if (p_sem->Ctr > (OS_SEM_CTR)0) {         //如果资源可用
        p_sem->Ctr--;                         //资源数目减1
        if (p_ts != (CPU_TS *)0) {            //如果 p_ts 非空
           *p_ts  = p_sem->TS;                //获取该信号量最后一次发布的时间戳
        }
        ctr   = p_sem->Ctr;                   //获取信号量的当前资源数目
        CPU_CRITICAL_EXIT();                  //开中断
       *p_err = OS_ERR_NONE;                  //返回错误类型为“无错误”
        return (ctr);                         //返回信号量的当前资源数目，不继续执行
    }

    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {    //如果没有资源可用，而且选择了不堵塞任务
        ctr   = p_sem->Ctr;                                 //获取信号量的资源数目到 ctr
        CPU_CRITICAL_EXIT();                                //开中断
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                    //返回错误类型为“等待渴求堵塞”  
        return (ctr);                                       //返回信号量的当前资源数目，不继续执行
    } else {                                                //如果没有资源可用，但选择了堵塞任务
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {    //如果调度器被锁
            CPU_CRITICAL_EXIT();                            //开中断
           *p_err = OS_ERR_SCHED_LOCKED;                    //返回错误类型为“调度器被锁”
            return ((OS_SEM_CTR)0);                         //返回0（有错误），不继续执行
        }
    }
                                                            
    OS_CRITICAL_ENTER_CPU_EXIT();                           //锁调度器，并重开中断
    OS_Pend(&pend_data,                                     //堵塞等待任务，将当前任务脱离就绪列表，
            (OS_PEND_OBJ *)((void *)p_sem),                 //并插入到节拍列表和等待列表。
            OS_TASK_PEND_ON_SEM,
            timeout);

    OS_CRITICAL_EXIT_NO_SCHED();                            //开调度器，但不进行调度

    OSSched();                                              //找到并调度最高优先级就绪任务
    /* 当前任务（获得信号量）得以继续运行 */
    CPU_CRITICAL_ENTER();                                   //关中断
    switch (OSTCBCurPtr->PendStatus) {                      //根据当前运行任务的等待状态分类处理
        case OS_STATUS_PEND_OK:                             //如果等待状态正常
             if (p_ts != (CPU_TS *)0) {                     //如果 p_ts 非空
                *p_ts  =  OSTCBCurPtr->TS;                  //获取信号被发布的时间戳
             }
            *p_err = OS_ERR_NONE;                           //返回错误类型为“无错误”
             break;

        case OS_STATUS_PEND_ABORT:                          //如果等待被终止中止
             if (p_ts != (CPU_TS *)0) {                     //如果 p_ts 非空
                *p_ts  =  OSTCBCurPtr->TS;                  //获取等待被中止的时间戳
             }
            *p_err = OS_ERR_PEND_ABORT;                     //返回错误类型为“等待被中止”
             break;

        case OS_STATUS_PEND_TIMEOUT:                        //如果等待超时
             if (p_ts != (CPU_TS *)0) {                     //如果 p_ts 非空
                *p_ts  = (CPU_TS  )0;                       //清零 p_ts
             }
            *p_err = OS_ERR_TIMEOUT;                        //返回错误类型为“等待超时”
             break;

        case OS_STATUS_PEND_DEL:                            //如果等待的内核对象被删除
             if (p_ts != (CPU_TS *)0) {                     //如果 p_ts 非空
                *p_ts  =  OSTCBCurPtr->TS;                  //获取内核对象被删除的时间戳
             }
            *p_err = OS_ERR_OBJ_DEL;                        //返回错误类型为“等待对象被删除”
             break;

        default:                                            //如果等待状态超出预期
            *p_err = OS_ERR_STATUS_INVALID;                 //返回错误类型为“等待状态非法”
             CPU_CRITICAL_EXIT();                           //开中断
             return ((OS_SEM_CTR)0);                        //返回0（有错误），不继续执行
    }
    ctr = p_sem->Ctr;                                       //获取信号量的当前资源数目
    CPU_CRITICAL_EXIT();                                    //开中断
    return (ctr);                                           //返回信号量的当前资源数目
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                             ABORT WAITING ON A SEMAPHORE
*
* Description: This function aborts & readies any tasks currently waiting on a semaphore.  This function should be used
*              to fault-abort the wait on the semaphore, rather than to normally signal the semaphore via OSSemPost().
*
* Arguments  : p_sem     is a pointer to the semaphore
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the semaphore
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the semaphore
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the semaphore was readied and
*                                                         informed of the aborted wait; check return value for the
*                                                         number of tasks whose wait on the semaphore was aborted.
*                            OS_ERR_OBJ_PTR_NULL          If 'p_sem' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If 'p_sem' is not pointing at a semaphore
*                            OS_ERR_OPT_INVALID           If you specified an invalid option
*                            OS_ERR_PEND_ABORT_ISR        If you called this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the semaphore, or upon error.
*              >  0          if one or more tasks waiting on the semaphore are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_SEM_PEND_ABORT_EN > 0u           //如果使能了 OSSemPendAbort() 函数
OS_OBJ_QTY  OSSemPendAbort (OS_SEM  *p_sem, //多值信号量指针
                            OS_OPT   opt,   //选项
                            OS_ERR  *p_err) //返回错误类型
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    OS_OBJ_QTY     nbr_tasks;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return ((OS_OBJ_QTY)0u);        //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) { //如果该函数在中断中被调用
       *p_err =  OS_ERR_PEND_ABORT_ISR;         //返回错误类型为“在中断中中止等待”
        return ((OS_OBJ_QTY)0u);                //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                      //如果使能了参数检测
    if (p_sem == (OS_SEM *)0) {                 //如果 p_sem 为空
       *p_err =  OS_ERR_OBJ_PTR_NULL;           //返回错误类型为“内核对象为空”
        return ((OS_OBJ_QTY)0u);                //返回0（有错误），不继续执行
    }
    switch (opt) {                                          //根据选项分类处理
        case OS_OPT_PEND_ABORT_1:                           //如果选项在预期内
        case OS_OPT_PEND_ABORT_ALL:
        case OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED:
             break;                                         //不处理，直接跳出

        default:                                            //如果选项超出预期
            *p_err =  OS_ERR_OPT_INVALID;                   //返回错误类型为“选项非法”
             return ((OS_OBJ_QTY)0u);                       //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u             //如果使能了对象类型检测
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {   //如果 p_sem 不是多值信号量类型
       *p_err =  OS_ERR_OBJ_TYPE;           //返回错误类型为“内核对象类型错误”
        return ((OS_OBJ_QTY)0u);            //返回0（有错误），不继续执行
    }
#endif

    CPU_CRITICAL_ENTER();                             //关中断
    p_pend_list = &p_sem->PendList;                   //获取 p_sem 的等待列表到 p_pend_list
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0u) {  //如果没有任务在等待 p_sem
        CPU_CRITICAL_EXIT();                          //开中断
       *p_err =  OS_ERR_PEND_ABORT_NONE;              //返回错误类型为“没有任务在等待”
        return ((OS_OBJ_QTY)0u);                      //返回0（有错误），不继续执行
    }

    OS_CRITICAL_ENTER_CPU_EXIT();                      //加锁调度器，并开中断
    nbr_tasks = 0u;
    ts        = OS_TS_GET();                           //获取时间戳
    while (p_pend_list->NbrEntries > (OS_OBJ_QTY)0u) { //如果有任务在等待 p_sem
        p_tcb = p_pend_list->HeadPtr->TCBPtr;          //获取优先级最高的等待任务
        OS_PendAbort((OS_PEND_OBJ *)((void *)p_sem),   //中止该任务对 p_sem 的等待
                     p_tcb,
                     ts);
        nbr_tasks++;
        if (opt != OS_OPT_PEND_ABORT_ALL) {            //如果不是选择了中止所有等待任务
            break;                                     //立即跳出，不再继续中止
        }
    }
    OS_CRITICAL_EXIT_NO_SCHED();                       //减锁调度器，但不调度

    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0u) {  //如果选择了任务调度
        OSSched();                                     //进行任务调度
    }

   *p_err = OS_ERR_NONE;   //返回错误类型为“无错误”
    return (nbr_tasks);    //返回被中止的任务数目
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : p_sem    is a pointer to the semaphore
*
*              opt      determines the type of POST performed:
*
*                           OS_OPT_POST_1            POST and ready only the highest priority task waiting on semaphore
*                                                    (if tasks are waiting).
*                           OS_OPT_POST_ALL          POST to ALL tasks that are waiting on the semaphore
*
*                           OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*                           Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*
*              p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE          The call was successful and the semaphore was signaled.
*                           OS_ERR_OBJ_PTR_NULL  If 'p_sem' is a NULL pointer.
*                           OS_ERR_OBJ_TYPE      If 'p_sem' is not pointing at a semaphore
*                           OS_ERR_SEM_OVF       If the post would cause the semaphore count to overflow.
*
* Returns    : The current value of the semaphore counter or 0 upon error.
************************************************************************************************************************
*/

OS_SEM_CTR  OSSemPost (OS_SEM  *p_sem,    //多值信号量控制块指针
                       OS_OPT   opt,      //选项
                       OS_ERR  *p_err)    //返回错误类型
{
    OS_SEM_CTR  ctr;
    CPU_TS      ts;



#ifdef OS_SAFETY_CRITICAL                 //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {           //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();   //执行安全检测异常函数
        return ((OS_SEM_CTR)0);           //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //如果使能（默认使能）了参数检测功能   
    if (p_sem == (OS_SEM *)0) {           //如果 p_sem 为空
       *p_err  = OS_ERR_OBJ_PTR_NULL;     //返回错误类型为“内核对象指针为空”
        return ((OS_SEM_CTR)0);           //返回0（有错误），不继续执行
    }
    switch (opt) {                                   //根据选项情况分类处理
        case OS_OPT_POST_1:                          //如果选项在预期内，不处理
        case OS_OPT_POST_ALL:
        case OS_OPT_POST_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
             break;

        default:                                     //如果选项超出预期
            *p_err =  OS_ERR_OPT_INVALID;            //返回错误类型为“选项非法”
             return ((OS_SEM_CTR)0u);                //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u            //如果使能了对象类型检测           
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {  //如果 p_sem 的类型不是多值信号量类型
       *p_err = OS_ERR_OBJ_TYPE;           //返回错误类型为“对象类型错误”
        return ((OS_SEM_CTR)0);            //返回0（有错误），不继续执行
    }
#endif

    ts = OS_TS_GET();                             //获取时间戳

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u              //如果使能了中断延迟发布
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {    //如果该函数是在中断中被调用
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_SEM, //将该信号量发布到中断消息队列
                    (void      *)p_sem,
                    (void      *)0,
                    (OS_MSG_SIZE)0,
                    (OS_FLAGS   )0,
                    (OS_OPT     )opt,
                    (CPU_TS     )ts,
                    (OS_ERR    *)p_err);
        return ((OS_SEM_CTR)0);                   //返回0（尚未发布），不继续执行        
    }
#endif

    ctr = OS_SemPost(p_sem,                       //将信号量按照普通方式处理
                     opt,
                     ts,
                     p_err);

    return (ctr);                                 //返回信号的当前计数值
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    SET SEMAPHORE
*
* Description: This function sets the semaphore count to the value specified as an argument.  Typically, this value
*              would be 0 but of course, we can set the semaphore to any value.
*
*              You would typically use this function when a semaphore is used as a signaling mechanism
*              and, you want to reset the count value.
*
* Arguments  : p_sem     is a pointer to the semaphore
*
*              cnt       is the new value for the semaphore count.  You would pass 0 to reset the semaphore count.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE           The call was successful and the semaphore value was set.
*                            OS_ERR_OBJ_PTR_NULL   If 'p_sem' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE       If 'p_sem' is not pointing to a semaphore.
*                            OS_ERR_TASK_WAITING   If tasks are waiting on the semaphore.
*
* Returns    : None
************************************************************************************************************************
*/

#if OS_CFG_SEM_SET_EN > 0u           //如果使能 OSSemSet() 函数
void  OSSemSet (OS_SEM      *p_sem,  //多值信号量指针
                OS_SEM_CTR   cnt,    //信号量计数值
                OS_ERR      *p_err)  //返回错误类型
{
    OS_PEND_LIST  *p_pend_list;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL                //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {          //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();  //执行安全检测异常函数
        return;                          //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //如果该函数在中断中被调用
       *p_err = OS_ERR_SET_ISR;                 //返回错误类型为“在中断中设置”
        return;                                 //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u            //如果使能了参数检测
    if (p_sem == (OS_SEM *)0) {       //如果 p_sem 为空
       *p_err = OS_ERR_OBJ_PTR_NULL;  //返回错误类型为“内核对象为空”
        return;                       //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u            //如果使能了对象类型检测
    if (p_sem->Type != OS_OBJ_TYPE_SEM) {  //如果 p_sem 不是多值信号量类型 
       *p_err = OS_ERR_OBJ_TYPE;           //返回错误类型为“内核对象类型错误”
        return;                            //返回0（有错误），不继续执行
    }
#endif

   *p_err = OS_ERR_NONE;                               //返回错误类型为“无错误”
    CPU_CRITICAL_ENTER();                              //关中断
    if (p_sem->Ctr > (OS_SEM_CTR)0) {                  //如果信号量计数值>0，说明没
        p_sem->Ctr = cnt;                              //被等待可以直接设置计数值。
    } else {                                           //如果信号量计数值=0
        p_pend_list = &p_sem->PendList;                //获取信号量的等待列表
        if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0) {//如果没任务在等待信号量
            p_sem->Ctr = cnt;                          //可以直接设置信号计数值
        } else {                                       //如果有任务在等待信号量
           *p_err      = OS_ERR_TASK_WAITING;          //返回错误类型为“有任务等待”
        }
    }
    CPU_CRITICAL_EXIT();
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                           CLEAR THE CONTENTS OF A SEMAPHORE
*
* Description: This function is called by OSSemDel() to clear the contents of a semaphore
*

* Argument(s): p_sem      is a pointer to the semaphore to clear
*              -----
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_SemClr (OS_SEM  *p_sem)
{
    p_sem->Type    = OS_OBJ_TYPE_NONE;                      /* Mark the data structure as a NONE                      */
    p_sem->Ctr     = (OS_SEM_CTR)0;                         /* Set semaphore value                                    */
    p_sem->TS      = (CPU_TS    )0;                         /* Clear the time stamp                                   */
    p_sem->NamePtr = (CPU_CHAR *)((void *)"?SEM");
    OS_PendListInit(&p_sem->PendList);                      /* Initialize the waiting list                            */
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                        ADD/REMOVE SEMAPHORE TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove a semaphore to/from the debug list.
*
* Arguments  : p_sem     is a pointer to the semaphore to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/


#if OS_CFG_DBG_EN > 0u                   //如果使能（默认使能）了调试代码和变量      
void  OS_SemDbgListAdd (OS_SEM  *p_sem)  //将该多值信号量插入到多值信号量列表的最前端
{
    p_sem->DbgNamePtr               = (CPU_CHAR *)((void *)" "); //先不指向任何任务的名称
    p_sem->DbgPrevPtr               = (OS_SEM   *)0;             //将该信号量作为列表的最前端
    if (OSSemDbgListPtr == (OS_SEM *)0) {                        //如果列表为空
        p_sem->DbgNextPtr           = (OS_SEM   *)0;             //该信号量的下一个元素也为空
    } else {                                                     //如果列表非空
        p_sem->DbgNextPtr           =  OSSemDbgListPtr;          //列表原来的首元素作为该信号量的下一个元素
        OSSemDbgListPtr->DbgPrevPtr =  p_sem;                    //原来的首元素的前面变为了该信号量
    }
    OSSemDbgListPtr                 =  p_sem;                    //该信号量成为列表的新首元素
}



void  OS_SemDbgListRemove (OS_SEM  *p_sem)
{
    OS_SEM  *p_sem_next;
    OS_SEM  *p_sem_prev;


    p_sem_prev = p_sem->DbgPrevPtr;
    p_sem_next = p_sem->DbgNextPtr;

    if (p_sem_prev == (OS_SEM *)0) {
        OSSemDbgListPtr = p_sem_next;
        if (p_sem_next != (OS_SEM *)0) {
            p_sem_next->DbgPrevPtr = (OS_SEM *)0;
        }
        p_sem->DbgNextPtr = (OS_SEM *)0;

    } else if (p_sem_next == (OS_SEM *)0) {
        p_sem_prev->DbgNextPtr = (OS_SEM *)0;
        p_sem->DbgPrevPtr      = (OS_SEM *)0;

    } else {
        p_sem_prev->DbgNextPtr =  p_sem_next;
        p_sem_next->DbgPrevPtr =  p_sem_prev;
        p_sem->DbgNextPtr      = (OS_SEM *)0;
        p_sem->DbgPrevPtr      = (OS_SEM *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                SEMAPHORE INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the semaphore management.
*

* Argument(s): p_err        is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE     the call was successful
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_SemInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSSemDbgListPtr = (OS_SEM *)0;
#endif

    OSSemQty        = (OS_OBJ_QTY)0;
   *p_err           = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 POST TO A SEMAPHORE
*
* Description: This function signals a semaphore
*
* Arguments  : p_sem    is a pointer to the semaphore
*
*              opt      determines the type of POST performed:
*
*                           OS_OPT_POST_1            POST to a single waiting task
*                           OS_OPT_POST_ALL          POST to ALL tasks that are waiting on the semaphore
*
*                           OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*                           Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*
*              ts       is a timestamp indicating when the post occurred.
*
*              p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE          The call was successful and the semaphore was signaled.
*                           OS_ERR_OBJ_PTR_NULL  If 'p_sem' is a NULL pointer.
*                           OS_ERR_OBJ_TYPE      If 'p_sem' is not pointing at a semaphore
*                           OS_ERR_SEM_OVF       If the post would cause the semaphore count to overflow.
*
* Returns    : The current value of the semaphore counter or 0 upon error.
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

OS_SEM_CTR  OS_SemPost (OS_SEM  *p_sem, //多值信号量指针
                        OS_OPT   opt,   //选项
                        CPU_TS   ts,    //时间戳
                        OS_ERR  *p_err) //返回错误类型
{
    OS_OBJ_QTY     cnt;
    OS_SEM_CTR     ctr;
    OS_PEND_LIST  *p_pend_list;
    OS_PEND_DATA  *p_pend_data;
    OS_PEND_DATA  *p_pend_data_next;
    OS_TCB        *p_tcb;
    CPU_SR_ALLOC();



    CPU_CRITICAL_ENTER();                                   //关中断
    p_pend_list = &p_sem->PendList;                         //取出该信号量的等待列表
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0) {         //如果没有任务在等待该信号量
        switch (sizeof(OS_SEM_CTR)) {                       //判断是否将导致该信号量计数值溢出，
            case 1u:                                        //如果溢出，则开中断，返回错误类型为
                 if (p_sem->Ctr == DEF_INT_08U_MAX_VAL) {   //“计数值溢出”，返回0（有错误），
                     CPU_CRITICAL_EXIT();                   //不继续执行。
                    *p_err = OS_ERR_SEM_OVF;
                     return ((OS_SEM_CTR)0);
                 }
                 break;

            case 2u:
                 if (p_sem->Ctr == DEF_INT_16U_MAX_VAL) {
                     CPU_CRITICAL_EXIT();
                    *p_err = OS_ERR_SEM_OVF;
                     return ((OS_SEM_CTR)0);
                 }
                 break;

            case 4u:
                 if (p_sem->Ctr == DEF_INT_32U_MAX_VAL) {
                     CPU_CRITICAL_EXIT();
                    *p_err = OS_ERR_SEM_OVF;
                     return ((OS_SEM_CTR)0);
                 }
                 break;

            default:
                 break;
        }
        p_sem->Ctr++;                                       //信号量计数值不溢出则加1
        ctr       = p_sem->Ctr;                             //获取信号量计数值到 ctr
        p_sem->TS = ts;                                     //保存时间戳
        CPU_CRITICAL_EXIT();                                //则开中断
       *p_err     = OS_ERR_NONE;                            //返回错误类型为“无错误”
        return (ctr);                                       //返回信号量的计数值，不继续执行
    }

    OS_CRITICAL_ENTER_CPU_EXIT();                           //加锁调度器，但开中断
    if ((opt & OS_OPT_POST_ALL) != (OS_OPT)0) {             //如果要将信号量发布给所有等待任务
        cnt = p_pend_list->NbrEntries;                      //获取等待任务数目到 cnt
    } else {                                                //如果要将信号量发布给优先级最高的等待任务
        cnt = (OS_OBJ_QTY)1;                                //将要操作的任务数为1，cnt 置1
    }
    p_pend_data = p_pend_list->HeadPtr;                     //获取等待列表的首个任务到 p_pend_data
    while (cnt > 0u) {                                      //逐个处理要发布的任务
        p_tcb            = p_pend_data->TCBPtr;             //取出当前任务
        p_pend_data_next = p_pend_data->NextPtr;            //取出下一个任务
        OS_Post((OS_PEND_OBJ *)((void *)p_sem),             //发布信号量给当前任务
                p_tcb,
                (void      *)0,
                (OS_MSG_SIZE)0,
                ts);
        p_pend_data = p_pend_data_next;                     //处理下一个任务          
        cnt--;
    }
    ctr = p_sem->Ctr;                                       //获取信号量计数值到 ctr
    OS_CRITICAL_EXIT_NO_SCHED();                            //减锁调度器，但不执行任务调度
    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) {        //如果 opt 没选择“发布时不调度任务”
        OSSched();                                          //任务调度
    }
   *p_err = OS_ERR_NONE;                                    //返回错误类型为“无错误”
    return (ctr);                                           //返回信号量的当前计数值
}

#endif
