/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                 ISR QUEUE MANAGEMENT
*
* File    : OS_INT.C
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

#define   MICRIUM_SOURCE
#include  <os.h>

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_int__c = "$Id: $";
#endif


#if OS_CFG_ISR_POST_DEFERRED_EN > 0u
/*$PAGE*/
/*
************************************************************************************************************************
*                                                   POST TO ISR QUEUE
*
* Description: This function places contents of posts into an intermediate queue to help defer processing of interrupts
*              at the task level.
*
* Arguments  : type       is the type of kernel object the post is destined to:
*
*                             OS_OBJ_TYPE_SEM
*                             OS_OBJ_TYPE_Q
*                             OS_OBJ_TYPE_FLAG
*                             OS_OBJ_TYPE_TASK_MSG
*                             OS_OBJ_TYPE_TASK_SIGNAL
*
*              p_obj      is a pointer to the kernel object to post to.  This can be a pointer to a semaphore,
*              -----      a message queue or a task control clock.
*
*              p_void     is a pointer to a message that is being posted.  This is used when posting to a message
*                         queue or directly to a task.
*
*              msg_size   is the size of the message being posted
*
*              flags      if the post is done to an event flag group then this corresponds to the flags being
*                         posted
*
*              ts         is a timestamp as to when the post was done
*
*              opt        this corresponds to post options and applies to:
*
*                             OSFlagPost()
*                             OSSemPost()
*                             OSQPost()
*                             OSTaskQPost()
*
*              p_err      is a pointer to a variable that will contain an error code returned by this function.
*
*                             OS_ERR_NONE         if the post to the ISR queue was successful
*                             OS_ERR_INT_Q_FULL   if the ISR queue is full and cannot accepts any further posts.  This
*                                                 generally indicates that you are receiving interrupts faster than you
*                                                 can process them or, that you didn't make the ISR queue large enough.
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OS_IntQPost (OS_OBJ_TYPE   type,        //内核对象类型
                   void         *p_obj,       //被发布的内核对象
                   void         *p_void,      //消息队列或任务消息
                   OS_MSG_SIZE   msg_size,    //消息的数目
                   OS_FLAGS      flags,       //事件标志组
                   OS_OPT        opt,         //发布内核对象时的选项
                   CPU_TS        ts,          //发布内核对象时的时间戳
                   OS_ERR       *p_err)       //返回错误类型
{
    CPU_SR_ALLOC();  //使用到临界段（在关/开中断时）时必需该宏，该宏声明和定义一个局部变
                     //量，用于保存关中断前的 CPU 状态寄存器 SR（临界段关中断只需保存SR）
                     //，开中断时将该值还原。 

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return;                         //返回，不继续执行
    }
#endif

    CPU_CRITICAL_ENTER();                                   //关中断
    if (OSIntQNbrEntries < OSCfg_IntQSize) {                //如果中断队列未占满   
        OSIntQNbrEntries++;

        if (OSIntQNbrEntriesMax < OSIntQNbrEntries) {       //更新中断队列的最大使用数目的历史记录
            OSIntQNbrEntriesMax = OSIntQNbrEntries;
        }
        /* 将要重新提交的内核对象的信息放入到中断队列入口的信息记录块 */
        OSIntQInPtr->Type       = type;                     //保存内核对象类型
        OSIntQInPtr->ObjPtr     = p_obj;                    //保存被发布的内核对象
        OSIntQInPtr->MsgPtr     = p_void;                   //保存消息内容指针
        OSIntQInPtr->MsgSize    = msg_size;                 //保存消息大小
        OSIntQInPtr->Flags      = flags;                    //保存事件标志组
        OSIntQInPtr->Opt        = opt;                      //保存选项
        OSIntQInPtr->TS         = ts;                       //保存对象被发布的时间错

        OSIntQInPtr             =  OSIntQInPtr->NextPtr;    //指向下一个带处理成员
        /* 让中断队列管理任务 OSIntQTask 就绪 */
        OSRdyList[0].NbrEntries = (OS_OBJ_QTY)1;            //更新就绪列表上的优先级0的任务数为1个 
        OSRdyList[0].HeadPtr    = &OSIntQTaskTCB;           //该就绪列表的头指针指向 OSIntQTask 任务
        OSRdyList[0].TailPtr    = &OSIntQTaskTCB;           //该就绪列表的尾指针指向 OSIntQTask 任务
        OS_PrioInsert(0u);                                  //在优先级列表中增加优先级0
        if (OSPrioCur != 0) {                               //如果当前运行的不是 OSIntQTask 任务
            OSPrioSaved         = OSPrioCur;                //保存当前任务的优先级
        }

       *p_err                   = OS_ERR_NONE;              //返回错误类型为“无错误”
    } else {                                                //如果中断队列已占满
        OSIntQOvfCtr++;                                     //中断队列溢出数目加1
       *p_err                   = OS_ERR_INT_Q_FULL;        //返回错误类型为“无错误”
    }
    CPU_CRITICAL_EXIT();                                    //开中断
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               INTERRUPT QUEUE MANAGEMENT TASK
*
* Description: This task is created by OS_IntQTaskInit().
*
* Arguments  : p_arg     is a pointer to an optional argument that is passed during task creation.  For this function
*                        the argument is not used and will be a NULL pointer.
*
* Returns    : none
************************************************************************************************************************
*/

void  OS_IntQRePost (void)
{
    CPU_TS  ts;
    OS_ERR  err;


    switch (OSIntQOutPtr->Type) {   //根据内核对象类型分类处理
        case OS_OBJ_TYPE_FLAG:      //如果对象类型是事件标志
#if OS_CFG_FLAG_EN > 0u             //如果使能了事件标志，则发布事件标志
             (void)OS_FlagPost((OS_FLAG_GRP *) OSIntQOutPtr->ObjPtr,
                               (OS_FLAGS     ) OSIntQOutPtr->Flags,
                               (OS_OPT       ) OSIntQOutPtr->Opt,
                               (CPU_TS       ) OSIntQOutPtr->TS,
                               (OS_ERR      *)&err);
#endif
             break;                 //跳出

        case OS_OBJ_TYPE_Q:         //如果对象类型是消息队列
#if OS_CFG_Q_EN > 0u                //如果使能了消息队列，则发布消息队列
             OS_QPost((OS_Q      *) OSIntQOutPtr->ObjPtr,
                      (void      *) OSIntQOutPtr->MsgPtr,
                      (OS_MSG_SIZE) OSIntQOutPtr->MsgSize,
                      (OS_OPT     ) OSIntQOutPtr->Opt,
                      (CPU_TS     ) OSIntQOutPtr->TS,
                      (OS_ERR    *)&err);
#endif
             break;                 //跳出

        case OS_OBJ_TYPE_SEM:       //如果对象类型是多值信号量
#if OS_CFG_SEM_EN > 0u              //如果使能了多值信号量，则发布多值信号量
             (void)OS_SemPost((OS_SEM *) OSIntQOutPtr->ObjPtr,
                              (OS_OPT  ) OSIntQOutPtr->Opt,
                              (CPU_TS  ) OSIntQOutPtr->TS,
                              (OS_ERR *)&err);
#endif
             break;                 //跳出

        case OS_OBJ_TYPE_TASK_MSG:  //如果对象类型是任务消息
#if OS_CFG_TASK_Q_EN > 0u           //如果使能了任务消息，则发布任务消息
             OS_TaskQPost((OS_TCB    *) OSIntQOutPtr->ObjPtr,
                          (void      *) OSIntQOutPtr->MsgPtr,
                          (OS_MSG_SIZE) OSIntQOutPtr->MsgSize,
                          (OS_OPT     ) OSIntQOutPtr->Opt,
                          (CPU_TS     ) OSIntQOutPtr->TS,
                          (OS_ERR    *)&err);
#endif
             break;                 //跳出

        case OS_OBJ_TYPE_TASK_RESUME://如果对象类型是恢复任务
#if OS_CFG_TASK_SUSPEND_EN > 0u      //如果使能了函数OSTaskResume()，恢复该任务
             (void)OS_TaskResume((OS_TCB *) OSIntQOutPtr->ObjPtr,
                                 (OS_ERR *)&err);
#endif
             break;                  //跳出

        case OS_OBJ_TYPE_TASK_SIGNAL://如果对象类型是任务信号量
             (void)OS_TaskSemPost((OS_TCB *) OSIntQOutPtr->ObjPtr,//发布任务信号量
                                  (OS_OPT  ) OSIntQOutPtr->Opt,
                                  (CPU_TS  ) OSIntQOutPtr->TS,
                                  (OS_ERR *)&err);
             break;                  //跳出

        case OS_OBJ_TYPE_TASK_SUSPEND://如果对象类型是挂起任务
#if OS_CFG_TASK_SUSPEND_EN > 0u       //如果使能了函数 OSTaskSuspend()，挂起该任务
             (void)OS_TaskSuspend((OS_TCB *) OSIntQOutPtr->ObjPtr,
                                  (OS_ERR *)&err);
#endif
             break;                   //跳出

        case OS_OBJ_TYPE_TICK:       //如果对象类型是时钟节拍
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u //如果使能了时间片轮转调度，
             OS_SchedRoundRobin(&OSRdyList[OSPrioSaved]); //轮转调度进中断前优先级任务
#endif

             (void)OS_TaskSemPost((OS_TCB *)&OSTickTaskTCB, //发送信号量给时钟节拍任务
                                  (OS_OPT  ) OS_OPT_POST_NONE,
                                  (CPU_TS  ) OSIntQOutPtr->TS,
                                  (OS_ERR *)&err);
#if OS_CFG_TMR_EN > 0u               //如果使能了软件定时器，发送信号量给定时器任务
             OSTmrUpdateCtr--;
             if (OSTmrUpdateCtr == (OS_CTR)0u) {
                 OSTmrUpdateCtr = OSTmrUpdateCnt;
                 ts             = OS_TS_GET();                             
                 (void)OS_TaskSemPost((OS_TCB *)&OSTmrTaskTCB,             
                                      (OS_OPT  ) OS_OPT_POST_NONE,
                                      (CPU_TS  ) ts,
                                      (OS_ERR *)&err);
             }
#endif
             break;                  //跳出

        default:                     //如果内核对象类型超出预期
             break;                  //直接跳出
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               INTERRUPT QUEUE MANAGEMENT TASK
*
* Description: This task is created by OS_IntQTaskInit().
*
* Arguments  : p_arg     is a pointer to an optional argument that is passed during task creation.  For this function
*                        the argument is not used and will be a NULL pointer.
*
* Returns    : none
************************************************************************************************************************
*/

void  OS_IntQTask (void  *p_arg)
{
    CPU_BOOLEAN  done;
    CPU_TS       ts_start;
    CPU_TS       ts_end;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

    p_arg = p_arg;                                          
    while (DEF_ON) {                                        //进入死循环
        done = DEF_FALSE;
        while (done == DEF_FALSE) {
            CPU_CRITICAL_ENTER();                           //关中断
            if (OSIntQNbrEntries == (OS_OBJ_QTY)0u) {       //如果中断队列里的内核对象发布完毕
                OSRdyList[0].NbrEntries = (OS_OBJ_QTY)0u;   //从就绪列表移除中断队列管理任务 OS_IntQTask
                OSRdyList[0].HeadPtr    = (OS_TCB   *)0;
                OSRdyList[0].TailPtr    = (OS_TCB   *)0;
                OS_PrioRemove(0u);                          //从优先级表格移除优先级0
                CPU_CRITICAL_EXIT();                        //开中断
                OSSched();                                  //任务调度
                done = DEF_TRUE;                            //退出循环
            } else {                                        //如果中断队列里还有内核对象
                CPU_CRITICAL_EXIT();                        //开中断
                ts_start = OS_TS_GET();                     //获取时间戳
                OS_IntQRePost();                            //发布中断队列里的内核对象
                ts_end   = OS_TS_GET() - ts_start;          //计算该次发布时间
                if (OSIntQTaskTimeMax < ts_end) {           //更新中断队列发布内核对象的最大时间的历史记录
                    OSIntQTaskTimeMax = ts_end;
                }
                CPU_CRITICAL_ENTER();                       //关中断
                OSIntQOutPtr = OSIntQOutPtr->NextPtr;       //中断队列出口转至下一个
                OSIntQNbrEntries--;                         //中断队列的成员数目减1
                CPU_CRITICAL_EXIT();                        //开中断
            }
        }
    }
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 INITIALIZE THE ISR QUEUE
*
* Description: This function is called by OSInit() to initialize the ISR queue.
*
* Arguments  : p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                           OS_ERR_INT_Q             If you didn't provide an ISR queue in OS_CFG.C
*                           OS_ERR_INT_Q_SIZE        If you didn't specify a large enough ISR queue.
*                           OS_ERR_STK_INVALID       If you specified a NULL pointer for the task of the ISR task
*                                                    handler
*                           OS_ERR_STK_SIZE_INVALID  If you didn't specify a stack size greater than the minimum
*                                                    specified by OS_CFG_STK_SIZE_MIN
*                           OS_ERR_???               An error code returned by OSTaskCreate().
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/

void  OS_IntQTaskInit (OS_ERR  *p_err)
{
    OS_INT_Q      *p_int_q;
    OS_INT_Q      *p_int_q_next;
    OS_OBJ_QTY     i;



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

    OSIntQOvfCtr = (OS_QTY)0u;                              /* Clear the ISR queue overflow counter                   */

    if (OSCfg_IntQBasePtr == (OS_INT_Q *)0) {
       *p_err = OS_ERR_INT_Q;
        return;
    }

    if (OSCfg_IntQSize < (OS_OBJ_QTY)2u) {
       *p_err = OS_ERR_INT_Q_SIZE;
        return;
    }

    OSIntQTaskTimeMax = (CPU_TS)0;

    p_int_q           = OSCfg_IntQBasePtr;                  /* Initialize the circular ISR queue                      */
    p_int_q_next      = p_int_q;
    p_int_q_next++;
    for (i = 0u; i < OSCfg_IntQSize; i++) {
        p_int_q->Type    =  OS_OBJ_TYPE_NONE;
        p_int_q->ObjPtr  = (void      *)0;
        p_int_q->MsgPtr  = (void      *)0;
        p_int_q->MsgSize = (OS_MSG_SIZE)0u;
        p_int_q->Flags   = (OS_FLAGS   )0u;
        p_int_q->Opt     = (OS_OPT     )0u;
        p_int_q->NextPtr = p_int_q_next;
        p_int_q++;
        p_int_q_next++;
    }
    p_int_q--;
    p_int_q_next        = OSCfg_IntQBasePtr;
    p_int_q->NextPtr    = p_int_q_next;
    OSIntQInPtr         = p_int_q_next;
    OSIntQOutPtr        = p_int_q_next;
    OSIntQNbrEntries    = (OS_OBJ_QTY)0u;
    OSIntQNbrEntriesMax = (OS_OBJ_QTY)0u;

                                                            /* -------------- CREATE THE ISR QUEUE TASK ------------- */
    if (OSCfg_IntQTaskStkBasePtr == (CPU_STK *)0) {
       *p_err = OS_ERR_INT_Q_STK_INVALID;
        return;
    }

    if (OSCfg_IntQTaskStkSize < OSCfg_StkSizeMin) {
       *p_err = OS_ERR_INT_Q_STK_SIZE_INVALID;
        return;
    }

    OSTaskCreate((OS_TCB     *)&OSIntQTaskTCB,
                 (CPU_CHAR   *)((void *)"uC/OS-III ISR Queue Task"),
                 (OS_TASK_PTR )OS_IntQTask,
                 (void       *)0,
                 (OS_PRIO     )0u,                          /* This task is ALWAYS at priority '0' (i.e. highest)     */
                 (CPU_STK    *)OSCfg_IntQTaskStkBasePtr,
                 (CPU_STK_SIZE)OSCfg_IntQTaskStkLimit,
                 (CPU_STK_SIZE)OSCfg_IntQTaskStkSize,
                 (OS_MSG_QTY  )0u,
                 (OS_TICK     )0u,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)p_err);
}

#endif
