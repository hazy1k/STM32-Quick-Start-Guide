/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   MUTEX MANAGEMENT
*
* File    : OS_MUTEX.C
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
const  CPU_CHAR  *os_mutex__c = "$Id: $";
#endif


#if OS_CFG_MUTEX_EN > 0u
/*
************************************************************************************************************************
*                                                   CREATE A MUTEX
*
* Description: This function creates a mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex to initialize.  Your application is responsible for allocating
*                            storage for the mutex.
*
*              p_name        is a pointer to the name you would like to give the mutex.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                    if the call was successful
*                                OS_ERR_CREATE_ISR              if you called this function from an ISR
*                                OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Mutex after you called
*                                                                 OSSafetyCriticalStart().
*                                OS_ERR_NAME                    if 'p_name'  is a NULL pointer
*                                OS_ERR_OBJ_CREATED             if the mutex has already been created
*                                OS_ERR_OBJ_PTR_NULL            if 'p_mutex' is a NULL pointer
*
* Returns    : none
************************************************************************************************************************
*/

void  OSMutexCreate (OS_MUTEX  *p_mutex, //互斥信号量指针
                     CPU_CHAR  *p_name,  //取信号量的名称
                     OS_ERR    *p_err)   //返回错误类型
{
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。 

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return;                         //返回，不继续执行
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508               //如果使能（默认禁用）了安全关键
    if (OSSafetyCriticalStartFlag == DEF_TRUE) { //如果是在调用 OSSafetyCriticalStart() 后创建
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;  //错误类型为“非法创建内核对象”
        return;                                  //返回，不继续执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u           //如果使能（默认使能）了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {   //如果该函数是在中断中被调用
       *p_err = OS_ERR_CREATE_ISR;               //错误类型为“在中断函数中定时”
        return;                                  //返回，不继续执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u            //如果使能（默认使能）了参数检测
    if (p_mutex == (OS_MUTEX *)0) {   //如果参数 p_mutex 为空                       
       *p_err = OS_ERR_OBJ_PTR_NULL;  //错误类型为“创建对象为空”
        return;                       //返回，不继续执行
    }
#endif

    OS_CRITICAL_ENTER();              //进入临界段，初始化互斥信号量指标 
    p_mutex->Type              =  OS_OBJ_TYPE_MUTEX;  //标记创建对象数据结构为互斥信号量  
    p_mutex->NamePtr           =  p_name;
    p_mutex->OwnerTCBPtr       = (OS_TCB       *)0;
    p_mutex->OwnerNestingCtr   = (OS_NESTING_CTR)0;   //互斥信号量目前可用
    p_mutex->TS                = (CPU_TS        )0;
    p_mutex->OwnerOriginalPrio =  OS_CFG_PRIO_MAX;
    OS_PendListInit(&p_mutex->PendList);              //初始化该互斥信号量的等待列表   

#if OS_CFG_DBG_EN > 0u           //如果使能（默认使能）了调试代码和变量 
    OS_MutexDbgListAdd(p_mutex); //将该信号量添加到互斥信号量双向调试链表
#endif
    OSMutexQty++;                //互斥信号量个数加1         

    OS_CRITICAL_EXIT_NO_SCHED(); //退出临界段（无调度）
   *p_err = OS_ERR_NONE;          //错误类型为“无错误”
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   DELETE A MUTEX
*
* Description: This function deletes a mutex and readies all tasks pending on the mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex to delete
*
*              opt           determines delete options as follows:
*
*                                OS_OPT_DEL_NO_PEND          Delete mutex ONLY if no task pending
*                                OS_OPT_DEL_ALWAYS           Deletes the mutex even if tasks are waiting.
*                                                            In this case, all the tasks pending will be readied.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE                 The call was successful and the mutex was deleted
*                                OS_ERR_DEL_ISR              If you attempted to delete the mutex from an ISR
*                                OS_ERR_OBJ_PTR_NULL         If 'p_mutex' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE             If 'p_mutex' is not pointing to a mutex
*                                OS_ERR_OPT_INVALID          An invalid option was specified
*                                OS_ERR_STATE_INVALID        Task is in an invalid state
*                                OS_ERR_TASK_WAITING         One or more tasks were waiting on the mutex
*
* Returns    : == 0          if no tasks were waiting on the mutex, or upon error.
*              >  0          if one or more tasks waiting on the mutex are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the mutex MUST
*                 check the return code of OSMutexPend().
*
*              2) OSMutexAccept() callers will not know that the intended mutex has been deleted.
*
*              3) Because ALL tasks pending on the mutex will be readied, you MUST be careful in applications where the
*                 mutex is used for mutual exclusion because the resource(s) will no longer be guarded by the mutex.
************************************************************************************************************************
*/

#if OS_CFG_MUTEX_DEL_EN > 0u                //如果使能了 OSMutexDel()   
OS_OBJ_QTY  OSMutexDel (OS_MUTEX  *p_mutex, //互斥信号量指针
                        OS_OPT     opt,     //选项
                        OS_ERR    *p_err)   //返回错误类型
{
    OS_OBJ_QTY     cnt;
    OS_OBJ_QTY     nbr_tasks;
    OS_PEND_DATA  *p_pend_data;
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    OS_TCB        *p_tcb_owner;
    CPU_TS         ts;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return ((OS_OBJ_QTY)0);         //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u        //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {//如果该函数在中断中被调用
       *p_err = OS_ERR_DEL_ISR;               //错误类型为“在中断中中止等待”
        return ((OS_OBJ_QTY)0);               //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //如果使能了参数检测
    if (p_mutex == (OS_MUTEX *)0) {       //如果 p_mutex 为空             
       *p_err = OS_ERR_OBJ_PTR_NULL;      //错误类型为“对象为空”
        return ((OS_OBJ_QTY)0);           //返回0（有错误），停止执行
    }
    switch (opt) {                        //根据选项分类处理
        case OS_OPT_DEL_NO_PEND:          //如果选项在预期内
        case OS_OPT_DEL_ALWAYS:
             break;                       //直接跳出

        default:                          //如果选项超出预期
            *p_err =  OS_ERR_OPT_INVALID; //错误类型为“选项非法”
             return ((OS_OBJ_QTY)0);      //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u               //如果使能了对象类型检测
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) { //如果 p_mutex 非互斥信号量类型
       *p_err = OS_ERR_OBJ_TYPE;              //错误类型为“对象类型错误”
        return ((OS_OBJ_QTY)0);               //返回0（有错误），停止执行
    }
#endif

    OS_CRITICAL_ENTER();                        //进入临界段
    p_pend_list = &p_mutex->PendList;           //获取信号量的等待列表
    cnt         = p_pend_list->NbrEntries;      //获取等待该信号量的任务数
    nbr_tasks   = cnt;
    switch (opt) {                              //根据选项分类处理
        case OS_OPT_DEL_NO_PEND:                //如果只在没任务等待时删除信号量
             if (nbr_tasks == (OS_OBJ_QTY)0) {  //如果没有任务在等待该信号量
#if OS_CFG_DBG_EN > 0u                          //如果使能了调试代码和变量  
                 OS_MutexDbgListRemove(p_mutex);//将该信号量从信号量调试列表移除
#endif
                 OSMutexQty--;                  //互斥信号量数目减1
                 OS_MutexClr(p_mutex);          //清除信号量内容
                 OS_CRITICAL_EXIT();            //退出临界段
                *p_err = OS_ERR_NONE;           //错误类型为“无错误”
             } else {                           //如果有任务在等待该信号量
                 OS_CRITICAL_EXIT();            //退出临界段
                *p_err = OS_ERR_TASK_WAITING;   //错误类型为“有任务正在等待”
             }
             break;                             //跳出

        case OS_OPT_DEL_ALWAYS:                                          //如果必须删除信号量  
             p_tcb_owner = p_mutex->OwnerTCBPtr;                         //获取信号量持有任务
             if ((p_tcb_owner       != (OS_TCB *)0) &&                   //如果持有任务存在，
                 (p_tcb_owner->Prio !=  p_mutex->OwnerOriginalPrio)) {   //而且优先级被提升过。
                 switch (p_tcb_owner->TaskState) {                       //根据其任务状态处理
                     case OS_TASK_STATE_RDY:                             //如果是就绪状态
                          OS_RdyListRemove(p_tcb_owner);                 //将任务从就绪列表移除
                          p_tcb_owner->Prio = p_mutex->OwnerOriginalPrio;//还原任务的优先级
                          OS_PrioInsert(p_tcb_owner->Prio);              //将该优先级插入优先级表格
                          OS_RdyListInsertTail(p_tcb_owner);             //将任务重插入就绪列表
                          break;                                         //跳出

                     case OS_TASK_STATE_DLY:                             //如果是延时状态
                     case OS_TASK_STATE_SUSPENDED:                       //如果是被挂起状态
                     case OS_TASK_STATE_DLY_SUSPENDED:                   //如果是延时中被挂起状态
                          p_tcb_owner->Prio = p_mutex->OwnerOriginalPrio;//还原任务的优先级
                          break;

                     case OS_TASK_STATE_PEND:                            //如果是无期限等待状态
                     case OS_TASK_STATE_PEND_TIMEOUT:                    //如果是有期限等待状态
                     case OS_TASK_STATE_PEND_SUSPENDED:                  //如果是无期等待中被挂状态
                     case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:          //如果是有期等待中被挂状态
                          OS_PendListChangePrio(p_tcb_owner,             //改变任务在等待列表的位置
                                                p_mutex->OwnerOriginalPrio);
                          break;

                     default:                                            //如果状态超出预期
                          OS_CRITICAL_EXIT();
                         *p_err = OS_ERR_STATE_INVALID;                  //错误类型为“任务状态非法”
                          return ((OS_OBJ_QTY)0);                        //返回0（有错误），停止执行
                 }
             }

             ts = OS_TS_GET();                                           //获取时间戳
             while (cnt > 0u) {                                          //移除该互斥信号量等待列表
                 p_pend_data = p_pend_list->HeadPtr;                     //中的所有任务。
                 p_tcb       = p_pend_data->TCBPtr;
                 OS_PendObjDel((OS_PEND_OBJ *)((void *)p_mutex),
                               p_tcb,
                               ts);
                 cnt--;
             }
#if OS_CFG_DBG_EN > 0u                          //如果使能了调试代码和变量 
             OS_MutexDbgListRemove(p_mutex);    //将信号量从互斥信号量调试列表移除
#endif
             OSMutexQty--;                      //互斥信号量数目减1
             OS_MutexClr(p_mutex);              //清除信号量内容
             OS_CRITICAL_EXIT_NO_SCHED();       //退出临界段，但不调度
             OSSched();                         //调度最高优先级任务运行
            *p_err = OS_ERR_NONE;               //错误类型为“无错误”
             break;                             //跳出

        default:                                //如果选项超出预期
             OS_CRITICAL_EXIT();                //退出临界段
            *p_err = OS_ERR_OPT_INVALID;        //错误类型为“选项非法”
             break;                             //跳出
    }
    return (nbr_tasks);                         //返回删除前信号量等待列表中的任务数
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    PEND ON MUTEX
*
* Description: This function waits for a mutex.
*
* Arguments  : p_mutex       is a pointer to the mutex
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for the
*                            resource up to the amount of time (in 'ticks') specified by this argument.  If you specify
*                            0, however, your task will wait forever at the specified mutex or, until the resource
*                            becomes available.
*
*              opt           determines whether the user wants to block if the mutex is not available or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the mutex was posted or
*                            pend aborted or the mutex deleted.  If you pass a NULL pointer (i.e. (CPU_TS *)0) then you
*                            will not get the timestamp.  In other words, passing a NULL pointer is valid and indicates
*                            that you don't need the timestamp.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE               The call was successful and your task owns the resource
*                                OS_ERR_MUTEX_OWNER        If calling task already owns the mutex
*                                OS_ERR_OBJ_DEL            If 'p_mutex' was deleted
*                                OS_ERR_OBJ_PTR_NULL       If 'p_mutex' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE           If 'p_mutex' is not pointing at a mutex
*                                OS_ERR_OPT_INVALID        If you didn't specify a valid option
*                                OS_ERR_PEND_ABORT         If the pend was aborted by another task
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                                          would lead to a suspension.
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the mutex was not
*                                                          available.
*                                OS_ERR_SCHED_LOCKED       If you called this function when the scheduler is locked
*                                OS_ERR_STATE_INVALID      If the task is in an invalid state
*                                OS_ERR_STATUS_INVALID     If the pend status has an invalid value
*                                OS_ERR_TIMEOUT            The mutex was not received within the specified timeout.
*
* Returns    : none
************************************************************************************************************************
*/

void  OSMutexPend (OS_MUTEX  *p_mutex, //互斥信号量指针
                   OS_TICK    timeout, //超时时间（节拍）
                   OS_OPT     opt,     //选项
                   CPU_TS    *p_ts,    //时间戳
                   OS_ERR    *p_err)   //返回错误类型
{
    OS_PEND_DATA  pend_data;
    OS_TCB       *p_tcb;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return;                         //返回，不继续执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
       *p_err = OS_ERR_PEND_ISR;               //错误类型为“在中断中等待”
        return;                                //返回，不继续执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u               //如果使能了参数检测
    if (p_mutex == (OS_MUTEX *)0) {      //如果 p_mutex 为空
       *p_err = OS_ERR_OBJ_PTR_NULL;     //返回错误类型为“内核对象为空”
        return;                          //返回，不继续执行
    }
    switch (opt) {                       //根据选项分类处理
        case OS_OPT_PEND_BLOCKING:       //如果选项在预期内
        case OS_OPT_PEND_NON_BLOCKING:
             break;

        default:                         //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID; //错误类型为“选项非法”
             return;                     //返回，不继续执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u               //如果使能了对象类型检测
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) { //如果 p_mutex 非互斥信号量类型
       *p_err = OS_ERR_OBJ_TYPE;              //错误类型为“内核对象类型错误”
        return;                               //返回，不继续执行
    }
#endif

    if (p_ts != (CPU_TS *)0) {  //如果 p_ts 非空
       *p_ts  = (CPU_TS  )0;    //初始化（清零）p_ts，待用于返回时间戳    
    }

    CPU_CRITICAL_ENTER();                                //关中断
    if (p_mutex->OwnerNestingCtr == (OS_NESTING_CTR)0) { //如果信号量可用
        p_mutex->OwnerTCBPtr       =  OSTCBCurPtr;       //让当前任务持有信号量 
        p_mutex->OwnerOriginalPrio =  OSTCBCurPtr->Prio; //保存持有任务的优先级
        p_mutex->OwnerNestingCtr   = (OS_NESTING_CTR)1;  //开始嵌套
        if (p_ts != (CPU_TS *)0) {                       //如果 p_ts 非空    
           *p_ts  = p_mutex->TS;                         //返回信号量的时间戳记录
        }
        CPU_CRITICAL_EXIT();                             //开中断
       *p_err = OS_ERR_NONE;                             //错误类型为“无错误”
        return;                                          //返回，不继续执行
    }
    /* 如果信号量不可用 */
    if (OSTCBCurPtr == p_mutex->OwnerTCBPtr) { //如果当前任务已经持有该信号量
        p_mutex->OwnerNestingCtr++;            //信号量前套数加1
        if (p_ts != (CPU_TS *)0) {             //如果 p_ts 非空
           *p_ts  = p_mutex->TS;               //返回信号量的时间戳记录
        }
        CPU_CRITICAL_EXIT();                   //开中断
       *p_err = OS_ERR_MUTEX_OWNER;            //错误类型为“任务已持有信号量”
        return;                                //返回，不继续执行
    }
    /* 如果当前任务非持有该信号量 */
    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {//如果选择了不堵塞任务
        CPU_CRITICAL_EXIT();                            //开中断
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                //错误类型为“渴求堵塞”  
        return;                                         //返回，不继续执行
    } else {                                            //如果选择了堵塞任务
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {//如果调度器被锁
            CPU_CRITICAL_EXIT();                        //开中断
           *p_err = OS_ERR_SCHED_LOCKED;                //错误类型为“调度器被锁”
            return;                                     //返回，不继续执行
        }
    }
    /* 如果调度器未被锁 */                                                        
    OS_CRITICAL_ENTER_CPU_EXIT();                          //锁调度器，并重开中断
    p_tcb = p_mutex->OwnerTCBPtr;                          //获取信号量持有任务
    if (p_tcb->Prio > OSTCBCurPtr->Prio) {                 //如果持有任务优先级低于当前任务
        switch (p_tcb->TaskState) {                        //根据持有任务的任务状态分类处理
            case OS_TASK_STATE_RDY:                        //如果是就绪状态
                 OS_RdyListRemove(p_tcb);                  //从就绪列表移除持有任务
                 p_tcb->Prio = OSTCBCurPtr->Prio;          //提升持有任务的优先级到当前任务
                 OS_PrioInsert(p_tcb->Prio);               //将该优先级插入优先级表格
                 OS_RdyListInsertHead(p_tcb);              //将持有任务插入就绪列表
                 break;                                    //跳出

            case OS_TASK_STATE_DLY:                        //如果是延时状态
            case OS_TASK_STATE_DLY_SUSPENDED:              //如果是延时中被挂起状态
            case OS_TASK_STATE_SUSPENDED:                  //如果是被挂起状态
                 p_tcb->Prio = OSTCBCurPtr->Prio;          //提升持有任务的优先级到当前任务
                 break;                                    //跳出

            case OS_TASK_STATE_PEND:                       //如果是无期限等待状态
            case OS_TASK_STATE_PEND_TIMEOUT:               //如果是有期限等待状态
            case OS_TASK_STATE_PEND_SUSPENDED:             //如果是无期限等待中被挂起状态
            case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:     //如果是有期限等待中被挂起状态
                 OS_PendListChangePrio(p_tcb,              //改变持有任务在等待列表的位置
                                       OSTCBCurPtr->Prio);
                 break;                                    //跳出

            default:                                       //如果任务状态超出预期
                 OS_CRITICAL_EXIT();                       //开中断
                *p_err = OS_ERR_STATE_INVALID;             //错误类型为“任务状态非法”
                 return;                                   //返回，不继续执行
        }
    }
    /* 堵塞任务，将当前任务脱离就绪列表，并插入到节拍列表和等待列表。*/
    OS_Pend(&pend_data,                                     
            (OS_PEND_OBJ *)((void *)p_mutex),
             OS_TASK_PEND_ON_MUTEX,
             timeout);

    OS_CRITICAL_EXIT_NO_SCHED();          //开调度器，但不进行调度

    OSSched();                            //调度最高优先级任务运行

    CPU_CRITICAL_ENTER();                 //开中断
    switch (OSTCBCurPtr->PendStatus) {    //根据当前运行任务的等待状态分类处理
        case OS_STATUS_PEND_OK:           //如果等待正常（获得信号量）
             if (p_ts != (CPU_TS *)0) {   //如果 p_ts 非空
                *p_ts  = OSTCBCurPtr->TS; //返回信号量最后一次被释放的时间戳
             }
            *p_err = OS_ERR_NONE;         //错误类型为“无错误”
             break;                       //跳出

        case OS_STATUS_PEND_ABORT:        //如果等待被中止
             if (p_ts != (CPU_TS *)0) {   //如果 p_ts 非空
                *p_ts  = OSTCBCurPtr->TS; //返回等待被中止时的时间戳
             }
            *p_err = OS_ERR_PEND_ABORT;   //错误类型为“等待被中止”
             break;                       //跳出

        case OS_STATUS_PEND_TIMEOUT:      //如果超时内为获得信号量
             if (p_ts != (CPU_TS *)0) {   //如果 p_ts 非空
                *p_ts  = (CPU_TS  )0;     //清零 p_ts
             }
            *p_err = OS_ERR_TIMEOUT;      //错误类型为“超时”
             break;                       //跳出

        case OS_STATUS_PEND_DEL:          //如果信号量已被删除                  
             if (p_ts != (CPU_TS *)0) {   //如果 p_ts 非空
                *p_ts  = OSTCBCurPtr->TS; //返回信号量被删除时的时间戳
             }
            *p_err = OS_ERR_OBJ_DEL;      //错误类型为“对象被删除”
             break;                       //跳出

        default:                           //根据等待状态超出预期
            *p_err = OS_ERR_STATUS_INVALID;//错误类型为“状态非法”
             break;                        //跳出
    }
    CPU_CRITICAL_EXIT();                   //开中断
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               ABORT WAITING ON A MUTEX
*
* Description: This function aborts & readies any tasks currently waiting on a mutex.  This function should be used
*              to fault-abort the wait on the mutex, rather than to normally signal the mutex via OSMutexPost().
*
* Arguments  : p_mutex   is a pointer to the mutex
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the mutex
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the mutex
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the mutex was readied and
*                                                         informed of the aborted wait; check return value for the
*                                                         number of tasks whose wait on the mutex was aborted.
*                            OS_ERR_OBJ_PTR_NULL          If 'p_mutex' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If 'p_mutex' is not pointing at a mutex
*                            OS_ERR_OPT_INVALID           If you specified an invalid option
*                            OS_ERR_PEND_ABORT_ISR        If you attempted to call this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the mutex, or upon error.
*              >  0          if one or more tasks waiting on the mutex are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_MUTEX_PEND_ABORT_EN > 0u               //如果使能了 OSMutexPendAbort()
OS_OBJ_QTY  OSMutexPendAbort (OS_MUTEX  *p_mutex, //互斥信号量指针
                              OS_OPT     opt,     //选项
                              OS_ERR    *p_err)   //返回错误类型
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    OS_OBJ_QTY     nbr_tasks;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return ((OS_OBJ_QTY)0u);        //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) { //如果该函数在中断中被调用
       *p_err =  OS_ERR_PEND_ABORT_ISR;         //错误类型为“在中断中中止等待”
        return ((OS_OBJ_QTY)0u);                //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                   //如果使能了参数检测
    if (p_mutex == (OS_MUTEX *)0) {          //如果 p_sem 为空
       *p_err =  OS_ERR_OBJ_PTR_NULL;        //错误类型为“对象为空”
        return ((OS_OBJ_QTY)0u);             //返回0（有错误），不继续执行
    }
    switch (opt) {                                         //根据选项分类处理
        case OS_OPT_PEND_ABORT_1:                          //如果选项在预期内
        case OS_OPT_PEND_ABORT_ALL:
        case OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED:
             break;                                        //直接跳出

        default:                               //如果选项超出预期
            *p_err =  OS_ERR_OPT_INVALID;      //错误类型为“选项非法”
             return ((OS_OBJ_QTY)0u);          //返回0（有错误），不继续执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u               //如果使能了对象类型检测
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) { //如果 p_mutex 不是互斥信号量类型
       *p_err =  OS_ERR_OBJ_TYPE;             //错误类型为“对象类型错误”
        return ((OS_OBJ_QTY)0u);              //返回0（有错误），不继续执行
    }
#endif

    CPU_CRITICAL_ENTER();                            //关中断
    p_pend_list = &p_mutex->PendList;                //获取 p_mutex 的等待列表
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0u) { //如果没有任务在等待 p_mutex
        CPU_CRITICAL_EXIT();                         //开中断
       *p_err =  OS_ERR_PEND_ABORT_NONE;             //错误类型为“没有任务在等待”
        return ((OS_OBJ_QTY)0u);                     //返回0（有错误），不继续执行
    }

    OS_CRITICAL_ENTER_CPU_EXIT();                      //加锁调度器，重开中断
    nbr_tasks = 0u;
    ts        = OS_TS_GET();                           //获取时间戳
    while (p_pend_list->NbrEntries > (OS_OBJ_QTY)0u) { //如果有任务在等待 p_mutex
        p_tcb = p_pend_list->HeadPtr->TCBPtr;          //获取优先级最高的等待任务
        OS_PendAbort((OS_PEND_OBJ *)((void *)p_mutex), //中止该任务对p_mutex的等待
                     p_tcb,
                     ts);
        nbr_tasks++;
        if (opt != OS_OPT_PEND_ABORT_ALL) {            //如果非选择中止所有等待任务
            break;                                     //立即跳出，不再继续中止
        }
    }
    OS_CRITICAL_EXIT_NO_SCHED();                       //减锁调度器，但不调度

    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0u) {  //如果选择了任务调度
        OSSched();                                     //进行任务调度
    }

   *p_err = OS_ERR_NONE;                               //返错误类型为“无错误”
    return (nbr_tasks);                                //返回被中止的任务数目
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   POST TO A MUTEX
*
* Description: This function signals a mutex
*
* Arguments  : p_mutex  is a pointer to the mutex
*
*              opt      is an option you can specify to alter the behavior of the post.  The choices are:
*
*                           OS_OPT_POST_NONE        No special option selected
*                           OS_OPT_POST_NO_SCHED    If you don't want the scheduler to be called after the post.
*
*              p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_NONE             The call was successful and the mutex was signaled.
*                           OS_ERR_MUTEX_NESTING    Mutex owner nested its use of the mutex
*                           OS_ERR_MUTEX_NOT_OWNER  If the task posting is not the Mutex owner
*                           OS_ERR_OBJ_PTR_NULL     If 'p_mutex' is a NULL pointer.
*                           OS_ERR_OBJ_TYPE         If 'p_mutex' is not pointing at a mutex
*                           OS_ERR_POST_ISR         If you attempted to post from an ISR
*
* Returns    : none
************************************************************************************************************************
*/

void  OSMutexPost (OS_MUTEX  *p_mutex, //互斥信号量指针
                   OS_OPT     opt,     //选项
                   OS_ERR    *p_err)   //返回错误类型
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和定义一个局部变
                    //量，用于保存关中断前的 CPU 状态寄存器 SR（临界段关中断只需保存SR）
                    //，开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return;                         //返回，不继续执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
       *p_err = OS_ERR_POST_ISR;               //错误类型为“在中断中等待”
        return;                                //返回，不继续执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                 //如果使能了参数检测
    if (p_mutex == (OS_MUTEX *)0) {        //如果 p_mutex 为空            
       *p_err = OS_ERR_OBJ_PTR_NULL;       //错误类型为“内核对象为空”
        return;                            //返回，不继续执行
    }
    switch (opt) {                         //根据选项分类处理  
        case OS_OPT_POST_NONE:             //如果选项在预期内，不处理
        case OS_OPT_POST_NO_SCHED:
             break;

        default:                           //如果选项超出预期
            *p_err =  OS_ERR_OPT_INVALID;  //错误类型为“选项非法”
             return;                       //返回，不继续执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u               //如果使能了对象类型检测   
    if (p_mutex->Type != OS_OBJ_TYPE_MUTEX) { //如果 p_mutex 的类型不是互斥信号量类型 
       *p_err = OS_ERR_OBJ_TYPE;              //返回，不继续执行
        return;
    }
#endif

    CPU_CRITICAL_ENTER();                      //关中断
    if (OSTCBCurPtr != p_mutex->OwnerTCBPtr) { //如果当前运行任务不持有该信号量
        CPU_CRITICAL_EXIT();                   //开中断
       *p_err = OS_ERR_MUTEX_NOT_OWNER;        //错误类型为“任务不持有该信号量”
        return;                                //返回，不继续执行
    }

    OS_CRITICAL_ENTER_CPU_EXIT();                       //锁调度器，开中断
    ts          = OS_TS_GET();                          //获取时间戳
    p_mutex->TS = ts;                                   //存储信号量最后一次被释放的时间戳
    p_mutex->OwnerNestingCtr--;                         //信号量的嵌套数减1
    if (p_mutex->OwnerNestingCtr > (OS_NESTING_CTR)0) { //如果信号量仍被嵌套
        OS_CRITICAL_EXIT();                             //解锁调度器
       *p_err = OS_ERR_MUTEX_NESTING;                   //错误类型为“信号量被嵌套”
        return;                                         //返回，不继续执行
    }
    /* 如果信号量未被嵌套，已可用 */
    p_pend_list = &p_mutex->PendList;                //获取信号量的等待列表
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0) {  //如果没有任务在等待该信号量
        p_mutex->OwnerTCBPtr     = (OS_TCB       *)0;//请空信号量持有者信息
        p_mutex->OwnerNestingCtr = (OS_NESTING_CTR)0;
        OS_CRITICAL_EXIT();                          //解锁调度器
       *p_err = OS_ERR_NONE;                         //错误类型为“无错误”
        return;                                      //返回，不继续执行
    }
    /* 如果有任务在等待该信号量 */
    if (OSTCBCurPtr->Prio != p_mutex->OwnerOriginalPrio) { //如果当前任务的优先级被改过
        OS_RdyListRemove(OSTCBCurPtr);                     //从就绪列表移除当前任务
        OSTCBCurPtr->Prio = p_mutex->OwnerOriginalPrio;    //还原当前任务的优先级
        OS_PrioInsert(OSTCBCurPtr->Prio);                  //在优先级表格插入这个优先级
        OS_RdyListInsertTail(OSTCBCurPtr);                 //将当前任务插入就绪列表尾端
        OSPrioCur         = OSTCBCurPtr->Prio;             //更改当前任务优先级变量的值
    }

    p_tcb                      = p_pend_list->HeadPtr->TCBPtr; //获取等待列表的首端任务
    p_mutex->OwnerTCBPtr       = p_tcb;                        //将信号量交给该任务
    p_mutex->OwnerOriginalPrio = p_tcb->Prio;
    p_mutex->OwnerNestingCtr   = (OS_NESTING_CTR)1;            //开始嵌套
    /* 释放信号量给该任务 */
    OS_Post((OS_PEND_OBJ *)((void *)p_mutex), 
            (OS_TCB      *)p_tcb,
            (void        *)0,
            (OS_MSG_SIZE  )0,
            (CPU_TS       )ts);

    OS_CRITICAL_EXIT_NO_SCHED();                     //减锁调度器，但不执行任务调度

    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) { //如果 opt 没选择“发布时不调度任务”
        OSSched();                                   //任务调度
    }

   *p_err = OS_ERR_NONE;                             //错误类型为“无错误”
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                            CLEAR THE CONTENTS OF A MUTEX
*
* Description: This function is called by OSMutexDel() to clear the contents of a mutex
*

* Argument(s): p_mutex      is a pointer to the mutex to clear
*              -------
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_MutexClr (OS_MUTEX  *p_mutex)
{
    p_mutex->Type              =  OS_OBJ_TYPE_NONE;         /* Mark the data structure as a NONE                      */
    p_mutex->NamePtr           = (CPU_CHAR     *)((void *)"?MUTEX");
    p_mutex->OwnerTCBPtr       = (OS_TCB       *)0;
    p_mutex->OwnerNestingCtr   = (OS_NESTING_CTR)0;
    p_mutex->TS                = (CPU_TS        )0;
    p_mutex->OwnerOriginalPrio =  OS_CFG_PRIO_MAX;
    OS_PendListInit(&p_mutex->PendList);                    /* Initialize the waiting list                            */
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                          ADD/REMOVE MUTEX TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove a mutex to/from the debug list.
*
* Arguments  : p_mutex     is a pointer to the mutex to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/


#if OS_CFG_DBG_EN > 0u                        //如果使能（默认使能）了调试代码和变量  
void  OS_MutexDbgListAdd (OS_MUTEX  *p_mutex) //将该互斥信号量插入到互斥信号量列表的最前端
{
    p_mutex->DbgNamePtr               = (CPU_CHAR *)((void *)" "); //先不指向任何任务的名称
    p_mutex->DbgPrevPtr               = (OS_MUTEX *)0;             //将该信号量作为列表的最前端
    if (OSMutexDbgListPtr == (OS_MUTEX *)0) {                      //如果列表为空
        p_mutex->DbgNextPtr           = (OS_MUTEX *)0;             //该信号量的下一个元素也为空
    } else {                                                       //如果列表非空
        p_mutex->DbgNextPtr           =  OSMutexDbgListPtr;        //列表原来的首元素作为该信号量的下一个元素
        OSMutexDbgListPtr->DbgPrevPtr =  p_mutex;                  //原来的首元素的前面变为了该信号量
    }
    OSMutexDbgListPtr                 =  p_mutex;                  //该信号量成为列表的新首元素
}



void  OS_MutexDbgListRemove (OS_MUTEX  *p_mutex)
{
    OS_MUTEX  *p_mutex_next;
    OS_MUTEX  *p_mutex_prev;


    p_mutex_prev = p_mutex->DbgPrevPtr;
    p_mutex_next = p_mutex->DbgNextPtr;

    if (p_mutex_prev == (OS_MUTEX *)0) {
        OSMutexDbgListPtr = p_mutex_next;
        if (p_mutex_next != (OS_MUTEX *)0) {
            p_mutex_next->DbgPrevPtr = (OS_MUTEX *)0;
        }
        p_mutex->DbgNextPtr = (OS_MUTEX *)0;

    } else if (p_mutex_next == (OS_MUTEX *)0) {
        p_mutex_prev->DbgNextPtr = (OS_MUTEX *)0;
        p_mutex->DbgPrevPtr      = (OS_MUTEX *)0;

    } else {
        p_mutex_prev->DbgNextPtr =  p_mutex_next;
        p_mutex_next->DbgPrevPtr =  p_mutex_prev;
        p_mutex->DbgNextPtr      = (OS_MUTEX *)0;
        p_mutex->DbgPrevPtr      = (OS_MUTEX *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                MUTEX INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the mutex management.
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

void  OS_MutexInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSMutexDbgListPtr = (OS_MUTEX *)0;
#endif

    OSMutexQty        = (OS_OBJ_QTY)0;
   *p_err             =  OS_ERR_NONE;
}

#endif                                                      /* OS_CFG_MUTEX_EN                                        */
