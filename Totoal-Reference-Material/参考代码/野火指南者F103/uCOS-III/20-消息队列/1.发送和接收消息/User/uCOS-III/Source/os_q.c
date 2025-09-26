/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                               MESSAGE QUEUE MANAGEMENT
*
* File    : OS_Q.C
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
const  CPU_CHAR  *os_q__c = "$Id: $";
#endif


#if OS_CFG_Q_EN > 0u
/*
************************************************************************************************************************
*                                               CREATE A MESSAGE QUEUE
*
* Description: This function is called by your application to create a message queue.  Message queues MUST be created
*              before they can be used.
*
* Arguments  : p_q         is a pointer to the message queue
*
*              p_name      is a pointer to an ASCII string that will be used to name the message queue
*
*              max_qty     indicates the maximum size of the message queue (must be non-zero).  Note that it's also not
*                          possible to have a size higher than the maximum number of OS_MSGs available.
*
*              p_err       is a pointer to a variable that will contain an error code returned by this function.
*
*                              OS_ERR_NONE                    the call was successful
*                              OS_ERR_CREATE_ISR              can't create from an ISR
*                              OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Queue after you called
*                                                               OSSafetyCriticalStart().
*                              OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                              OS_ERR_OBJ_CREATED             if the message queue has already been created
*                              OS_ERR_OBJ_PTR_NULL            if you passed a NULL pointer for 'p_q'
*                              OS_ERR_Q_SIZE                  if the size you specified is 0
*
* Returns    : none
************************************************************************************************************************
*/

void  OSQCreate (OS_Q        *p_q,     //消息队列指针
                 CPU_CHAR    *p_name,  //消息队列名称
                 OS_MSG_QTY   max_qty, //消息队列大小（不能为0）
                 OS_ERR      *p_err)   //返回错误类型

{
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return;                         //返回，停止执行
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508               //如果使能了安全关键
    if (OSSafetyCriticalStartFlag == DEF_TRUE) { //如果在调用OSSafetyCriticalStart()后创建
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;  //错误类型为“非法创建内核对象”
        return;                                  //返回，停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数是在中断中被调用
       *p_err = OS_ERR_CREATE_ISR;             //错误类型为“在中断中创建对象”
        return;                                //返回，停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u            //如果使能了参数检测
    if (p_q == (OS_Q *)0) {           //如果 p_q 为空                       
       *p_err = OS_ERR_OBJ_PTR_NULL;  //错误类型为“创建对象为空”
        return;                       //返回，停止执行
    }
    if (max_qty == (OS_MSG_QTY)0) {   //如果 max_qty = 0            
       *p_err = OS_ERR_Q_SIZE;        //错误类型为“队列空间为0”
        return;                       //返回，停止执行
    }
#endif

    OS_CRITICAL_ENTER();             //进入临界段
    p_q->Type    = OS_OBJ_TYPE_Q;    //标记创建对象数据结构为消息队列  
    p_q->NamePtr = p_name;           //标记消息队列的名称
    OS_MsgQInit(&p_q->MsgQ,          //初始化消息队列
                max_qty);
    OS_PendListInit(&p_q->PendList); //初始化该消息队列的等待列表   

#if OS_CFG_DBG_EN > 0u               //如果使能了调试代码和变量
    OS_QDbgListAdd(p_q);             //将该队列添加到消息队列双向调试链表
#endif
    OSQQty++;                        //消息队列个数加1

    OS_CRITICAL_EXIT_NO_SCHED();     //退出临界段（无调度）
   *p_err = OS_ERR_NONE;             //错误类型为“无错误”
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               DELETE A MESSAGE QUEUE
*
* Description: This function deletes a message queue and readies all tasks pending on the queue.
*
* Arguments  : p_q       is a pointer to the message queue you want to delete
*
*              opt       determines delete options as follows:
*
*                            OS_OPT_DEL_NO_PEND          Delete the queue ONLY if no task pending
*                            OS_OPT_DEL_ALWAYS           Deletes the queue even if tasks are waiting.
*                                                        In this case, all the tasks pending will be readied.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                 The call was successful and the queue was deleted
*                            OS_ERR_DEL_ISR              If you tried to delete the queue from an ISR
*                            OS_ERR_OBJ_PTR_NULL         if you pass a NULL pointer for 'p_q'
*                            OS_ERR_OBJ_TYPE             if the message queue was not created
*                            OS_ERR_OPT_INVALID          An invalid option was specified
*                            OS_ERR_TASK_WAITING         One or more tasks were waiting on the queue
*
* Returns    : == 0          if no tasks were waiting on the queue, or upon error.
*              >  0          if one or more tasks waiting on the queue are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the queue MUST
*                 check the return code of OSQPend().
*
*              2) OSQAccept() callers will not know that the intended queue has been deleted.
*
*              3) Because ALL tasks pending on the queue will be readied, you MUST be careful in applications where the
*                 queue is used for mutual exclusion because the resource(s) will no longer be guarded by the queue.
************************************************************************************************************************
*/

#if OS_CFG_Q_DEL_EN > 0u             //如果使能了 OSQDel() 函数
OS_OBJ_QTY  OSQDel (OS_Q    *p_q,    //消息队列指针
                    OS_OPT   opt,    //选项
                    OS_ERR  *p_err)  //返回错误类型
{
    OS_OBJ_QTY     cnt;
    OS_OBJ_QTY     nbr_tasks;
    OS_PEND_DATA  *p_pend_data;
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
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

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //如果该函数在中断中被调用
       *p_err = OS_ERR_DEL_ISR;                 //错误类型为“在中断中中止等待”
        return ((OS_OBJ_QTY)0);                 //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //如果使能了参数检测
    if (p_q == (OS_Q *)0) {               //如果 p_q 为空
       *p_err =  OS_ERR_OBJ_PTR_NULL;     //错误类型为“对象为空”
        return ((OS_OBJ_QTY)0u);          //返回0（有错误），停止执行
    }
    switch (opt) {                        //根据选项分类处理
        case OS_OPT_DEL_NO_PEND:          //如果选项在预期内
        case OS_OPT_DEL_ALWAYS:  
             break;                       //直接跳出

        default:
            *p_err =  OS_ERR_OPT_INVALID; //如果选项超出预期
             return ((OS_OBJ_QTY)0u);     //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u       //如果使能了对象类型检测
    if (p_q->Type != OS_OBJ_TYPE_Q) { //如果 p_q 不是消息队列类型
       *p_err = OS_ERR_OBJ_TYPE;      //错误类型为“对象类型有误”
        return ((OS_OBJ_QTY)0);       //返回0（有错误），停止执行
    }
#endif

    CPU_CRITICAL_ENTER();                                  //关中断
    p_pend_list = &p_q->PendList;                          //获取消息队列的等待列表
    cnt         = p_pend_list->NbrEntries;                 //获取等待该队列的任务数
    nbr_tasks   = cnt;                                     //按照任务数目逐个处理
    switch (opt) {                                         //根据选项分类处理
        case OS_OPT_DEL_NO_PEND:                           //如果只在没有任务等待的情况下删除队列
             if (nbr_tasks == (OS_OBJ_QTY)0) {             //如果没有任务在等待该队列
#if OS_CFG_DBG_EN > 0u                                     //如果使能了调试代码和变量  
                 OS_QDbgListRemove(p_q);                   //将该队列从消息队列调试列表移除
#endif
                 OSQQty--;                                 //消息队列数目减1
                 OS_QClr(p_q);                             //清除该队列的内容
                 CPU_CRITICAL_EXIT();                      //开中断
                *p_err = OS_ERR_NONE;                      //错误类型为“无错误”
             } else {                                      //如果有任务在等待该队列
                 CPU_CRITICAL_EXIT();                      //开中断
                *p_err = OS_ERR_TASK_WAITING;              //错误类型为“有任务在等待该队列”
             }
             break;

        case OS_OPT_DEL_ALWAYS:                             //如果必须删除信号量
             OS_CRITICAL_ENTER_CPU_EXIT();                  //进入临界段，重开中断
             ts = OS_TS_GET();                              //获取时间戳
             while (cnt > 0u) {                             //逐个移除该队列等待列表中的任务
                 p_pend_data = p_pend_list->HeadPtr;
                 p_tcb       = p_pend_data->TCBPtr;
                 OS_PendObjDel((OS_PEND_OBJ *)((void *)p_q),
                               p_tcb,
                               ts);
                 cnt--;
             }
#if OS_CFG_DBG_EN > 0u                                     //如果使能了调试代码和变量 
             OS_QDbgListRemove(p_q);                       //将该队列从消息队列调试列表移除
#endif
             OSQQty--;                                     //消息队列数目减1
             OS_QClr(p_q);                                 //清除消息队列内容
             OS_CRITICAL_EXIT_NO_SCHED();                  //退出临界段（无调度）
             OSSched();                                    //调度任务
            *p_err = OS_ERR_NONE;                          //错误类型为“无错误”
             break;                                        //跳出

        default:                                           //如果选项超出预期
             CPU_CRITICAL_EXIT();                          //开中断
            *p_err = OS_ERR_OPT_INVALID;                   //错误类型为“选项非法”
             break;                                        //跳出
    }
    return (nbr_tasks);                                    //返回删除队列前等待其的任务数
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                     FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments   : p_q        is a pointer to the message queue to flush
*
*               p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                              OS_ERR_NONE           upon success
*                              OS_ERR_FLUSH_ISR      if you called this function from an ISR
*                              OS_ERR_OBJ_PTR_NULL   If you passed a NULL pointer for 'p_q'
*                              OS_ERR_OBJ_TYPE       If you didn't create the message queue
*
* Returns     : The number of entries freed from the queue
*
* Note(s)     : 1) You should use this function with great care because, when to flush the queue, you LOOSE the
*                  references to what the queue entries are pointing to and thus, you could cause 'memory leaks'.  In
*                  other words, the data you are pointing to that's being referenced by the queue entries should, most
*                  likely, need to be de-allocated (i.e. freed).
************************************************************************************************************************
*/

#if OS_CFG_Q_FLUSH_EN > 0u             //如果使能了 OSQFlush() 函数
OS_MSG_QTY  OSQFlush (OS_Q    *p_q,    //消息队列指针
                      OS_ERR  *p_err)  //返回错误类型
{
    OS_MSG_QTY  entries;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return ((OS_MSG_QTY)0);         //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //如果该函数在中断中被调用
       *p_err = OS_ERR_FLUSH_ISR;               //错误类型为“在中断中请空队列”
        return ((OS_MSG_QTY)0);                 //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u            //如果使能了参数检测
    if (p_q == (OS_Q *)0) {           //如果 p_q 为空
       *p_err = OS_ERR_OBJ_PTR_NULL;  //错误类型为“对象为空”
        return ((OS_MSG_QTY)0);       //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u       //如果使能了对象类型检测
    if (p_q->Type != OS_OBJ_TYPE_Q) { //如果 p_q 不是消息队列类型  
       *p_err = OS_ERR_OBJ_TYPE;      //错误类型为“对象类型有误”
        return ((OS_MSG_QTY)0);       //返回0（有错误），停止执行
    }
#endif

    OS_CRITICAL_ENTER();                  //进入临界段
    entries = OS_MsgQFreeAll(&p_q->MsgQ); //把队列的所有消息均释放回消息池
    OS_CRITICAL_EXIT();                   //退出临界段
   *p_err   = OS_ERR_NONE;                //错误类型为“无错误”
    return ((OS_MSG_QTY)entries);         //返回清空队列前队列的消息数目
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                            PEND ON A QUEUE FOR A MESSAGE
*
* Description: This function waits for a message to be sent to a queue
*
* Arguments  : p_q           is a pointer to the message queue
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for a
*                            message to arrive at the queue up to the amount of time specified by this argument.  If you
*                            specify 0, however, your task will wait forever at the specified queue or, until a message
*                            arrives.
*
*              opt           determines whether the user wants to block if the queue is empty or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_msg_size    is a pointer to a variable that will receive the size of the message
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the message was
*                            received, pend aborted or the message queue deleted,  If you pass a NULL pointer (i.e.
*                            (CPU_TS *)0) then you will not get the timestamp.  In other words, passing a NULL pointer
*                            is valid and indicates that you don't need the timestamp.
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE               The call was successful and your task received a message.
*                                OS_ERR_OBJ_PTR_NULL       if you pass a NULL pointer for 'p_q'
*                                OS_ERR_OBJ_TYPE           if the message queue was not created
*                                OS_ERR_PEND_ABORT         the pend was aborted
*                                OS_ERR_PEND_ISR           if you called this function from an ISR
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the queue was not empty
*                                OS_ERR_SCHED_LOCKED       the scheduler is locked
*                                OS_ERR_TIMEOUT            A message was not received within the specified timeout
*                                                          would lead to a suspension.
*
* Returns    : != (void *)0  is a pointer to the message received
*              == (void *)0  if you received a NULL pointer message or,
*                            if no message was received or,
*                            if 'p_q' is a NULL pointer or,
*                            if you didn't pass a pointer to a queue.
************************************************************************************************************************
*/

void  *OSQPend (OS_Q         *p_q,       //消息队列指针
                OS_TICK       timeout,   //等待期限（单位：时钟节拍）
                OS_OPT        opt,       //选项
                OS_MSG_SIZE  *p_msg_size,//返回消息大小（单位：字节）
                CPU_TS       *p_ts,      //获取等到消息时的时间戳
                OS_ERR       *p_err)     //返回错误类型
{
    OS_PEND_DATA  pend_data;
    void         *p_void;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return ((void *)0);             //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
       *p_err = OS_ERR_PEND_ISR;               //错误类型为“在中断中中止等待”
        return ((void *)0);                    //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //如果使能了参数检测
    if (p_q == (OS_Q *)0) {               //如果 p_q 为空
       *p_err = OS_ERR_OBJ_PTR_NULL;      //错误类型为“对象为空”
        return ((void *)0);               //返回0（有错误），停止执行
    }
    if (p_msg_size == (OS_MSG_SIZE *)0) { //如果 p_msg_size 为空
       *p_err = OS_ERR_PTR_INVALID;       //错误类型为“指针不可用”
        return ((void *)0);               //返回0（有错误），停止执行
    }
    switch (opt) {                        //根据选项分类处理
        case OS_OPT_PEND_BLOCKING:        //如果选项在预期内
        case OS_OPT_PEND_NON_BLOCKING:
             break;                       //直接跳出

        default:                          //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;  //返回错误类型为“选项非法”
             return ((void *)0);          //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u        //如果使能了对象类型检测
    if (p_q->Type != OS_OBJ_TYPE_Q) {  //如果 p_q 不是消息队列类型
       *p_err = OS_ERR_OBJ_TYPE;       //错误类型为“对象类型有误”
        return ((void *)0);            //返回0（有错误），停止执行
    }
#endif

    if (p_ts != (CPU_TS *)0) {  //如果 p_ts 非空
       *p_ts  = (CPU_TS  )0;    //初始化（清零）p_ts，待用于返回时间戳
    }

    CPU_CRITICAL_ENTER();                                 //关中断
    p_void = OS_MsgQGet(&p_q->MsgQ,                       //从消息队列获取一个消息
                        p_msg_size,
                        p_ts,
                        p_err);
    if (*p_err == OS_ERR_NONE) {                          //如果获取消息成功
        CPU_CRITICAL_EXIT();                              //开中断
        return (p_void);                                  //返回消息内容
    }
    /* 如果获取消息不成功 */
    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {  //如果选择了不堵塞任务
        CPU_CRITICAL_EXIT();                              //开中断
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                  //错误类型为“等待渴求堵塞”
        return ((void *)0);                               //返回0（有错误），停止执行
    } else {                                              //如果选择了堵塞任务
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {  //如果调度器被锁
            CPU_CRITICAL_EXIT();                          //开中断
           *p_err = OS_ERR_SCHED_LOCKED;                  //错误类型为“调度器被锁”
            return ((void *)0);                           //返回0（有错误），停止执行
        }
    }
    /* 如果调度器未被锁 */                                
    OS_CRITICAL_ENTER_CPU_EXIT();                         //锁调度器，重开中断
    OS_Pend(&pend_data,                                   //堵塞当前任务，等待消息队列， 
            (OS_PEND_OBJ *)((void *)p_q),                 //将当前任务脱离就绪列表，并
            OS_TASK_PEND_ON_Q,                            //插入到节拍列表和等待列表。
            timeout);
    OS_CRITICAL_EXIT_NO_SCHED();                          //开调度器，但不进行调度

    OSSched();                                            //找到并调度最高优先级就绪任务
    /* 当前任务（获得消息队列的消息）得以继续运行 */
    CPU_CRITICAL_ENTER();                                 //关中断
    switch (OSTCBCurPtr->PendStatus) {                    //根据当前运行任务的等待状态分类处理
        case OS_STATUS_PEND_OK:                           //如果等待状态正常  
             p_void     = OSTCBCurPtr->MsgPtr;            //从（发布时放于）任务控制块提取消息
            *p_msg_size = OSTCBCurPtr->MsgSize;           //提取消息大小
             if (p_ts  != (CPU_TS *)0) {                  //如果 p_ts 非空
                *p_ts   =  OSTCBCurPtr->TS;               //获取任务等到消息时的时间戳
             }
            *p_err      = OS_ERR_NONE;                    //错误类型为“无错误”
             break;                                       //跳出

        case OS_STATUS_PEND_ABORT:                        //如果等待被中止
             p_void     = (void      *)0;                 //返回消息内容为空
            *p_msg_size = (OS_MSG_SIZE)0;                 //返回消息大小为0
             if (p_ts  != (CPU_TS *)0) {                  //如果 p_ts 非空
                *p_ts   =  OSTCBCurPtr->TS;               //获取等待被中止时的时间戳
             }
            *p_err      = OS_ERR_PEND_ABORT;              //错误类型为“等待被中止”
             break;                                       //跳出

        case OS_STATUS_PEND_TIMEOUT:                      //如果等待超时
             p_void     = (void      *)0;                 //返回消息内容为空
            *p_msg_size = (OS_MSG_SIZE)0;                 //返回消息大小为0
             if (p_ts  != (CPU_TS *)0) {                  //如果 p_ts 非空
                *p_ts   = (CPU_TS  )0;                    //清零 p_ts
             }
            *p_err      = OS_ERR_TIMEOUT;                 //错误类型为“等待超时”
             break;                                       //跳出

        case OS_STATUS_PEND_DEL:                          //如果等待的内核对象被删除
             p_void     = (void      *)0;                 //返回消息内容为空
            *p_msg_size = (OS_MSG_SIZE)0;                 //返回消息大小为0
             if (p_ts  != (CPU_TS *)0) {                  //如果 p_ts 非空
                *p_ts   =  OSTCBCurPtr->TS;               //获取对象被删时的时间戳
             }
            *p_err      = OS_ERR_OBJ_DEL;                 //错误类型为“等待对象被删”
             break;                                       //跳出

        default:                                          //如果等待状态超出预期
             p_void     = (void      *)0;                 //返回消息内容为空
            *p_msg_size = (OS_MSG_SIZE)0;                 //返回消息大小为0
            *p_err      = OS_ERR_STATUS_INVALID;          //错误类型为“状态非法”
             break;                                       //跳出
    }
    CPU_CRITICAL_EXIT();                                  //开中断
    return (p_void);                                      //返回消息内容
}


/*$PAGE*/
/*
************************************************************************************************************************
*                                             ABORT WAITING ON A MESSAGE QUEUE
*
* Description: This function aborts & readies any tasks currently waiting on a queue.  This function should be used to
*              fault-abort the wait on the queue, rather than to normally signal the queue via OSQPost().
*
* Arguments  : p_q       is a pointer to the message queue
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the queue
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the queue
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the queue was readied and
*                                                         informed of the aborted wait; check return value for the
*                                                         number of tasks whose wait on the queue was aborted.
*                            OS_ERR_OPT_INVALID           if you specified an invalid option
*                            OS_ERR_OBJ_PTR_NULL          if you pass a NULL pointer for 'p_q'
*                            OS_ERR_OBJ_TYPE              if the message queue was not created
*                            OS_ERR_PEND_ABORT_ISR        If this function was called from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the queue, or upon error.
*              >  0          if one or more tasks waiting on the queue are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_Q_PEND_ABORT_EN > 0u            //如果使能了 OSQPendAbort() 函数
OS_OBJ_QTY  OSQPendAbort (OS_Q    *p_q,    //消息队列
                          OS_OPT   opt,    //选项
                          OS_ERR  *p_err)  //返回错误类型
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    OS_OBJ_QTY     nbr_tasks;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {          //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();  //执行安全检测异常函数
        return ((OS_OBJ_QTY)0u);         //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u           //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) {  //如果该函数在中断中被调用
       *p_err =  OS_ERR_PEND_ABORT_ISR;          //错误类型为“在中断中中止等待”
        return ((OS_OBJ_QTY)0u);                 //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                                  //返回0（有错误），停止执行
    if (p_q == (OS_Q *)0) {                                 //如果 p_q 为空
       *p_err =  OS_ERR_OBJ_PTR_NULL;                       //错误类型为“对象为空”
        return ((OS_OBJ_QTY)0u);                            //返回0（有错误），停止执行
    }
    switch (opt) {                                          //根据选项分类处理
        case OS_OPT_PEND_ABORT_1:                           //如果选项在预期之内
        case OS_OPT_PEND_ABORT_ALL:
        case OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED:
             break;                                         //直接跳出

        default:                                            //如果选项超出预期
            *p_err =  OS_ERR_OPT_INVALID;                   //错误类型为“选项非法”
             return ((OS_OBJ_QTY)0u);                       //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u        //如果使能了对象类型检测
    if (p_q->Type != OS_OBJ_TYPE_Q) {  //如果 p_q 不是消息队列类型
       *p_err =  OS_ERR_OBJ_TYPE;      //错误类型为“对象类型有误”
        return ((OS_OBJ_QTY)0u);       //返回0（有错误），停止执行
    }
#endif

    CPU_CRITICAL_ENTER();                            //关中断
    p_pend_list = &p_q->PendList;                    //获取消息队列的等待列表
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0u) { //如果没有任务在等待
        CPU_CRITICAL_EXIT();                         //开中断
       *p_err =  OS_ERR_PEND_ABORT_NONE;             //错误类型为“没任务在等待”
        return ((OS_OBJ_QTY)0u);
    }
    /* 如果有任务在等待 */
    OS_CRITICAL_ENTER_CPU_EXIT();                     //锁调度器，重开中断   
    nbr_tasks = 0u;                                   //准备计数被中止的等待任务
    ts        = OS_TS_GET();                          //获取时间戳
    while (p_pend_list->NbrEntries > (OS_OBJ_QTY)0u) {//如果还有任务在等待
        p_tcb = p_pend_list->HeadPtr->TCBPtr;         //获取头端（最高优先级）任务
        OS_PendAbort((OS_PEND_OBJ *)((void *)p_q),    //中止该任务对 p_q 的等待
                     p_tcb,
                     ts);
        nbr_tasks++;
        if (opt != OS_OPT_PEND_ABORT_ALL) {           //如果不是选择了中止所有等待任务
            break;                                    //立即跳出，不再继续中止
        }
    }
    OS_CRITICAL_EXIT_NO_SCHED();                      //减锁调度器，但不调度

    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0u) { //如果选择了任务调度
        OSSched();                                    //进行任务调度
    }

   *p_err = OS_ERR_NONE;                              //错误类型为“无错误”
    return (nbr_tasks);                               //返回被中止的任务数目
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                               POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue.  With the 'opt' argument, you can specify whether the message
*              is broadcast to all waiting tasks and/or whether you post the message to the front of the queue (LIFO)
*              or normally (FIFO) at the end of the queue.
*
* Arguments  : p_q           is a pointer to a message queue that must have been created by OSQCreate().
*
*              p_void        is a pointer to the message to send.
*
*              msg_size      specifies the size of the message (in bytes)
*
*              opt           determines the type of POST performed:
*
*                                OS_OPT_POST_ALL          POST to ALL tasks that are waiting on the queue.  This option
*                                                         can be added to either OS_OPT_POST_FIFO or OS_OPT_POST_LIFO
*                                OS_OPT_POST_FIFO         POST message to end of queue (FIFO) and wake up a single
*                                                         waiting task.
*                                OS_OPT_POST_LIFO         POST message to the front of the queue (LIFO) and wake up
*                                                         a single waiting task.
*                                OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*                            Note(s): 1) OS_OPT_POST_NO_SCHED can be added (or OR'd) with one of the other options.
*                                     2) OS_OPT_POST_ALL      can be added (or OR'd) with one of the other options.
*                                     3) Possible combination of options are:
*
*                                        OS_OPT_POST_FIFO
*                                        OS_OPT_POST_LIFO
*                                        OS_OPT_POST_FIFO + OS_OPT_POST_ALL
*                                        OS_OPT_POST_LIFO + OS_OPT_POST_ALL
*                                        OS_OPT_POST_FIFO + OS_OPT_POST_NO_SCHED
*                                        OS_OPT_POST_LIFO + OS_OPT_POST_NO_SCHED
*                                        OS_OPT_POST_FIFO + OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED
*                                        OS_OPT_POST_LIFO + OS_OPT_POST_ALL + OS_OPT_POST_NO_SCHED
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE            The call was successful and the message was sent
*                                OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs to use to place the message into
*                                OS_ERR_OBJ_PTR_NULL    If 'p_q' is a NULL pointer
*                                OS_ERR_OBJ_TYPE        If the message queue was not initialized
*                                OS_ERR_Q_MAX           If the queue is full
*
* Returns    : None
************************************************************************************************************************
*/

void  OSQPost (OS_Q         *p_q,      //消息队列指针
               void         *p_void,   //消息指针
               OS_MSG_SIZE   msg_size, //消息大小（单位：字节）
               OS_OPT        opt,      //选项 
               OS_ERR       *p_err)    //返回错误类型
{
    CPU_TS  ts;



#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return;                         //返回，停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u             //如果使能了参数检测
    if (p_q == (OS_Q *)0) {            //如果 p_q 为空  
       *p_err = OS_ERR_OBJ_PTR_NULL;   //错误类型为“内核对象为空”
        return;                        //返回，停止执行
    }
    switch (opt) {                         //根据选项分类处理
        case OS_OPT_POST_FIFO:             //如果选项在预期内
        case OS_OPT_POST_LIFO:
        case OS_OPT_POST_FIFO | OS_OPT_POST_ALL:
        case OS_OPT_POST_LIFO | OS_OPT_POST_ALL:
        case OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_LIFO | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_FIFO | OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_LIFO | OS_OPT_POST_ALL | OS_OPT_POST_NO_SCHED:
             break;                       //直接跳出

        default:                          //如果选项超出预期
            *p_err =  OS_ERR_OPT_INVALID; //错误类型为“选项非法”
             return;                      //返回，停止执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u       //如果使能了对象类型检测  
    if (p_q->Type != OS_OBJ_TYPE_Q) { //如果 p_q 不是消息队列类型 
       *p_err = OS_ERR_OBJ_TYPE;      //错误类型为“对象类型错误”
        return;                       //返回，停止执行
    }
#endif

    ts = OS_TS_GET();                 //获取时间戳

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u            //如果使能了中断延迟发布
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //如果该函数在中断中被调用
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_Q, //将该消息发布到中断消息队列
                    (void      *)p_q,
                    (void      *)p_void,
                    (OS_MSG_SIZE)msg_size,
                    (OS_FLAGS   )0,
                    (OS_OPT     )opt,
                    (CPU_TS     )ts,
                    (OS_ERR    *)p_err);
        return;                                //返回（尚未发布），停止执行  
    }
#endif

    OS_QPost(p_q,                              //将消息按照普通方式
             p_void,
             msg_size,
             opt,
             ts,
             p_err);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                        CLEAR THE CONTENTS OF A MESSAGE QUEUE
*
* Description: This function is called by OSQDel() to clear the contents of a message queue
*

* Argument(s): p_q      is a pointer to the queue to clear
*              ---
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_QClr (OS_Q  *p_q)
{
    (void)OS_MsgQFreeAll(&p_q->MsgQ);                       /* Return all OS_MSGs to the free list                    */
    p_q->Type    =  OS_OBJ_TYPE_NONE;                       /* Mark the data structure as a NONE                      */
    p_q->NamePtr = (CPU_CHAR *)((void *)"?Q");
    OS_MsgQInit(&p_q->MsgQ,                                 /* Initialize the list of OS_MSGs                         */
                0u);
    OS_PendListInit(&p_q->PendList);                        /* Initialize the waiting list                            */
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                      ADD/REMOVE MESSAGE QUEUE TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove a message queue to/from a message queue debug
*              list.
*
* Arguments  : p_q     is a pointer to the message queue to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/


#if OS_CFG_DBG_EN > 0u             //如果使能（默认使能）了调试代码和变量
void  OS_QDbgListAdd (OS_Q  *p_q)  //将消息队列插入到消息队列列表的最前端
{
    p_q->DbgNamePtr               = (CPU_CHAR *)((void *)" "); //先不指向任何任务的名称
    p_q->DbgPrevPtr               = (OS_Q     *)0;             //将该队列作为列表的最前端
    if (OSQDbgListPtr == (OS_Q *)0) {                          //如果列表为空
        p_q->DbgNextPtr           = (OS_Q     *)0;             //该队列的下一个元素也为空
    } else {                                                   //如果列表非空
        p_q->DbgNextPtr           =  OSQDbgListPtr;            //列表原来的首元素作为该队列的下一个元素
        OSQDbgListPtr->DbgPrevPtr =  p_q;                      //原来的首元素的前面变为了该队列
    }
    OSQDbgListPtr                 =  p_q;                      //该信号量成为列表的新首元素
}



void  OS_QDbgListRemove (OS_Q  *p_q)
{
    OS_Q  *p_q_next;
    OS_Q  *p_q_prev;


    p_q_prev = p_q->DbgPrevPtr;
    p_q_next = p_q->DbgNextPtr;

    if (p_q_prev == (OS_Q *)0) {
        OSQDbgListPtr = p_q_next;
        if (p_q_next != (OS_Q *)0) {
            p_q_next->DbgPrevPtr = (OS_Q *)0;
        }
        p_q->DbgNextPtr = (OS_Q *)0;

    } else if (p_q_next == (OS_Q *)0) {
        p_q_prev->DbgNextPtr = (OS_Q *)0;
        p_q->DbgPrevPtr      = (OS_Q *)0;

    } else {
        p_q_prev->DbgNextPtr =  p_q_next;
        p_q_next->DbgPrevPtr =  p_q_prev;
        p_q->DbgNextPtr      = (OS_Q *)0;
        p_q->DbgPrevPtr      = (OS_Q *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                              MESSAGE QUEUE INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the message queue management.
*

* Arguments  : p_err         is a pointer to a variable that will receive an error code.
*
*                                OS_ERR_NONE     the call was successful
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application MUST NOT call it.
************************************************************************************************************************
*/

void  OS_QInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSQDbgListPtr = (OS_Q *)0;
#endif

    OSQQty        = (OS_OBJ_QTY)0;
   *p_err         = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue.  With the 'opt' argument, you can specify whether the message
*              is broadcast to all waiting tasks and/or whether you post the message to the front of the queue (LIFO)
*              or normally (FIFO) at the end of the queue.
*
* Arguments  : p_q           is a pointer to a message queue that must have been created by OSQCreate().
*
*              p_void        is a pointer to the message to send.
*
*              msg_size      specifies the size of the message (in bytes)
*
*              opt           determines the type of POST performed:
*
*                                OS_OPT_POST_ALL          POST to ALL tasks that are waiting on the queue
*
*                                OS_OPT_POST_FIFO         POST as FIFO and wake up single waiting task
*                                OS_OPT_POST_LIFO         POST as LIFO and wake up single waiting task
*
*                                OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*              ts            is the timestamp of the post
*
*              p_err         is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE            The call was successful and the message was sent
*                                OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs to use to place the message into
*                                OS_ERR_OBJ_PTR_NULL    If 'p_q' is a NULL pointer
*                                OS_ERR_OBJ_TYPE        If the message queue was not initialized
*                                OS_ERR_Q_MAX           If the queue is full
*
* Returns    : None
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_QPost (OS_Q         *p_q,      //消息队列指针
                void         *p_void,   //消息指针
                OS_MSG_SIZE   msg_size, //消息大小（单位：字节）
                OS_OPT        opt,      //选项
                CPU_TS        ts,       //消息被发布时的时间戳
                OS_ERR       *p_err)    //返回错误类型
{
    OS_OBJ_QTY     cnt;
    OS_OPT         post_type;
    OS_PEND_LIST  *p_pend_list;
    OS_PEND_DATA  *p_pend_data;
    OS_PEND_DATA  *p_pend_data_next;
    OS_TCB        *p_tcb;
    CPU_SR_ALLOC();  //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
									   //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
									   // SR（临界段关中断只需保存SR），开中断时将该值还原。

    OS_CRITICAL_ENTER();                              //进入临界段
    p_pend_list = &p_q->PendList;                     //取出该队列的等待列表
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0) {   //如果没有任务在等待该队列
        if ((opt & OS_OPT_POST_LIFO) == (OS_OPT)0) {  //把消息发布到队列的末端
            post_type = OS_OPT_POST_FIFO;
        } else {                                      //把消息发布到队列的前端
            post_type = OS_OPT_POST_LIFO;
        }
        OS_MsgQPut(&p_q->MsgQ,                        //把消息放入消息队列          /* Place message in the message queue                     */
                   p_void,
                   msg_size,
                   post_type,
                   ts,
                   p_err);
        OS_CRITICAL_EXIT();                          //退出临界段
        return;                                      //返回，执行完毕
    }
    /* 如果有任务在等待该队列 */
    if ((opt & OS_OPT_POST_ALL) != (OS_OPT)0) {     //如果要把消息发布给所有等待任务
        cnt = p_pend_list->NbrEntries;              //获取等待任务数目
    } else {                                        //如果要把消息发布给一个等待任务
        cnt = (OS_OBJ_QTY)1;                        //要处理的任务数目为1
    }
    p_pend_data = p_pend_list->HeadPtr;             //获取等待列表的头部（任务）
    while (cnt > 0u) {                              //根据要发布的任务数目逐个发布
        p_tcb            = p_pend_data->TCBPtr;
        p_pend_data_next = p_pend_data->NextPtr;
        OS_Post((OS_PEND_OBJ *)((void *)p_q),       //把消息发布给任务
                p_tcb,
                p_void,
                msg_size,
                ts);
        p_pend_data = p_pend_data_next;
        cnt--;
    }
    OS_CRITICAL_EXIT_NO_SCHED();                    //退出临界段（无调度）
    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) {//如果没选择“发布完不调度任务”
        OSSched();                                  //任务调度
    }
   *p_err = OS_ERR_NONE;                            //错误类型为“无错误”
}

#endif
