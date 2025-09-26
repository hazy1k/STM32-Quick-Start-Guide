/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                   TASK MANAGEMENT
*
* File    : OS_TASK.C
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
const  CPU_CHAR  *os_task__c = "$Id: $";
#endif

/*
************************************************************************************************************************
*                                                CHANGE PRIORITY OF A TASK
*
* Description: This function allows you to change the priority of a task dynamically.  Note that the new
*              priority MUST be available.
*
* Arguments  : p_tcb      is the TCB of the tack to change the priority for
*
*              prio_new   is the new priority
*
*              p_err      is a pointer to an error code returned by this function:
*
*                             OS_ERR_NONE                 is the call was successful
*                             OS_ERR_PRIO_INVALID         if the priority you specify is higher that the maximum allowed
*                                                         (i.e. >= (OS_CFG_PRIO_MAX-1))
*                             OS_ERR_STATE_INVALID        if the task is in an invalid state
*                             OS_ERR_TASK_CHANGE_PRIO_ISR if you tried to change the task's priority from an ISR
************************************************************************************************************************
*/

#if OS_CFG_TASK_CHANGE_PRIO_EN > 0u           //如果使能了函数 OSTaskChangePrio()
void  OSTaskChangePrio (OS_TCB   *p_tcb,      //目标任务的任务控制块指针
                        OS_PRIO   prio_new,   //新优先级
                        OS_ERR   *p_err)      //返回错误类型
{
    CPU_BOOLEAN   self;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                       //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {                 //如果 p_err 为空
        OS_SAFETY_CRITICAL_EXCEPTION();         //执行安全检测异常函数
        return;                                 //返回，停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //如果该函数在中断中被调用
       *p_err = OS_ERR_TASK_CHANGE_PRIO_ISR;    //错误类型为“在中断中改变优先级”
        return;                                 //返回，停止执行
    }
#endif

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u            //如果使能了中断延迟发布
    if (prio_new == 0) {                        //如果 prio_new 为0
       *p_err = OS_ERR_PRIO_INVALID;            //错误类型为“优先级非法”
        return;                                 //返回，停止执行
    }
#endif

    if (prio_new >= (OS_CFG_PRIO_MAX - 1u)) {   //如果 prio_new 超出限制范围
       *p_err = OS_ERR_PRIO_INVALID;            //错误类型为“优先级非法”
        return;                                 //返回，停止执行
    }

    if (p_tcb == (OS_TCB *)0) {                 //如果 p_tcb 为空
        CPU_CRITICAL_ENTER();                   //关中断
        p_tcb = OSTCBCurPtr;                    //目标任务为当前任务（自身）
        CPU_CRITICAL_EXIT();                    //开中断
        self  = DEF_TRUE;                       //目标任务是自身
    } else {                                    //如果 p_tcb 非空
        self  = DEF_FALSE;                      //目标任务不是自身
    }

    OS_CRITICAL_ENTER();                        //进入临界段
    switch (p_tcb->TaskState) {                 //根据目标任务状态分类处理
        case OS_TASK_STATE_RDY:                 //如果是就绪状态
             OS_RdyListRemove(p_tcb);           //将任务从就绪列表移除
             p_tcb->Prio = prio_new;            //为任务设置新优先级
             OS_PrioInsert(p_tcb->Prio);        //在优先级表格中将该优先级位置1
             if (self == DEF_TRUE) {            //如果目标任务是自身
                 OS_RdyListInsertHead(p_tcb);   //将目标任务插至就绪列表头端
             } else {                           //如果目标任务不是自身
                 OS_RdyListInsertTail(p_tcb);   //将目标任务插至就绪列表尾端
             }
             break;                             //跳出

        case OS_TASK_STATE_DLY:                 //如果是延时状态   
        case OS_TASK_STATE_SUSPENDED:           //如果是挂起状态
        case OS_TASK_STATE_DLY_SUSPENDED:       //如果是延时中被挂起状态
             p_tcb->Prio = prio_new;            //直接修改任务的优先级
             break;                             //跳出

        case OS_TASK_STATE_PEND:                //如果包含等待状态
        case OS_TASK_STATE_PEND_TIMEOUT:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             switch (p_tcb->PendOn) {          //根据任务等待的对象分类处理
                 case OS_TASK_PEND_ON_TASK_Q:  //如果等待的是任务消息
                 case OS_TASK_PEND_ON_TASK_SEM://如果等待的是任务信号量
                 case OS_TASK_PEND_ON_FLAG:    //如果等待的是事件标志组
                      p_tcb->Prio = prio_new;  //直接修改任务的优先级
                      break;                   //跳出

                 case OS_TASK_PEND_ON_MUTEX:   //如果等待的是互斥信号量
                 case OS_TASK_PEND_ON_MULTI:   //如果等待的是多个内核对象
                 case OS_TASK_PEND_ON_Q:       //如果等待的是消息队列
                 case OS_TASK_PEND_ON_SEM:     //如果等待的是多值信号量
                      OS_PendListChangePrio(p_tcb,    //改变任务的优先级和
                                            prio_new);//在等待列表中的位置。
                      break;                   //跳出
 
                 default:                      //如果任务等待的对象超出预期
                      break;                   //直接跳出
            }
             break;                            //跳出
 
        default:                               //如果目标任务状态超出预期
             OS_CRITICAL_EXIT();               //退出临界段
            *p_err = OS_ERR_STATE_INVALID;     //错误类型为“状态非法”
             return;                           //返回，停止执行
    }

    OS_CRITICAL_EXIT_NO_SCHED();               //退出临界段（无调度）

    OSSched();                                 //调度任务

   *p_err = OS_ERR_NONE;                       //错误类型为“无错误”
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    CREATE A TASK
*
* Description: This function is used to have uC/OS-III manage the execution of a task.  Tasks can either be created
*              prior to the start of multitasking or by a running task.  A task cannot be created by an ISR.
*
* Arguments  : p_tcb          is a pointer to the task's TCB
*
*              p_name         is a pointer to an ASCII string to provide a name to the task.
*
*              p_task         is a pointer to the task's code
*
*              p_arg          is a pointer to an optional data area which can be used to pass parameters to
*                             the task when the task first executes.  Where the task is concerned it thinks
*                             it was invoked and passed the argument 'p_arg' as follows:
*
*                                 void Task (void *p_arg)
*                                 {
*                                     for (;;) {
*                                         Task code;
*                                     }
*                                 }
*
*              prio           is the task's priority.  A unique priority MUST be assigned to each task and the
*                             lower the number, the higher the priority.
*
*              p_stk_base     is a pointer to the base address of the stack (i.e. low address).
*
*              stk_limit      is the number of stack elements to set as 'watermark' limit for the stack.  This value
*                             represents the number of CPU_STK entries left before the stack is full.  For example,
*                             specifying 10% of the 'stk_size' value indicates that the stack limit will be reached
*                             when the stack reaches 90% full.
*
*              stk_size       is the size of the stack in number of elements.  If CPU_STK is set to CPU_INT08U,
*                             'stk_size' corresponds to the number of bytes available.  If CPU_STK is set to
*                             CPU_INT16U, 'stk_size' contains the number of 16-bit entries available.  Finally, if
*                             CPU_STK is set to CPU_INT32U, 'stk_size' contains the number of 32-bit entries
*                             available on the stack.
*
*              q_size         is the maximum number of messages that can be sent to the task
*
*              time_quanta    amount of time (in ticks) for time slice when round-robin between tasks.  Specify 0 to use
*                             the default.
*
*              p_ext          is a pointer to a user supplied memory location which is used as a TCB extension.
*                             For example, this user memory can hold the contents of floating-point registers
*                             during a context switch, the time each task takes to execute, the number of times
*                             the task has been switched-in, etc.
*
*              opt            contains additional information (or options) about the behavior of the task.
*                             See OS_OPT_TASK_xxx in OS.H.  Current choices are:
*
*                                 OS_OPT_TASK_NONE            No option selected
*                                 OS_OPT_TASK_STK_CHK         Stack checking to be allowed for the task
*                                 OS_OPT_TASK_STK_CLR         Clear the stack when the task is created
*                                 OS_OPT_TASK_SAVE_FP         If the CPU has floating-point registers, save them
*                                                             during a context switch.
*                                 OS_OPT_TASK_NO_TLS          If the caller doesn't want or need TLS (Thread Local 
*                                                             Storage) support for the task.  If you do not include this
*                                                             option, TLS will be supported by default.
*
*              p_err          is a pointer to an error code that will be set during this call.  The value pointer
*                             to by 'p_err' can be:
*
*                                 OS_ERR_NONE                    if the function was successful.
*                                 OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the task after you called
*                                                                   OSSafetyCriticalStart().
*                                 OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                 OS_ERR_PRIO_INVALID            if the priority you specify is higher that the maximum
*                                                                   allowed (i.e. >= OS_CFG_PRIO_MAX-1) or,
*                                                                if OS_CFG_ISR_POST_DEFERRED_EN is set to 1 and you tried
*                                                                   to use priority 0 which is reserved.
*                                 OS_ERR_STK_INVALID             if you specified a NULL pointer for 'p_stk_base'
*                                 OS_ERR_STK_SIZE_INVALID        if you specified zero for the 'stk_size'
*                                 OS_ERR_STK_LIMIT_INVALID       if you specified a 'stk_limit' greater than or equal
*                                                                   to 'stk_size'
*                                 OS_ERR_TASK_CREATE_ISR         if you tried to create a task from an ISR.
*                                 OS_ERR_TASK_INVALID            if you specified a NULL pointer for 'p_task'
*                                 OS_ERR_TCB_INVALID             if you specified a NULL pointer for 'p_tcb'
*
* Returns    : A pointer to the TCB of the task created.  This pointer must be used as an ID (i.e handle) to the task.
************************************************************************************************************************
*/
/*$PAGE*/
void  OSTaskCreate (OS_TCB        *p_tcb,        //任务控制块指针
                    CPU_CHAR      *p_name,       //命名任务
                    OS_TASK_PTR    p_task,       //任务函数
                    void          *p_arg,        //传递给任务函数的参数
                    OS_PRIO        prio,         //任务优先级
                    CPU_STK       *p_stk_base,   //任务堆栈基地址
                    CPU_STK_SIZE   stk_limit,    //堆栈的剩余限制
                    CPU_STK_SIZE   stk_size,     //堆栈大小
                    OS_MSG_QTY     q_size,       //任务消息容量
                    OS_TICK        time_quanta,  //时间片
                    void          *p_ext,        //任务扩展
                    OS_OPT         opt,          //选项
                    OS_ERR        *p_err)        //返回错误类型
{
    CPU_STK_SIZE   i;
#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    OS_REG_ID      reg_nbr;
#endif
#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    OS_TLS_ID      id;
#endif

    CPU_STK       *p_sp;
    CPU_STK       *p_stk_limit;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL                             //如果使能了安全检测         
    if (p_err == (OS_ERR *)0) {                       //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();               //执行安全检测异常函数
        return;                                       //返回，停止执行
    }
#endif

#ifdef OS_SAFETY_CRITICAL_IEC61508                    //如果使能了安全关键
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {      //如果在调用OSSafetyCriticalStart()后创建
       *p_err = OS_ERR_ILLEGAL_CREATE_RUN_TIME;       //错误类型为“非法创建内核对象”
        return;                                       //返回，停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u                //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {        //如果该函数是在中断中被调用
       *p_err = OS_ERR_TASK_CREATE_ISR;               //错误类型为“在中断中创建对象”
        return;                                       //返回，停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                            //如果使能了参数检测      
    if (p_tcb == (OS_TCB *)0) {                       //如果 p_tcb 为空     
       *p_err = OS_ERR_TCB_INVALID;                   //错误类型为“任务控制块非法”
        return;                                       //返回，停止执行
    }
    if (p_task == (OS_TASK_PTR)0) {                   //如果 p_task 为空
       *p_err = OS_ERR_TASK_INVALID;                  //错误类型为“任务函数非法”
        return;                                       //返回，停止执行
    }
    if (p_stk_base == (CPU_STK *)0) {                 //如果 p_stk_base 为空      
       *p_err = OS_ERR_STK_INVALID;                   //错误类型为“任务堆栈非法”
        return;                                       //返回，停止执行
    }
    if (stk_size < OSCfg_StkSizeMin) {                      //如果分配给任务的堆栈空间小于最小要求
       *p_err = OS_ERR_STK_SIZE_INVALID;                    //错误类型为“任务堆栈空间非法”
        return;                                             //返回，停止执行
    }
    if (stk_limit >= stk_size) {                            //如果堆栈限制空间占尽了整个堆栈
       *p_err = OS_ERR_STK_LIMIT_INVALID;                   //错误类型为“堆栈限制非法”
        return;                                             //返回，停止执行
    }
    if (prio >= OS_CFG_PRIO_MAX) {                          //如果任务优先级超出了限制范围     
       *p_err = OS_ERR_PRIO_INVALID;                        //错误类型为i“优先级非法”
        return;                                             //返回，停止执行
    }
#endif

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                        //如果使能了中断延迟发布
    if (prio == (OS_PRIO)0) {                               //如果优先级是0（中断延迟提交任务所需）
        if (p_tcb != &OSIntQTaskTCB) {                      //如果 p_tcb 不是中断延迟提交任务
           *p_err = OS_ERR_PRIO_INVALID;                    //错误类型为“优先级非法”
            return;                                         //返回，停止执行
        }
    }
#endif

    if (prio == (OS_CFG_PRIO_MAX - 1u)) {                   //如果任务的优先级为最低（空闲任务所需）
        if (p_tcb != &OSIdleTaskTCB) {                      //如果 p_tcb 不是空闲任务
           *p_err = OS_ERR_PRIO_INVALID;                    //错误类型为“优先级非法”
            return;                                         //返回，停止执行
        }
    }

    OS_TaskInitTCB(p_tcb);                                  //初始化任务控制块 p_tcb

   *p_err = OS_ERR_NONE;                                    //错误类型为“无错误”
    
    if ((opt & OS_OPT_TASK_STK_CHK) != (OS_OPT)0) {         //如果选择了检测堆栈
        if ((opt & OS_OPT_TASK_STK_CLR) != (OS_OPT)0) {     //如果选择了清零堆栈
            p_sp = p_stk_base;                              //获取堆栈的基地址
            for (i = 0u; i < stk_size; i++) {               //将堆栈（数组）元素逐个清零
               *p_sp = (CPU_STK)0;                          
                p_sp++;
            }
        }
    }
    /* 初始化任务的堆栈结构 */                                               
#if (CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO)         //如果 CPU 的栈增长方向为从高到低
    p_stk_limit = p_stk_base + stk_limit;                   //堆栈限制空间靠堆栈的低地址端
#else                                                       //如果 CPU 的栈增长方向为从低到高
    p_stk_limit = p_stk_base + (stk_size - 1u) - stk_limit; //堆栈限制空间靠堆栈的高地址端
#endif
    /* 初始化任务堆栈 */
    p_sp = OSTaskStkInit(p_task,                            
                         p_arg,
                         p_stk_base,
                         p_stk_limit,
                         stk_size,
                         opt);

    /* 初始化任务控制块 */                                                        
    p_tcb->TaskEntryAddr = p_task;                          //保存任务地址
    p_tcb->TaskEntryArg  = p_arg;                           //保存传递给任务函数的参数

    p_tcb->NamePtr       = p_name;                          //保存任务名称

    p_tcb->Prio          = prio;                            //保存任务遇险记

    p_tcb->StkPtr        = p_sp;                            //保存堆栈的栈顶指针
    p_tcb->StkLimitPtr   = p_stk_limit;                     //保存堆栈限制的临界地址

    p_tcb->TimeQuanta    = time_quanta;                     //保存时间片
#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u                        //如果使能了时间片轮转调度
    if (time_quanta == (OS_TICK)0) {                        //如果 time_quanta 为0
        p_tcb->TimeQuantaCtr = OSSchedRoundRobinDfltTimeQuanta; //使用默认的时间片小小
    } else {                                                //如果 time_quanta 非0
        p_tcb->TimeQuantaCtr = time_quanta;                 //时间片为 time_quanta
    }
#endif
    p_tcb->ExtPtr        = p_ext;                           //保存任务扩展部分的指针
    p_tcb->StkBasePtr    = p_stk_base;                      //保存堆栈的基地址
    p_tcb->StkSize       = stk_size;                        //保存堆栈的大小
    p_tcb->Opt           = opt;                             //保存选项

#if OS_CFG_TASK_REG_TBL_SIZE > 0u                           //如果允许使用任务寄存器
    for (reg_nbr = 0u; reg_nbr < OS_CFG_TASK_REG_TBL_SIZE; reg_nbr++) { //将任务寄存器初始化为0
        p_tcb->RegTbl[reg_nbr] = (OS_REG)0;
    }
#endif

#if OS_CFG_TASK_Q_EN > 0u                                   //如果使能了任务消息队列
    OS_MsgQInit(&p_tcb->MsgQ,                               //初始化任务消息队列
                q_size);
#else                                                       //如果禁用了任务消息队列
    (void)&q_size;                                          //参数 q_size 无效
#endif

    OSTaskCreateHook(p_tcb);                                //调用用户定义的钩子函数

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    for (id = 0u; id < OS_CFG_TLS_TBL_SIZE; id++) {
        p_tcb->TLS_Tbl[id] = (OS_TLS)0;
    }
    OS_TLS_TaskCreate(p_tcb);                               /* Call TLS hook                                          */
#endif
    /* 把任务插入就绪列表 */                                  
    OS_CRITICAL_ENTER();                                    //进入临界段
    OS_PrioInsert(p_tcb->Prio);                             //置就绪优先级位映像表中相应优先级处于就绪状态
    OS_RdyListInsertTail(p_tcb);                            //将新创建的任务插入就绪列表尾端

#if OS_CFG_DBG_EN > 0u                                      //如果使能了调试代码和变量 
    OS_TaskDbgListAdd(p_tcb);                               //把任务插入到任务调试双向列表
#endif

    OSTaskQty++;                                            //任务数目加1

    if (OSRunning != OS_STATE_OS_RUNNING) {                 //如果系统尚未启动
        OS_CRITICAL_EXIT();                                 //退出临界段
        return;                                             //返回，停止执行
    }
    /* 如果系统已经启动 */
    OS_CRITICAL_EXIT_NO_SCHED();                            //退出临界段（无调度）

    OSSched();                                              //进行任务调度
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                     DELETE A TASK
*
* Description: This function allows you to delete a task.  The calling task can delete itself by specifying a NULL
*              pointer for 'p_tcb'.  The deleted task is returned to the dormant state and can be re-activated by
*              creating the deleted task again.
*
* Arguments  : p_tcb      is the TCB of the tack to delete
*
*              p_err      is a pointer to an error code returned by this function:
*
*                             OS_ERR_NONE                  if the call is successful
*                             OS_ERR_STATE_INVALID         if the state of the task is invalid
*                             OS_ERR_TASK_DEL_IDLE         if you attempted to delete uC/OS-III's idle task
*                             OS_ERR_TASK_DEL_INVALID      if you attempted to delete uC/OS-III's ISR handler task
*                             OS_ERR_TASK_DEL_ISR          if you tried to delete a task from an ISR
*
* Note(s)    : 1) 'p_err' gets set to OS_ERR_NONE before OSSched() to allow the returned error code to be monitored even
*                 for a task that is deleting itself. In this case, 'p_err' MUST point to a global variable that can be
*                 accessed by another task.
************************************************************************************************************************
*/

#if OS_CFG_TASK_DEL_EN > 0u                      //如果使能了函数 OSTaskDel()
void  OSTaskDel (OS_TCB  *p_tcb,                 //目标任务控制块指针
                 OS_ERR  *p_err)                 //返回错误类型
{
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                      //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {                //如果 p_err 为空
        OS_SAFETY_CRITICAL_EXCEPTION();        //执行安全检测异常函数
        return;                                //返回，停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
       *p_err = OS_ERR_TASK_DEL_ISR;           //错误类型为“在中断中删除任务”
        return;                                //返回，停止执行
    }
#endif

    if (p_tcb == &OSIdleTaskTCB) {             //如果目标任务是空闲任务
       *p_err = OS_ERR_TASK_DEL_IDLE;          //错误类型为“删除空闲任务”
        return;                                //返回，停止执行
    }

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u           //如果使能了中断延迟发布
    if (p_tcb == &OSIntQTaskTCB) {             //如果目标任务是中断延迟提交任务
       *p_err = OS_ERR_TASK_DEL_INVALID;       //错误类型为“非法删除任务”
        return;                                //返回，停止执行
    }
#endif

    if (p_tcb == (OS_TCB *)0) {                //如果 p_tcb 为空
        CPU_CRITICAL_ENTER();                  //关中断
        p_tcb  = OSTCBCurPtr;                  //目标任务设为自身
        CPU_CRITICAL_EXIT();                   //开中断
    }

    OS_CRITICAL_ENTER();                       //进入临界段
    switch (p_tcb->TaskState) {                //根据目标任务的任务状态分类处理
        case OS_TASK_STATE_RDY:                //如果是就绪状态
             OS_RdyListRemove(p_tcb);          //将任务从就绪列表移除
             break;                            //跳出

        case OS_TASK_STATE_SUSPENDED:          //如果是挂起状态
             break;                            //直接跳出

        case OS_TASK_STATE_DLY:                //如果包含延时状态
        case OS_TASK_STATE_DLY_SUSPENDED:      
             OS_TickListRemove(p_tcb);         //将任务从节拍列表移除
             break;                            //跳出

        case OS_TASK_STATE_PEND:               //如果包含等待状态
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             OS_TickListRemove(p_tcb);         //将任务从节拍列表移除
             switch (p_tcb->PendOn) {          //根据任务的等待对象分类处理
                 case OS_TASK_PEND_ON_NOTHING: //如果没在等待内核对象
                 case OS_TASK_PEND_ON_TASK_Q:  //如果等待的是任务消息队列            
                 case OS_TASK_PEND_ON_TASK_SEM://如果等待的是任务信号量
                      break;                   //直接跳出

                 case OS_TASK_PEND_ON_FLAG:    //如果等待的是事件标志组  
                 case OS_TASK_PEND_ON_MULTI:   //如果等待多个内核对象
                 case OS_TASK_PEND_ON_MUTEX:   //如果等待的是互斥信号量
                 case OS_TASK_PEND_ON_Q:       //如果等待的是消息队列
                 case OS_TASK_PEND_ON_SEM:     //如果等待的是多值信号量
                      OS_PendListRemove(p_tcb);//将任务从等待列表移除
                      break;                   //跳出

                 default:                      //如果等待对象超出预期
                      break;                   //直接跳出
             }
             break;                            //跳出

        default:                               //如果目标任务状态超出预期
            OS_CRITICAL_EXIT();                //退出临界段
           *p_err = OS_ERR_STATE_INVALID;      //错误类型为“状态非法”
            return;                            //返回，停止执行
    }

#if OS_CFG_TASK_Q_EN > 0u                      //如果使能了任务消息队列
    (void)OS_MsgQFreeAll(&p_tcb->MsgQ);        //释放任务的所有任务消息
#endif

    OSTaskDelHook(p_tcb);                      //调用用户自定义的钩子函数

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    OS_TLS_TaskDel(p_tcb);                                  /* Call TLS hook                                          */
#endif

#if OS_CFG_DBG_EN > 0u                         //如果使能了调试代码和变量
    OS_TaskDbgListRemove(p_tcb);               //将任务从任务调试双向列表移除
#endif
    OSTaskQty--;                               //任务数目减1

    OS_TaskInitTCB(p_tcb);                     //初始化任务控制块
    p_tcb->TaskState = (OS_STATE)OS_TASK_STATE_DEL;//标定任务已被删除

    OS_CRITICAL_EXIT_NO_SCHED();               //退出临界段（无调度）

   *p_err = OS_ERR_NONE;                       //错误类型为“无错误”

    OSSched();                                 //调度任务
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    FLUSH TASK's QUEUE
*
* Description: This function is used to flush the task's internal message queue.
*
* Arguments  : p_tcb       is a pointer to the task's OS_TCB.  Specifying a NULL pointer indicates that you wish to
*                          flush the message queue of the calling task.
*
*              p_err       is a pointer to a variable that will contain an error code returned by this function.
*
*                              OS_ERR_NONE           upon success
*                              OS_ERR_FLUSH_ISR      if you called this function from an ISR
*
* Returns     : The number of entries freed from the queue
*
* Note(s)     : 1) You should use this function with great care because, when to flush the queue, you LOOSE the
*                  references to what the queue entries are pointing to and thus, you could cause 'memory leaks'.  In
*                  other words, the data you are pointing to that's being referenced by the queue entries should, most
*                  likely, need to be de-allocated (i.e. freed).
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u
OS_MSG_QTY  OSTaskQFlush (OS_TCB  *p_tcb,
                          OS_ERR  *p_err)
{
    OS_MSG_QTY  entries;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_MSG_QTY)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* Can't flush a message queue from an ISR                */
       *p_err = OS_ERR_FLUSH_ISR;
        return ((OS_MSG_QTY)0);
    }
#endif

    if (p_tcb == (OS_TCB *)0) {                             /* Flush message queue of calling task?                   */
        CPU_CRITICAL_ENTER();
        p_tcb = OSTCBCurPtr;
        CPU_CRITICAL_EXIT();
    }

    OS_CRITICAL_ENTER();
    entries = OS_MsgQFreeAll(&p_tcb->MsgQ);                 /* Return all OS_MSGs to the OS_MSG pool                  */
    OS_CRITICAL_EXIT();
   *p_err   = OS_ERR_NONE;
    return (entries);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                  WAIT FOR A MESSAGE
*
* Description: This function causes the current task to wait for a message to be posted to it.
*
* Arguments  : timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait for a
*                            message to arrive up to the amount of time specified by this argument.
*                            If you specify 0, however, your task will wait forever or, until a message arrives.
*
*              opt           determines whether the user wants to block if the task's queue is empty or not:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_msg_size    is a pointer to a variable that will receive the size of the message
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the message was
*                            received.  If you pass a NULL pointer (i.e. (CPU_TS *)0) then you will not get the
*                            timestamp.  In other words, passing a NULL pointer is valid and indicates that you don't
*                            need the timestamp.
*
*              p_err         is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                                OS_ERR_NONE               The call was successful and your task received a message.
*                                OS_ERR_PEND_ABORT
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but the queue was not empty
*                                OS_ERR_Q_EMPTY
*                                OS_ERR_SCHED_LOCKED       If the scheduler is locked
*                                OS_ERR_TIMEOUT            A message was not received within the specified timeout
*                                                          would lead to a suspension.
*
* Returns    : A pointer to the message received or a NULL pointer upon error.
*
* Note(s)    : 1) It is possible to receive NULL pointers when there are no errors.
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u                     //如果使能了任务消息队列
void  *OSTaskQPend (OS_TICK       timeout,    //等待期限（单位：时钟节拍）
                    OS_OPT        opt,        //选项
                    OS_MSG_SIZE  *p_msg_size, //返回消息长度
                    CPU_TS       *p_ts,       //返回时间戳
                    OS_ERR       *p_err)      //返回错误类型
{
    OS_MSG_Q     *p_msg_q;
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

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //如果该函数在中断中被调用
       *p_err = OS_ERR_PEND_ISR;                //错误类型为“在中断中中止等待”
        return ((void *)0);                     //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                   //如果使能了参数检测
    if (p_msg_size == (OS_MSG_SIZE *)0) {    //如果 p_msg_size 为空
       *p_err = OS_ERR_PTR_INVALID;          //错误类型为“指针不可用”
        return ((void *)0);                  //返回0（有错误），停止执行
    }
    switch (opt) {                           //根据选项分类处理
        case OS_OPT_PEND_BLOCKING:           //如果选项在预期内
        case OS_OPT_PEND_NON_BLOCKING:
             break;                          //直接跳出

        default:                             //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;     //错误类型为“选项非法”
             return ((void *)0);             //返回0（有错误），停止执行
    }
#endif

    if (p_ts != (CPU_TS *)0) {    //如果 p_ts 非空
       *p_ts  = (CPU_TS  )0;      //初始化（清零）p_ts，待用于返回时间戳
    }

    CPU_CRITICAL_ENTER();                                  //关中断
    p_msg_q = &OSTCBCurPtr->MsgQ;                          //获取当前任务的消息队列
    p_void  = OS_MsgQGet(p_msg_q,                          //从队列里获取一个消息
                         p_msg_size,
                         p_ts,
                         p_err);
    if (*p_err == OS_ERR_NONE) {                          //如果获取消息成功
#if OS_CFG_TASK_PROFILE_EN > 0u                        //如果使能了任务控制块的简况变量
        if (p_ts != (CPU_TS *)0) {                                         //如果 p_ts 
            OSTCBCurPtr->MsgQPendTime = OS_TS_GET() - *p_ts;               //非空，更新
            if (OSTCBCurPtr->MsgQPendTimeMax < OSTCBCurPtr->MsgQPendTime) {//等待任务消
                OSTCBCurPtr->MsgQPendTimeMax = OSTCBCurPtr->MsgQPendTime;  //息队列的最
            }                                                              //长时间记录。
        }
#endif
        CPU_CRITICAL_EXIT();                             //开中断 
        return (p_void);                                 //返回消息内容
    }
    /* 如果获取消息不成功（队列里没有消息） */
    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) { //如果选择了不堵塞任务
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                 //错误类型为“缺乏阻塞”
        CPU_CRITICAL_EXIT();                             //开中断
        return ((void *)0);                              //返回0（有错误），停止执行
    } else {                                             //如果选择了堵塞任务
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //如果调度器被锁
            CPU_CRITICAL_EXIT();                         //开中断
           *p_err = OS_ERR_SCHED_LOCKED;                 //错误类型为“调度器被锁”
            return ((void *)0);                          //返回0（有错误），停止执行
        }
    }
    /* 如果调度器未被锁 */                                                       
    OS_CRITICAL_ENTER_CPU_EXIT();                        //锁调度器，重开中断
    OS_Pend((OS_PEND_DATA *)0,                           //阻塞当前任务，等待消息 
            (OS_PEND_OBJ  *)0,
            (OS_STATE      )OS_TASK_PEND_ON_TASK_Q,
            (OS_TICK       )timeout);
    OS_CRITICAL_EXIT_NO_SCHED();                         //解锁调度器（无调度）

    OSSched();                                           //调度任务
    /* 当前任务（获得消息队列的消息）得以继续运行 */
    CPU_CRITICAL_ENTER();                                //关中断
    switch (OSTCBCurPtr->PendStatus) {                   //根据任务的等待状态分类处理
        case OS_STATUS_PEND_OK:                          //如果任务已成功获得消息
             p_void      = OSTCBCurPtr->MsgPtr;          //提取消息内容地址
            *p_msg_size  = OSTCBCurPtr->MsgSize;         //提取消息长度
             if (p_ts != (CPU_TS *)0) {                  //如果 p_ts 非空
                *p_ts  = OSTCBCurPtr->TS;                //获取任务等到消息时的时间戳
#if OS_CFG_TASK_PROFILE_EN > 0u                          //如果使能了任务控制块的简况变量
                OSTCBCurPtr->MsgQPendTime = OS_TS_GET() - OSTCBCurPtr->TS;     //更新等待
                if (OSTCBCurPtr->MsgQPendTimeMax < OSTCBCurPtr->MsgQPendTime) {//任务消息
                    OSTCBCurPtr->MsgQPendTimeMax = OSTCBCurPtr->MsgQPendTime;  //队列的最
                }                                                              //长时间记
#endif                                                                         //录。
             }
            *p_err = OS_ERR_NONE;                        //错误类型为“无错误”
             break;                                      //跳出

        case OS_STATUS_PEND_ABORT:                       //如果等待被中止
             p_void     = (void      *)0;                //返回消息内容为空
            *p_msg_size = (OS_MSG_SIZE)0;                //返回消息大小为0
             if (p_ts  != (CPU_TS *)0) {                 //如果 p_ts 非空
                *p_ts   = (CPU_TS  )0;                   //清零 p_ts
             }
            *p_err      =  OS_ERR_PEND_ABORT;            //错误类型为“等待被中止”
             break;                                      //跳出

        case OS_STATUS_PEND_TIMEOUT:                     //如果等待超时，
        default:                                         //或者任务状态超出预期。
             p_void     = (void      *)0;                //返回消息内容为空
            *p_msg_size = (OS_MSG_SIZE)0;                //返回消息大小为0
             if (p_ts  != (CPU_TS *)0) {                 //如果 p_ts 非空
                *p_ts   =  OSTCBCurPtr->TS;
             }
            *p_err      =  OS_ERR_TIMEOUT;               //错误类为“等待超时”
             break;                                      //跳出
    }
    CPU_CRITICAL_EXIT();                                 //开中断
    return (p_void);                                     //返回消息内容地址
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                              ABORT WAITING FOR A MESSAGE
*
* Description: This function aborts & readies the task specified.  This function should be used to fault-abort the wait
*              for a message, rather than to normally post the message to the task via OSTaskQPost().
*
* Arguments  : p_tcb     is a pointer to the task to pend abort
*
*              opt       provides options for this function:
*
*                            OS_OPT_POST_NONE         No option specified
*                            OS_OPT_POST_NO_SCHED     Indicates that the scheduler will not be called.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE              If the task was readied and informed of the aborted wait
*                            OS_ERR_PEND_ABORT_ISR    If you called this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE   If task was not pending on a message and thus there is nothing to
*                                                     abort.
*                            OS_ERR_PEND_ABORT_SELF   If you passed a NULL pointer for 'p_tcb'
*
* Returns    : == DEF_FALSE   if task was not waiting for a message, or upon error.
*              == DEF_TRUE    if task was waiting for a message and was readied and informed.
************************************************************************************************************************
*/

#if (OS_CFG_TASK_Q_EN > 0u) && (OS_CFG_TASK_Q_PEND_ABORT_EN > 0u)//如果使能了该函数
CPU_BOOLEAN  OSTaskQPendAbort (OS_TCB  *p_tcb,                   //目标任务
                               OS_OPT   opt,                     //选项
                               OS_ERR  *p_err)                   //返回错误类型
{
    CPU_TS         ts;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return (DEF_FALSE);             //返回（有错误），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
       *p_err = OS_ERR_PEND_ABORT_ISR;         //错误类型为“在中断中中止等待”
        return (DEF_FALSE);                    //返回（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                   //如果使能了参数检测
    switch (opt) {                           //根据选项分类处理
        case OS_OPT_POST_NONE:               //如果选项在预期内
        case OS_OPT_POST_NO_SCHED:
             break;                          //直接跳出

        default:                             //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;     //错误类型为“选项非法”
             return (DEF_FALSE);             //返回（有错误），停止执行
    }
#endif

    CPU_CRITICAL_ENTER();                           //关中断
#if OS_CFG_ARG_CHK_EN > 0u                          //如果使能了参数检测
    if ((p_tcb == (OS_TCB *)0) ||                   //如果目标任务为空，
        (p_tcb == OSTCBCurPtr)) {                   //或是自身。
        CPU_CRITICAL_EXIT();                        //开中断
       *p_err = OS_ERR_PEND_ABORT_SELF;             //错误类型为“中止自身”
        return (DEF_FALSE);                         //返回（有错误），停止执行
    }
#endif

    if (p_tcb->PendOn != OS_TASK_PEND_ON_TASK_Q) {  //如果任务没在等待任务消息队列
        CPU_CRITICAL_EXIT();                        //关中断
       *p_err = OS_ERR_PEND_ABORT_NONE;             //错误类型为“中止对象有误”
        return (DEF_FALSE);                         //返回（有错误），停止执行
    }
    /* 如果任务是在等待任务消息队列 */
    OS_CRITICAL_ENTER_CPU_EXIT();                   //锁调度器，重开中断
    ts = OS_TS_GET();                               //获取时间戳作为中止时间
    OS_PendAbort((OS_PEND_OBJ *)0,                  //中止任务对其消息队列的等待
                 p_tcb,
                 ts);
    OS_CRITICAL_EXIT_NO_SCHED();                    //解锁调度器（无调度）
    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) {//如果选择了调度任务
        OSSched();                                  //调度任务
    }
   *p_err = OS_ERR_NONE;                            //错误类型为“无错误”
    return (DEF_TRUE);                              //返回（中止成功）
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                               POST MESSAGE TO A TASK
*
* Description: This function sends a message to a task
*
* Arguments  : p_tcb      is a pointer to the TCB of the task receiving a message.  If you specify a NULL pointer then
*                         the message will be posted to the task's queue of the calling task.  In other words, you'd be
*                         posting a message to yourself.
*
*              p_void     is a pointer to the message to send.
*
*              msg_size   is the size of the message sent (in #bytes)
*
*              opt        specifies whether the post will be FIFO or LIFO:
*
*                             OS_OPT_POST_FIFO       Post at the end   of the queue
*                             OS_OPT_POST_LIFO       Post at the front of the queue
*
*                             OS_OPT_POST_NO_SCHED   Do not run the scheduler after the post
*
*                          Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*
*
*              p_err      is a pointer to a variable that will hold the error code associated
*                         with the outcome of this call.  Errors can be:
*
*                             OS_ERR_NONE            The call was successful and the message was sent
*                             OS_ERR_Q_MAX           If the queue is full
*                             OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs available from the pool
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u                  //如果使能了任务消息队列
void  OSTaskQPost (OS_TCB       *p_tcb,    //目标任务
                   void         *p_void,   //消息内容地址
                   OS_MSG_SIZE   msg_size, //消息长度
                   OS_OPT        opt,      //选项
                   OS_ERR       *p_err)    //返回错误类型
{
    CPU_TS   ts;



#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return;                         //返回，停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //如果使能了参数检测
    switch (opt) {                        //根据选项分类处理
        case OS_OPT_POST_FIFO:            //如果选项在预期内
        case OS_OPT_POST_LIFO:
        case OS_OPT_POST_FIFO | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_LIFO | OS_OPT_POST_NO_SCHED:
             break;                       //直接跳出

        default:                          //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;  //错误类型为“选项非法”
             return;                      //返回，停止执行
    }
#endif

    ts = OS_TS_GET();                                  //获取时间戳

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                   //如果使能了中断延迟发布
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {         //如果该函数在中断中被调用
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_MSG, //将消息先发布到中断消息队列  
                    (void      *)p_tcb,
                    (void      *)p_void,
                    (OS_MSG_SIZE)msg_size,
                    (OS_FLAGS   )0,
                    (OS_OPT     )opt,
                    (CPU_TS     )ts,
                    (OS_ERR    *)p_err);
        return;                                         //返回
    }
#endif

    OS_TaskQPost(p_tcb,                                 //将消息直接发布
                 p_void,
                 msg_size,
                 opt,
                 ts,
                 p_err);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                       GET THE CURRENT VALUE OF A TASK REGISTER
*
* Description: This function is called to obtain the current value of a task register.  Task registers are application
*              specific and can be used to store task specific values such as 'error numbers' (i.e. errno), statistics,
*              etc.
*
* Arguments  : p_tcb     is a pointer to the OS_TCB of the task you want to read the register from.  If 'p_tcb' is a
*                        NULL pointer then you will get the register of the current task.
*
*              id        is the 'id' of the desired task variable.  Note that the 'id' must be less than
*                        OS_CFG_TASK_REG_TBL_SIZE
*
*              p_err     is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE            if the call was successful
*                            OS_ERR_REG_ID_INVALID  if the 'id' is not between 0 and OS_CFG_TASK_REG_TBL_SIZE-1
*
* Returns    : The current value of the task's register or 0 if an error is detected.
************************************************************************************************************************
*/

#if OS_CFG_TASK_REG_TBL_SIZE > 0u          //如果为任务定义了任务寄存器
OS_REG  OSTaskRegGet (OS_TCB     *p_tcb,   //目标任务控制块的指针
                      OS_REG_ID   id,      //任务寄存器的id（数组下标）
                      OS_ERR     *p_err)   //返回错误类型
{
    OS_REG     value;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                 //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {           //如果 p_err 为空
        OS_SAFETY_CRITICAL_EXCEPTION();   //执行安全检测异常函数
        return ((OS_REG)0);               //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //如果使能了参数检测
    if (id >= OS_CFG_TASK_REG_TBL_SIZE) { //如果 id 超出范围
       *p_err = OS_ERR_REG_ID_INVALID;    //错误类型为“id 非法”
        return ((OS_REG)0);               //返回0（有错误），停止执行
    }
#endif
 
    CPU_CRITICAL_ENTER();                 //关中断
    if (p_tcb == (OS_TCB *)0) {           //如果 p_tcb 为空
        p_tcb = OSTCBCurPtr;              //目标任务为当前运行任务（自身）
    }
    value = p_tcb->RegTbl[id];            //获取任务寄存器的内容
    CPU_CRITICAL_EXIT();                  //开中断
   *p_err = OS_ERR_NONE;                  //错误类型为“无错误”
    return ((OS_REG)value);               //返回任务寄存器的内容
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                    ALLOCATE THE NEXT AVAILABLE TASK REGISTER ID
*
* Description: This function is called to obtain a task register ID.  This function thus allows task registers IDs to be
*              allocated dynamically instead of statically.
*
* Arguments  : p_err       is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE               if the call was successful
*                            OS_ERR_NO_MORE_ID_AVAIL   if you are attempting to assign more task register IDs than you 
*                                                           have available through OS_CFG_TASK_REG_TBL_SIZE.
*
* Returns    : The next available task register 'id' or OS_CFG_TASK_REG_TBL_SIZE if an error is detected.
************************************************************************************************************************
*/

#if OS_CFG_TASK_REG_TBL_SIZE > 0u
OS_REG_ID  OSTaskRegGetID (OS_ERR  *p_err)
{
    OS_REG_ID  id;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_REG_ID)OS_CFG_TASK_REG_TBL_SIZE);
    }
#endif

    CPU_CRITICAL_ENTER();
    if (OSTaskRegNextAvailID >= OS_CFG_TASK_REG_TBL_SIZE) {       /* See if we exceeded the number of IDs available   */
       *p_err = OS_ERR_NO_MORE_ID_AVAIL;                          /* Yes, cannot allocate more task register IDs      */
        CPU_CRITICAL_EXIT();
        return ((OS_REG_ID)OS_CFG_TASK_REG_TBL_SIZE);
    }
     
    id    = OSTaskRegNextAvailID;								  /* Assign the next available ID                     */
    OSTaskRegNextAvailID++;										  /* Increment available ID for next request          */
    CPU_CRITICAL_EXIT();
   *p_err = OS_ERR_NONE;
    return (id);
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                       SET THE CURRENT VALUE OF A TASK REGISTER
*
* Description: This function is called to change the current value of a task register.  Task registers are application
*              specific and can be used to store task specific values such as 'error numbers' (i.e. errno), statistics,
*              etc.
*
* Arguments  : p_tcb     is a pointer to the OS_TCB of the task you want to set the register for.  If 'p_tcb' is a NULL
*                        pointer then you will change the register of the current task.
*
*              id        is the 'id' of the desired task register.  Note that the 'id' must be less than
*                        OS_CFG_TASK_REG_TBL_SIZE
*
*              value     is the desired value for the task register.
*
*              p_err     is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE            if the call was successful
*                            OS_ERR_REG_ID_INVALID  if the 'id' is not between 0 and OS_CFG_TASK_REG_TBL_SIZE-1
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_TASK_REG_TBL_SIZE > 0u        //如果为任务定义了任务寄存器
void  OSTaskRegSet (OS_TCB     *p_tcb,   //目标任务控制块的指针
                    OS_REG_ID   id,      //任务寄存器的id（数组下标）
                    OS_REG      value,   //设置任务寄存器的内容
                    OS_ERR     *p_err)   //返回错误类型
{
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {          //如果 p_err 为空
        OS_SAFETY_CRITICAL_EXCEPTION();  //执行安全检测异常函数
        return;                          //返回，停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //如果使能了参数检测
    if (id >= OS_CFG_TASK_REG_TBL_SIZE) { //如果 id 超出范围
       *p_err = OS_ERR_REG_ID_INVALID;    //错误类型为“id 非法”
        return;                           //返回，停止执行
    }
#endif

    CPU_CRITICAL_ENTER();                 //关中断
    if (p_tcb == (OS_TCB *)0) {           //如果 p_tcb 为空
        p_tcb = OSTCBCurPtr;              //目标任务为当前运行任务（自身）
    }
    p_tcb->RegTbl[id] = value;            //设置任务寄存器的内容
    CPU_CRITICAL_EXIT();                  //开中断
   *p_err             = OS_ERR_NONE;      //错误类型为“无错误”
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                               RESUME A SUSPENDED TASK
*
* Description: This function is called to resume a previously suspended task.  This is the only call that will remove an
*              explicit task suspension.
*
* Arguments  : p_tcb      Is a pointer to the task's OS_TCB to resume
*
*              p_err      Is a pointer to a variable that will contain an error code returned by this function
*
*                             OS_ERR_NONE                  if the requested task is resumed
*                             OS_ERR_STATE_INVALID         if the task is in an invalid state
*                             OS_ERR_TASK_RESUME_ISR       if you called this function from an ISR
*                             OS_ERR_TASK_RESUME_SELF      You cannot resume 'self'
*                             OS_ERR_TASK_NOT_SUSPENDED    if the task to resume has not been suspended
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_TASK_SUSPEND_EN > 0u            //如果使能了函数 OSTaskResume() 
void  OSTaskResume (OS_TCB  *p_tcb,        //任务控制块指针
                    OS_ERR  *p_err)        //返回错误类型
{
    CPU_SR_ALLOC();  //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
									   //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
									   // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                 //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {           //如果 p_err 为空
        OS_SAFETY_CRITICAL_EXCEPTION();   //执行安全检测异常函数
        return;                           //返回，停止执行
    }
#endif

#if (OS_CFG_ISR_POST_DEFERRED_EN   == 0u) && \
    (OS_CFG_CALLED_FROM_ISR_CHK_EN >  0u)      //如果禁用了中断延迟发布和中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果在中断中调用该函数
       *p_err = OS_ERR_TASK_RESUME_ISR;        //错误类型为“在中断中恢复任务”
        return;                                //返回，停止执行
    }
#endif


    CPU_CRITICAL_ENTER();                     //关中断
#if OS_CFG_ARG_CHK_EN > 0u                    //如果使能了参数检测
    if ((p_tcb == (OS_TCB *)0) ||             //如果被恢复任务为空或是自身
        (p_tcb == OSTCBCurPtr)) {
        CPU_CRITICAL_EXIT();                  //开中断
       *p_err  = OS_ERR_TASK_RESUME_SELF;     //错误类型为“恢复自身”
        return;                               //返回，停止执行
    }
#endif
    CPU_CRITICAL_EXIT();                      //关中断

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                      //如果使能了中断延迟发布
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {            //如果该函数在中断中被调用
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_RESUME, //把恢复任务命令发布到中断消息队列
                    (void      *)p_tcb,
                    (void      *)0,
                    (OS_MSG_SIZE)0,
                    (OS_FLAGS   )0,
                    (OS_OPT     )0,
                    (CPU_TS     )0,
                    (OS_ERR    *)p_err);
        return;                                          //返回，停止执行
    }
#endif
    /* 如果禁用了中断延迟发布或不是在中断中调用该函数 */
    OS_TaskResume(p_tcb, p_err);                        //直接将任务 p_tcb 挂起 
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                              WAIT FOR A TASK SEMAPHORE
*
* Description: This function is called to block the current task until a signal is sent by another task or ISR.
*
* Arguments  : timeout       is the amount of time you are will to wait for the signal
*
*              opt           determines whether the user wants to block if a semaphore post was not received:
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the semaphore was posted
*                            or pend aborted.  If you pass a NULL pointer (i.e. (CPU_TS *)0) then you will not get the
*                            timestamp.  In other words, passing a NULL pointer is valid and indicates that you don't
*                            need the timestamp.
*
*              p_err         is a pointer to an error code that will be set by this function
*
*                                OS_ERR_NONE               The call was successful and your task received a message.
*                                OS_ERR_PEND_ABORT
*                                OS_ERR_PEND_ISR           If you called this function from an ISR and the result
*                                OS_ERR_PEND_WOULD_BLOCK   If you specified non-blocking but no signal was received
*                                OS_ERR_SCHED_LOCKED       If the scheduler is locked
*                                OS_ERR_STATUS_INVALID     If the pend status is invalid
*                                OS_ERR_TIMEOUT            A message was not received within the specified timeout
*                                                          would lead to a suspension.
*
* Returns    : The current count of signals the task received, 0 if none.
************************************************************************************************************************
*/

OS_SEM_CTR  OSTaskSemPend (OS_TICK   timeout,  //等待超时时间
                           OS_OPT    opt,      //选项
                           CPU_TS   *p_ts,     //返回时间戳
                           OS_ERR   *p_err)    //返回错误类型
{
    OS_SEM_CTR    ctr;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {          //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();  //执行安全检测异常函数
        return ((OS_SEM_CTR)0);          //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //如果该函数在中断中被调用
       *p_err = OS_ERR_PEND_ISR;                //返回错误类型为“在中断中等待”
        return ((OS_SEM_CTR)0);                 //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                  //如果使能了参数检测
    switch (opt) {                          //根据选项分类处理
        case OS_OPT_PEND_BLOCKING:          //如果选项在预期内
        case OS_OPT_PEND_NON_BLOCKING:
             break;                         //直接跳出

        default:                            //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;    //错误类型为“选项非法”
             return ((OS_SEM_CTR)0);        //返回0（有错误），停止执行
    }
#endif

    if (p_ts != (CPU_TS *)0) {      //如果 p_ts 非空
       *p_ts  = (CPU_TS  )0;        //清零（初始化）p_ts
    }

    CPU_CRITICAL_ENTER();                        //关中断  
    if (OSTCBCurPtr->SemCtr > (OS_SEM_CTR)0) {   //如果任务信号量当前可用
        OSTCBCurPtr->SemCtr--;                   //信号量计数器减1
        ctr    = OSTCBCurPtr->SemCtr;            //获取信号量的当前计数值
        if (p_ts != (CPU_TS *)0) {               //如果 p_ts 非空
           *p_ts  = OSTCBCurPtr->TS;             //返回信号量被发布的时间戳
        }
#if OS_CFG_TASK_PROFILE_EN > 0u                  //如果使能了任务控制块的简况变量
        OSTCBCurPtr->SemPendTime = OS_TS_GET() - OSTCBCurPtr->TS;     //更新任务等待
        if (OSTCBCurPtr->SemPendTimeMax < OSTCBCurPtr->SemPendTime) { //任务信号量的
            OSTCBCurPtr->SemPendTimeMax = OSTCBCurPtr->SemPendTime;   //最长时间记录。
        }
#endif
        CPU_CRITICAL_EXIT();                     //开中断            
       *p_err = OS_ERR_NONE;                     //错误类型为“无错误”
        return (ctr);                            //返回信号量的当前计数值
    }
    /* 如果任务信号量当前不可用 */
    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {  //如果选择了不阻塞任务
        CPU_CRITICAL_EXIT();                              //开中断
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                  //错误类型为“缺乏阻塞”
        return ((OS_SEM_CTR)0);                           //返回0（有错误），停止执行
    } else {                                              //如果选择了阻塞任务
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {  //如果调度器被锁
            CPU_CRITICAL_EXIT();                          //开中断
           *p_err = OS_ERR_SCHED_LOCKED;                  //错误类型为“调度器被锁”
            return ((OS_SEM_CTR)0);                       //返回0（有错误），停止执行
        }
    }
    /* 如果调度器未被锁 */
    OS_CRITICAL_ENTER_CPU_EXIT();                         //锁调度器，重开中断                      
    OS_Pend((OS_PEND_DATA *)0,                            //阻塞任务，等待信号量。
            (OS_PEND_OBJ  *)0,                            //不需插入等待列表。
            (OS_STATE      )OS_TASK_PEND_ON_TASK_SEM,
            (OS_TICK       )timeout);
    OS_CRITICAL_EXIT_NO_SCHED();                          //开调度器（无调度）

    OSSched();                                            //调度任务
    /* 任务获得信号量后得以继续运行 */
    CPU_CRITICAL_ENTER();                                 //关中断
    switch (OSTCBCurPtr->PendStatus) {                    //根据任务的等待状态分类处理
        case OS_STATUS_PEND_OK:                           //如果任务成功获得信号量
             if (p_ts != (CPU_TS *)0) {                   //返回信号量被发布的时间戳
                *p_ts                    =  OSTCBCurPtr->TS;
#if OS_CFG_TASK_PROFILE_EN > 0u                           //更新最长等待时间记录
                OSTCBCurPtr->SemPendTime = OS_TS_GET() - OSTCBCurPtr->TS;
                if (OSTCBCurPtr->SemPendTimeMax < OSTCBCurPtr->SemPendTime) {
                    OSTCBCurPtr->SemPendTimeMax = OSTCBCurPtr->SemPendTime;
                }
#endif
             }
            *p_err = OS_ERR_NONE;                         //错误类型为“无错误”
             break;                                       //跳出

        case OS_STATUS_PEND_ABORT:                        //如果等待被中止
             if (p_ts != (CPU_TS *)0) {                   //返回被终止时的时间戳
                *p_ts  =  OSTCBCurPtr->TS;
             }
            *p_err = OS_ERR_PEND_ABORT;                   //错误类型为“等待被中止”
             break;                                       //跳出

        case OS_STATUS_PEND_TIMEOUT:                      //如果等待超时
             if (p_ts != (CPU_TS *)0) {                   //返回时间戳为0
                *p_ts  = (CPU_TS  )0;
             }
            *p_err = OS_ERR_TIMEOUT;                      //错误类型为“等待超时”
             break;                                       //跳出

        default:                                          //如果等待状态超出预期
            *p_err = OS_ERR_STATUS_INVALID;               //错误类型为“状态非法”
             break;                                       //跳出
    }                                                     
    ctr = OSTCBCurPtr->SemCtr;                            //获取信号量的当前计数值
    CPU_CRITICAL_EXIT();                                  //开中断
    return (ctr);                                         //返回信号量的当前计数值
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               ABORT WAITING FOR A SIGNAL
*
* Description: This function aborts & readies the task specified.  This function should be used to fault-abort the wait
*              for a signal, rather than to normally post the signal to the task via OSTaskSemPost().
*
* Arguments  : p_tcb     is a pointer to the task to pend abort
*
*              opt       provides options for this function:
*
*                            OS_OPT_POST_NONE         No option selected
*                            OS_OPT_POST_NO_SCHED     Indicates that the scheduler will not be called.
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE              If the task was readied and informed of the aborted wait
*                            OS_ERR_PEND_ABORT_ISR    If you tried calling this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE   If the task was not waiting for a signal
*                            OS_ERR_PEND_ABORT_SELF   If you attempted to pend abort the calling task.  This is not
*                                                     possible since the calling task cannot be pending because it's
*                                                     running.
*
* Returns    : == DEF_FALSE   if task was not waiting for a message, or upon error.
*              == DEF_TRUE    if task was waiting for a message and was readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_TASK_SEM_PEND_ABORT_EN > 0u  //如果使能了 OSTaskSemPendAbort()
CPU_BOOLEAN  OSTaskSemPendAbort (OS_TCB  *p_tcb, //目标任务
                                 OS_OPT   opt,   //选项
                                 OS_ERR  *p_err) //返回错误类型
{
    CPU_TS         ts;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return (DEF_FALSE);             //返回（失败），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u           //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {   //如果该函数是在中断中被调用
       *p_err = OS_ERR_PEND_ABORT_ISR;           //错误类型为“在中断中创建对象”
        return (DEF_FALSE);                      //返回（失败），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                  //如果使能了参数检测
    switch (opt) {                          //根据选项匪类处理
        case OS_OPT_POST_NONE:              //如果选项在预期内
        case OS_OPT_POST_NO_SCHED:
             break;                         //直接跳出

        default:                            //如果选项超出预期
            *p_err =  OS_ERR_OPT_INVALID;   //错误类型为“选项非法”
             return (DEF_FALSE);            //返回（失败），停止执行
    }
#endif

    CPU_CRITICAL_ENTER();                 //关中断
    if ((p_tcb == (OS_TCB *)0) ||         //如果 p_tcb 为空，或者
        (p_tcb == OSTCBCurPtr)) {         //p_tcb 指向当前运行任务。
        CPU_CRITICAL_EXIT();              //开中断
       *p_err = OS_ERR_PEND_ABORT_SELF;   //错误类型为“中止自身”
        return (DEF_FALSE);               //返回（失败），停止执行
    }
    /* 如果 p_tcb （目标任务） 不是当前运行任务（自身） */
    if (p_tcb->PendOn != OS_TASK_PEND_ON_TASK_SEM) { //如果目标任务没在等待任务信号量
        CPU_CRITICAL_EXIT();                         //开中断
       *p_err = OS_ERR_PEND_ABORT_NONE;              //错误类型为“没在等待任务信号量”
        return (DEF_FALSE);                          //返回（失败），停止执行
    }
    CPU_CRITICAL_EXIT();                             //开中断

    OS_CRITICAL_ENTER();                             //进入临界段
    ts = OS_TS_GET();                                //获取时间戳
    OS_PendAbort((OS_PEND_OBJ *)0,                   //中止目标任务对信号量的等待
                 p_tcb, 
                 ts);
    OS_CRITICAL_EXIT_NO_SCHED();                     //退出临界段（无调度）
    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) { //如果选择了任务调度
        OSSched();                                   //调度任务
    }
   *p_err = OS_ERR_NONE;                             //错误类型为“无错误”
    return (DEF_TRUE);                               //返回（中止成功）
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    SIGNAL A TASK
*
* Description: This function is called to signal a task waiting for a signal.
*
* Arguments  : p_tcb     is the pointer to the TCB of the task to signal.  A NULL pointer indicates that you are sending
*                        a signal to yourself.
*
*              opt       determines the type of POST performed:
*
*                             OS_OPT_POST_NONE         No option
*                             OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*              p_err     is a pointer to an error code returned by this function:
*
*                            OS_ERR_NONE              If the requested task is signaled
*                            OS_ERR_SEM_OVF           If the post would cause the semaphore count to overflow.
*
* Returns    : The current value of the task's signal counter or 0 if called from an ISR
************************************************************************************************************************
*/

OS_SEM_CTR  OSTaskSemPost (OS_TCB  *p_tcb,   //目标任务
                           OS_OPT   opt,     //选项
                           OS_ERR  *p_err)   //返回错误类型
{
    OS_SEM_CTR  ctr;
    CPU_TS      ts;



#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果 p_err 为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return ((OS_SEM_CTR)0);         //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                  //如果使能（默认使能）了参数检测功能
    switch (opt) {                          //根据选项分类处理
        case OS_OPT_POST_NONE:              //如果选项在预期之内
        case OS_OPT_POST_NO_SCHED:
             break;                         //跳出

        default:                            //如果选项超出预期
            *p_err =  OS_ERR_OPT_INVALID;   //错误类型为“选项非法”
             return ((OS_SEM_CTR)0u);       //返回0（有错误），停止执行
    }
#endif

    ts = OS_TS_GET();                                      //获取时间戳

#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                       //如果使能了中断延迟发布
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {             //如果该函数是在中断中被调用
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_SIGNAL,  //将该信号量发布到中断消息队列
                    (void      *)p_tcb,
                    (void      *)0,
                    (OS_MSG_SIZE)0,
                    (OS_FLAGS   )0,
                    (OS_OPT     )0,
                    (CPU_TS     )ts,
                    (OS_ERR    *)p_err);
        return ((OS_SEM_CTR)0);                           //返回0（尚未发布）   
    }
#endif

    ctr = OS_TaskSemPost(p_tcb,                          //将信号量按照普通方式处理
                         opt,
                         ts,
                         p_err);

    return (ctr);                                       //返回信号的当前计数值
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                            SET THE SIGNAL COUNTER OF A TASK
*
* Description: This function is called to clear the signal counter
*
* Arguments  : p_tcb      is the pointer to the TCB of the task to clear the counter.  If you specify a NULL pointer
*                         then the signal counter of the current task will be cleared.
*
*              cnt        is the desired value of the semaphore counter
*
*              p_err      is a pointer to an error code returned by this function
*
*                             OS_ERR_NONE        if the signal counter of the requested task is cleared
*                             OS_ERR_SET_ISR     if the function was called from an ISR
*
* Returns    : none
************************************************************************************************************************
*/

OS_SEM_CTR  OSTaskSemSet (OS_TCB      *p_tcb,
                          OS_SEM_CTR   cnt,
                          OS_ERR      *p_err)
{
    OS_SEM_CTR  ctr;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_SEM_CTR)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* Not allowed to call from an ISR                        */
       *p_err = OS_ERR_SET_ISR;
        return ((OS_SEM_CTR)0);
    }
#endif

    CPU_CRITICAL_ENTER();
    if (p_tcb == (OS_TCB *)0) {
        p_tcb = OSTCBCurPtr;
    }
    ctr           = p_tcb->SemCtr;
    p_tcb->SemCtr = (OS_SEM_CTR)cnt;
    CPU_CRITICAL_EXIT();
   *p_err         =  OS_ERR_NONE;
    return (ctr);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    STACK CHECKING
*
* Description: This function is called to calculate the amount of free memory left on the specified task's stack.
*
* Arguments  : p_tcb       is a pointer to the TCB of the task to check.  If you specify a NULL pointer then
*                          you are specifying that you want to check the stack of the current task.
*
*              p_free      is a pointer to a variable that will receive the number of free 'entries' on the task's stack.
*
*              p_used      is a pointer to a variable that will receive the number of used 'entries' on the task's stack.
*
*              p_err       is a pointer to a variable that will contain an error code.
*
*                              OS_ERR_NONE               upon success
*                              OS_ERR_PTR_INVALID        if either 'p_free' or 'p_used' are NULL pointers
*                              OS_ERR_TASK_NOT_EXIST     if the stack pointer of the task is a NULL pointer
*                              OS_ERR_TASK_OPT           if you did NOT specified OS_OPT_TASK_STK_CHK when the task
*                                                        was created
*                              OS_ERR_TASK_STK_CHK_ISR   you called this function from an ISR
************************************************************************************************************************
*/

#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
void  OSTaskStkChk (OS_TCB        *p_tcb,
                    CPU_STK_SIZE  *p_free,
                    CPU_STK_SIZE  *p_used,
                    OS_ERR        *p_err)
{
    CPU_STK_SIZE  free_stk;
    CPU_STK      *p_stk;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* See if trying to check stack from ISR                  */
       *p_err = OS_ERR_TASK_STK_CHK_ISR;
        return;
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u
    if (p_free == (CPU_STK_SIZE*)0) {                       /* User must specify valid destinations for the sizes     */
       *p_err  = OS_ERR_PTR_INVALID;
        return;
    }

    if (p_used == (CPU_STK_SIZE*)0) {
       *p_err  = OS_ERR_PTR_INVALID;
        return;
    }
#endif

    CPU_CRITICAL_ENTER();
    if (p_tcb == (OS_TCB *)0) {                             /* Check the stack of the current task?                   */
        p_tcb = OSTCBCurPtr;                                /* Yes                                                    */
    }

    if (p_tcb->StkPtr == (CPU_STK*)0) {                     /* Make sure task exist                                   */
        CPU_CRITICAL_EXIT();
       *p_free = (CPU_STK_SIZE)0;
       *p_used = (CPU_STK_SIZE)0;
       *p_err  =  OS_ERR_TASK_NOT_EXIST;
        return;
    }

    if ((p_tcb->Opt & OS_OPT_TASK_STK_CHK) == (OS_OPT)0) {  /* Make sure stack checking option is set                 */
        CPU_CRITICAL_EXIT();
       *p_free = (CPU_STK_SIZE)0;
       *p_used = (CPU_STK_SIZE)0;
       *p_err  =  OS_ERR_TASK_OPT;
        return;
    }
    CPU_CRITICAL_EXIT();

    free_stk  = 0u;
#if CPU_CFG_STK_GROWTH == CPU_STK_GROWTH_HI_TO_LO
    p_stk = p_tcb->StkBasePtr;                              /* Start at the lowest memory and go up                   */
    while (*p_stk == (CPU_STK)0) {                          /* Compute the number of zero entries on the stk          */
        p_stk++;
        free_stk++;
    }
#else
    p_stk = p_tcb->StkBasePtr + p_tcb->StkSize - 1u;        /* Start at the highest memory and go down                */
    while (*p_stk == (CPU_STK)0) {
        free_stk++;
        p_stk--;
    }
#endif
   *p_free = free_stk;
   *p_used = (p_tcb->StkSize - free_stk);                   /* Compute number of entries used on the stack            */
   *p_err  = OS_ERR_NONE;
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   SUSPEND A TASK
*
* Description: This function is called to suspend a task.  The task can be the calling task if 'p_tcb' is a NULL pointer
*              or the pointer to the TCB of the calling task.
*
* Arguments  : p_tcb    is a pointer to the TCB to suspend.
*                       If p_tcb is a NULL pointer then, suspend the current task.
*
*              p_err    is a pointer to a variable that will receive an error code from this function.
*
*                           OS_ERR_NONE                      if the requested task is suspended
*                           OS_ERR_SCHED_LOCKED              you can't suspend the current task is the scheduler is
*                                                            locked
*                           OS_ERR_TASK_SUSPEND_ISR          if you called this function from an ISR
*                           OS_ERR_TASK_SUSPEND_IDLE         if you attempted to suspend the idle task which is not
*                                                            allowed.
*                           OS_ERR_TASK_SUSPEND_INT_HANDLER  if you attempted to suspend the idle task which is not
*                                                            allowed.
*
* Note(s)    : 1) You should use this function with great care.  If you suspend a task that is waiting for an event
*                 (i.e. a message, a semaphore, a queue ...) you will prevent this task from running when the event
*                 arrives.
************************************************************************************************************************
*/

#if OS_CFG_TASK_SUSPEND_EN > 0u                         //如果使能了函数 OSTaskSuspend()     
void   OSTaskSuspend (OS_TCB  *p_tcb,                   //任务控制块指针
                      OS_ERR  *p_err)                   //返回错误类型
{
#ifdef OS_SAFETY_CRITICAL                               //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {                         //如果 p_err 为空
        OS_SAFETY_CRITICAL_EXCEPTION();                 //执行安全检测异常函数
        return;                                         //返回，停止执行
    }
#endif

#if (OS_CFG_ISR_POST_DEFERRED_EN   == 0u) && \
    (OS_CFG_CALLED_FROM_ISR_CHK_EN >  0u)               //如果禁用了中断延迟发布和中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {          //如果在中断中调用该函数
       *p_err = OS_ERR_TASK_SUSPEND_ISR;                //错误类型为“在中断中挂起任务”
        return;                                         //返回，停止执行
    }
#endif

    if (p_tcb == &OSIdleTaskTCB) {                      //如果 p_tcb 是空闲任务   
       *p_err = OS_ERR_TASK_SUSPEND_IDLE;               //错误类型为“挂起空闲任务”
        return;                                         //返回，停止执行
    }
 
#if OS_CFG_ISR_POST_DEFERRED_EN > 0u                    //如果使能了中断延迟发布
    if (p_tcb == &OSIntQTaskTCB) {                      //如果 p_tcb 为中断延迟提交任务
       *p_err = OS_ERR_TASK_SUSPEND_INT_HANDLER;        //错误类型为“挂起中断延迟提交任务”
        return;                                         //返回，停止执行
    }

    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              //如果在中断中调用该函数
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_TASK_SUSPEND,  //把挂起任务命令发布到中断消息队列
                    (void      *)p_tcb,
                    (void      *)0,
                    (OS_MSG_SIZE)0,
                    (OS_FLAGS   )0,
                    (OS_OPT     )0,
                    (CPU_TS     )0,
                    (OS_ERR    *)p_err);
        return;                                            //返回，停止执行
    }
#endif
    /* 如果禁用了中断延迟发布或不是在中断中调用该函数 */
    OS_TaskSuspend(p_tcb, p_err);                          //直接将任务 p_tcb 挂起
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                CHANGE A TASK'S TIME SLICE
*
* Description: This function is called to change the value of the task's specific time slice.
*
* Arguments  : p_tcb        is the pointer to the TCB of the task to change. If you specify an NULL pointer, the current
*                           task is assumed.
*
*              time_quanta  is the number of ticks before the CPU is taken away when round-robin scheduling is enabled.
*
*              p_err        is a pointer to an error code returned by this function:
*
*                               OS_ERR_NONE       upon success
*                               OS_ERR_SET_ISR    if you called this function from an ISR
*
* Returns    : none
************************************************************************************************************************
*/

#if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u                //如果使能了时间片轮转调度
void  OSTaskTimeQuantaSet (OS_TCB   *p_tcb,         //任务控制块指针
                           OS_TICK   time_quanta,   //设置时间片
                           OS_ERR   *p_err)         //返回错误类型
{
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {          //如果 p_err 为空
        OS_SAFETY_CRITICAL_EXCEPTION();  //执行安全检测异常函数
        return;                          //返回，停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
       *p_err = OS_ERR_SET_ISR;                //错误类型为“在中断中设置”
        return;                                //返回，停止执行
    }
#endif

    CPU_CRITICAL_ENTER();                      //关中断
    if (p_tcb == (OS_TCB *)0) {                //如果 p_tcb 为空
        p_tcb = OSTCBCurPtr;                   //目标任务为当前运行任务
    }

    if (time_quanta == 0u) {                   //如果 time_quanta 为0
        p_tcb->TimeQuanta    = OSSchedRoundRobinDfltTimeQuanta; //设置为默认时间片
    } else {                                   //如果 time_quanta 非0
        p_tcb->TimeQuanta    = time_quanta;    //把 time_quanta 设为任务的时间片
    }
    if (p_tcb->TimeQuanta > p_tcb->TimeQuantaCtr) { //如果任务的剩余时间片小于新设置值
        p_tcb->TimeQuantaCtr = p_tcb->TimeQuanta;   //把剩余时间片调整为新设置值
    }
    CPU_CRITICAL_EXIT();                       //开中断
   *p_err = OS_ERR_NONE;                       //错误类型为“无错误”
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                            ADD/REMOVE TASK TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove an OS_TCB from the debug list.
*
* Arguments  : p_tcb     is a pointer to the OS_TCB to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u                                    //如果使能了调试代码和变量
void  OS_TaskDbgListAdd (OS_TCB  *p_tcb)                  //将任务插入到任务调试列表的最前端
{
    p_tcb->DbgPrevPtr                = (OS_TCB *)0;       //将该任务作为列表的最前端
    if (OSTaskDbgListPtr == (OS_TCB *)0) {                //如果列表为空
        p_tcb->DbgNextPtr            = (OS_TCB *)0;       //该队列的下一个元素也为空
    } else {                                              //如果列表非空
        p_tcb->DbgNextPtr            =  OSTaskDbgListPtr; //列表原来的首元素作为该任务的下一个元素
        OSTaskDbgListPtr->DbgPrevPtr =  p_tcb;            //原来的首元素的前面变为了该任务
    }
    OSTaskDbgListPtr                 =  p_tcb;            //该任务成为列表的新首元素
}



void  OS_TaskDbgListRemove (OS_TCB  *p_tcb)
{
    OS_TCB  *p_tcb_next;
    OS_TCB  *p_tcb_prev;


    p_tcb_prev = p_tcb->DbgPrevPtr;
    p_tcb_next = p_tcb->DbgNextPtr;

    if (p_tcb_prev == (OS_TCB *)0) {
        OSTaskDbgListPtr = p_tcb_next;
        if (p_tcb_next != (OS_TCB *)0) {
            p_tcb_next->DbgPrevPtr = (OS_TCB *)0;
        }
        p_tcb->DbgNextPtr = (OS_TCB *)0;

    } else if (p_tcb_next == (OS_TCB *)0) {
        p_tcb_prev->DbgNextPtr = (OS_TCB *)0;
        p_tcb->DbgPrevPtr      = (OS_TCB *)0;

    } else {
        p_tcb_prev->DbgNextPtr =  p_tcb_next;
        p_tcb_next->DbgPrevPtr =  p_tcb_prev;
        p_tcb->DbgNextPtr      = (OS_TCB *)0;
        p_tcb->DbgPrevPtr      = (OS_TCB *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                             TASK MANAGER INITIALIZATION
*
* Description: This function is called by OSInit() to initialize the task management.
*

* Argument(s): p_err        is a pointer to a variable that will contain an error code returned by this function.
*
*                                OS_ERR_NONE     the call was successful
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TaskInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSTaskDbgListPtr = (OS_TCB      *)0;
#endif

    OSTaskQty        = (OS_OBJ_QTY   )0;                    /* Clear the number of tasks                              */
    OSTaskCtxSwCtr   = (OS_CTX_SW_CTR)0;                    /* Clear the context switch counter                       */

   *p_err            = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               INITIALIZE TCB FIELDS
*
* Description: This function is called to initialize a TCB to default values
*
* Arguments  : p_tcb    is a pointer to the TCB to initialize
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TaskInitTCB (OS_TCB  *p_tcb)
{
#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    OS_REG_ID   reg_id;
#endif
#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    OS_TLS_ID   id;
#endif
#if OS_CFG_TASK_PROFILE_EN > 0u
    CPU_TS      ts;
#endif


    p_tcb->StkPtr             = (CPU_STK       *)0;
    p_tcb->StkLimitPtr        = (CPU_STK       *)0;

    p_tcb->ExtPtr             = (void          *)0;

    p_tcb->NextPtr            = (OS_TCB        *)0;
    p_tcb->PrevPtr            = (OS_TCB        *)0;

    p_tcb->TickNextPtr        = (OS_TCB        *)0;
    p_tcb->TickPrevPtr        = (OS_TCB        *)0;
    p_tcb->TickSpokePtr       = (OS_TICK_SPOKE *)0;

    p_tcb->NamePtr            = (CPU_CHAR      *)((void *)"?Task");

    p_tcb->StkBasePtr         = (CPU_STK       *)0;

    p_tcb->TaskEntryAddr      = (OS_TASK_PTR    )0;
    p_tcb->TaskEntryArg       = (void          *)0;

#if (OS_CFG_PEND_MULTI_EN > 0u)
    p_tcb->PendDataTblPtr     = (OS_PEND_DATA  *)0;
    p_tcb->PendDataTblEntries = (OS_OBJ_QTY     )0u;
#endif

    p_tcb->TS                 = (CPU_TS         )0u;

#if (OS_MSG_EN > 0u)
    p_tcb->MsgPtr             = (void          *)0;
    p_tcb->MsgSize            = (OS_MSG_SIZE    )0u;
#endif

#if OS_CFG_TASK_Q_EN > 0u
    OS_MsgQInit(&p_tcb->MsgQ,
                (OS_MSG_QTY)0u);
#if OS_CFG_TASK_PROFILE_EN > 0u
    p_tcb->MsgQPendTime       = (CPU_TS         )0u;
    p_tcb->MsgQPendTimeMax    = (CPU_TS         )0u;
#endif
#endif

#if OS_CFG_FLAG_EN > 0u
    p_tcb->FlagsPend          = (OS_FLAGS       )0u;
    p_tcb->FlagsOpt           = (OS_OPT         )0u;
    p_tcb->FlagsRdy           = (OS_FLAGS       )0u;
#endif

#if OS_CFG_TASK_REG_TBL_SIZE > 0u
    for (reg_id = 0u; reg_id < OS_CFG_TASK_REG_TBL_SIZE; reg_id++) {
        p_tcb->RegTbl[reg_id] = (OS_REG)0u;
    }
#endif

#if defined(OS_CFG_TLS_TBL_SIZE) && (OS_CFG_TLS_TBL_SIZE > 0u)
    for (id = 0u; id < OS_CFG_TLS_TBL_SIZE; id++) {
        p_tcb->TLS_Tbl[id]    = (OS_TLS)0;
    }
#endif

    p_tcb->SemCtr             = (OS_SEM_CTR     )0u;
#if OS_CFG_TASK_PROFILE_EN > 0u
    p_tcb->SemPendTime        = (CPU_TS         )0u;
    p_tcb->SemPendTimeMax     = (CPU_TS         )0u;
#endif

    p_tcb->StkSize            = (CPU_STK_SIZE   )0u;


#if OS_CFG_TASK_SUSPEND_EN > 0u
    p_tcb->SuspendCtr         = (OS_NESTING_CTR )0u;
#endif

#if OS_CFG_STAT_TASK_STK_CHK_EN > 0u
    p_tcb->StkFree            = (CPU_STK_SIZE   )0u;
    p_tcb->StkUsed            = (CPU_STK_SIZE   )0u;
#endif

    p_tcb->Opt                = (OS_OPT         )0u;

    p_tcb->TickCtrPrev        = (OS_TICK        )OS_TICK_TH_INIT;
    p_tcb->TickCtrMatch       = (OS_TICK        )0u;
    p_tcb->TickRemain         = (OS_TICK        )0u;

    p_tcb->TimeQuanta         = (OS_TICK        )0u;
    p_tcb->TimeQuantaCtr      = (OS_TICK        )0u;

#if OS_CFG_TASK_PROFILE_EN > 0u
    p_tcb->CPUUsage           = (OS_CPU_USAGE   )0u;
    p_tcb->CPUUsageMax        = (OS_CPU_USAGE   )0u;
    p_tcb->CtxSwCtr           = (OS_CTX_SW_CTR  )0u;
    p_tcb->CyclesDelta        = (CPU_TS         )0u;
    ts                        = OS_TS_GET();                /* Read the current timestamp and save                    */
    p_tcb->CyclesStart        = ts;
    p_tcb->CyclesTotal        = (OS_CYCLES      )0u;
#endif
#ifdef CPU_CFG_INT_DIS_MEAS_EN
    p_tcb->IntDisTimeMax      = (CPU_TS         )0u;
#endif
#if OS_CFG_SCHED_LOCK_TIME_MEAS_EN > 0u
    p_tcb->SchedLockTimeMax   = (CPU_TS         )0u;
#endif

    p_tcb->PendOn             = (OS_STATE       )OS_TASK_PEND_ON_NOTHING;
    p_tcb->PendStatus         = (OS_STATUS      )OS_STATUS_PEND_OK;
    p_tcb->TaskState          = (OS_STATE       )OS_TASK_STATE_RDY;

    p_tcb->Prio               = (OS_PRIO        )OS_PRIO_INIT;

#if OS_CFG_DBG_EN > 0u
    p_tcb->DbgPrevPtr         = (OS_TCB        *)0;
    p_tcb->DbgNextPtr         = (OS_TCB        *)0;
    p_tcb->DbgNamePtr         = (CPU_CHAR      *)((void *)" ");
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                               POST MESSAGE TO A TASK
*
* Description: This function sends a message to a task
*
* Arguments  : p_tcb      is a pointer to the TCB of the task receiving a message.  If you specify a NULL pointer then
*                         the message will be posted to the task's queue of the calling task.  In other words, you'd be
*                         posting a message to yourself.
*
*              p_void     is a pointer to the message to send.
*
*              msg_size   is the size of the message sent (in #bytes)
*
*              opt        specifies whether the post will be FIFO or LIFO:
*
*                             OS_OPT_POST_FIFO       Post at the end   of the queue
*                             OS_OPT_POST_LIFO       Post at the front of the queue
*
*                             OS_OPT_POST_NO_SCHED   Do not run the scheduler after the post
*
*                          Note(s): 1) OS_OPT_POST_NO_SCHED can be added with one of the other options.
*
*
*              ts         is a timestamp indicating when the post occurred.
*
*              p_err      is a pointer to a variable that will hold the error code associated
*                         with the outcome of this call.  Errors can be:
*
*                             OS_ERR_NONE            The call was successful and the message was sent
*                             OS_ERR_MSG_POOL_EMPTY  If there are no more OS_MSGs available from the pool
*                             OS_ERR_Q_MAX           If the queue is full
*                             OS_ERR_STATE_INVALID   If the task is in an invalid state.  This should never happen
*                                                    and if it does, would be considered a system failure.
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_TASK_Q_EN > 0u                   //如果使能了任务消息队列
void  OS_TaskQPost (OS_TCB       *p_tcb,    //目标任务
                    void         *p_void,   //消息内容地址
                    OS_MSG_SIZE   msg_size, //消息长度
                    OS_OPT        opt,      //选项
                    CPU_TS        ts,       //时间戳
                    OS_ERR       *p_err)    //返回错误类型
{
    CPU_SR_ALLOC();  //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
									   //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
									   // SR（临界段关中断只需保存SR），开中断时将该值还原。

    OS_CRITICAL_ENTER();                                   //进入临界段
    if (p_tcb == (OS_TCB *)0) {                            //如果 p_tcb 为空
        p_tcb = OSTCBCurPtr;                               //目标任务为自身
    }
   *p_err  = OS_ERR_NONE;                                  //错误类型为“无错误”
    switch (p_tcb->TaskState) {                            //根据任务状态分类处理
        case OS_TASK_STATE_RDY:                            //如果目标任务没等待状态
        case OS_TASK_STATE_DLY:
        case OS_TASK_STATE_SUSPENDED:
        case OS_TASK_STATE_DLY_SUSPENDED:
             OS_MsgQPut(&p_tcb->MsgQ,                      //把消息放入任务消息队列
                        p_void,
                        msg_size,
                        opt,
                        ts,
                        p_err);
             OS_CRITICAL_EXIT();                           //退出临界段
             break;                                        //跳出

        case OS_TASK_STATE_PEND:                           //如果目标任务有等待状态 
        case OS_TASK_STATE_PEND_TIMEOUT:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             if (p_tcb->PendOn == OS_TASK_PEND_ON_TASK_Q) {//如果等的是任务消息队列
                 OS_Post((OS_PEND_OBJ *)0,                 //把消息发布给目标任务
                         p_tcb,
                         p_void,
                         msg_size,
                         ts);
                 OS_CRITICAL_EXIT_NO_SCHED();              //退出临界段（无调度）
                 if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0u) { //如果要调度任务
                     OSSched();                                    //调度任务
                 }
             } else {                                      //如果没在等待任务消息队列
                 OS_MsgQPut(&p_tcb->MsgQ,                  //把消息放入任务消息队列
                            p_void,                        
                            msg_size,
                            opt,
                            ts,
                            p_err);
                 OS_CRITICAL_EXIT();                      //退出临界段
             }
             break;                                       //跳出

        default:                                          //如果状态超出预期
             OS_CRITICAL_EXIT();                          //退出临界段
            *p_err = OS_ERR_STATE_INVALID;                //错误类型为“状态非法”
             break;                                       //跳出
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                               RESUME A SUSPENDED TASK
*
* Description: This function is called to resume a previously suspended task.  This is the only call that will remove an
*              explicit task suspension.
*
* Arguments  : p_tcb      Is a pointer to the task's OS_TCB to resume
*
*              p_err      Is a pointer to a variable that will contain an error code returned by this function
*
*                             OS_ERR_NONE                  if the requested task is resumed
*                             OS_ERR_STATE_INVALID         if the task is in an invalid state
*                             OS_ERR_TASK_RESUME_ISR       if you called this function from an ISR
*                             OS_ERR_TASK_RESUME_SELF      You cannot resume 'self'
*                             OS_ERR_TASK_NOT_SUSPENDED    if the task to resume has not been suspended
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_TASK_SUSPEND_EN > 0u           //如果使能了函数 OSTaskResume()
void  OS_TaskResume (OS_TCB  *p_tcb,      //任务控制块指针
                     OS_ERR  *p_err)      //返回错误类型
{
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。
    CPU_CRITICAL_ENTER();                                      //关中断
   *p_err  = OS_ERR_NONE;                                      //错误类型为“无错误”
    switch (p_tcb->TaskState) {                                //根据 p_tcb 的任务状态分类处理
        case OS_TASK_STATE_RDY:                                //如果状态中没有挂起状态
        case OS_TASK_STATE_DLY:
        case OS_TASK_STATE_PEND:
        case OS_TASK_STATE_PEND_TIMEOUT:
             CPU_CRITICAL_EXIT();                              //开中断
            *p_err = OS_ERR_TASK_NOT_SUSPENDED;                //错误类型为“任务未被挂起”
             break;                                            //跳出

        case OS_TASK_STATE_SUSPENDED:                          //如果是“挂起状态”
             OS_CRITICAL_ENTER_CPU_EXIT();                     //锁调度器，重开中断
             p_tcb->SuspendCtr--;                              //任务的挂起嵌套数减1
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {     //如果挂起前套数为0
                 p_tcb->TaskState = OS_TASK_STATE_RDY;         //修改状态为“就绪状态”
                 OS_TaskRdy(p_tcb);                            //把 p_tcb 插入就绪列表
             }
             OS_CRITICAL_EXIT_NO_SCHED();                      //开调度器，不调度任务
             break;                                            //跳出

        case OS_TASK_STATE_DLY_SUSPENDED:                      //如果是“延时中被挂起”
             p_tcb->SuspendCtr--;                              //任务的挂起嵌套数减1
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {     //如果挂起前套数为0
                 p_tcb->TaskState = OS_TASK_STATE_DLY;         //修改状态为“延时状态”
             }
             CPU_CRITICAL_EXIT();                              //开中断
             break;                                            //跳出

        case OS_TASK_STATE_PEND_SUSPENDED:                     //如果是“无期限等待中被挂起”
             p_tcb->SuspendCtr--;                              //任务的挂起嵌套数减1
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {     //如果挂起前套数为0
                 p_tcb->TaskState = OS_TASK_STATE_PEND;        //修改状态为“无期限等待状态”
             }
             CPU_CRITICAL_EXIT();                              //开中断
             break;                                            //跳出

        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:             //如果是“有期限等待中被挂起”
             p_tcb->SuspendCtr--;                              //任务的挂起嵌套数减1
             if (p_tcb->SuspendCtr == (OS_NESTING_CTR)0) {     //如果挂起前套数为0
                 p_tcb->TaskState = OS_TASK_STATE_PEND_TIMEOUT;//修改状态为“有期限等待状态”
             }
             CPU_CRITICAL_EXIT();                              //开中断
             break;                                            //跳出

        default:                                               //如果 p_tcb 任务状态超出预期
             CPU_CRITICAL_EXIT();                              //开中断
            *p_err = OS_ERR_STATE_INVALID;                     //错误类型为“状态非法”
             return;                                           //跳出
    }

    OSSched();                                                 //调度任务
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                              CATCH ACCIDENTAL TASK RETURN
*
* Description: This function is called if a task accidentally returns without deleting itself.  In other words, a task
*              should either be an infinite loop or delete itself if it's done.
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TaskReturn (void)
{
    OS_ERR  err;



    OSTaskReturnHook(OSTCBCurPtr);                          /* Call hook to let user decide on what to do             */
#if OS_CFG_TASK_DEL_EN > 0u
    OSTaskDel((OS_TCB *)0,                                  /* Delete task if it accidentally returns!                */
              (OS_ERR *)&err);
#else
    for (;;) {
        OSTimeDly((OS_TICK )OSCfg_TickRate_Hz,
                  (OS_OPT  )OS_OPT_TIME_DLY,
                  (OS_ERR *)&err);
    }
#endif
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                    SIGNAL A TASK
*
* Description: This function is called to signal a task waiting for a signal.
*
* Arguments  : p_tcb     is the pointer to the TCB of the task to signal.  A NULL pointer indicates that you are sending
*                        a signal to yourself.
*
*              opt       determines the type of POST performed:
*
*                             OS_OPT_POST_NONE         No option
*
*                             OS_OPT_POST_NO_SCHED     Do not call the scheduler
*
*              ts        is a timestamp indicating when the post occurred.
*
*              p_err     is a pointer to an error code returned by this function:
*
*                            OS_ERR_NONE           If the requested task is signaled
*                            OS_ERR_SEM_OVF        If the post would cause the semaphore count to overflow.
*                            OS_ERR_STATE_INVALID  If the task is in an invalid state.  This should never happen
*                                                  and if it does, would be considered a system failure.
*
* Returns    : The current value of the task's signal counter or 0 if called from an ISR
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

OS_SEM_CTR  OS_TaskSemPost (OS_TCB  *p_tcb,   //目标任务
                            OS_OPT   opt,     //选项
                            CPU_TS   ts,      //时间戳
                            OS_ERR  *p_err)   //返回错误类型
{
    OS_SEM_CTR  ctr;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

    OS_CRITICAL_ENTER();                               //进入临界段
    if (p_tcb == (OS_TCB *)0) {                        //如果 p_tcb 为空
        p_tcb = OSTCBCurPtr;                           //将任务信号量发给自己（任务）
    }
    p_tcb->TS = ts;                                    //记录信号量被发布的时间戳
   *p_err     = OS_ERR_NONE;                           //错误类型为“无错误”
    switch (p_tcb->TaskState) {                        //跟吴目标任务的任务状态分类处理
        case OS_TASK_STATE_RDY:                        //如果目标任务没有等待状态
        case OS_TASK_STATE_DLY:
        case OS_TASK_STATE_SUSPENDED:
        case OS_TASK_STATE_DLY_SUSPENDED:
             switch (sizeof(OS_SEM_CTR)) {                        //判断是否将导致该信
                 case 1u:                                         //号量计数值溢出，如
                      if (p_tcb->SemCtr == DEF_INT_08U_MAX_VAL) { //果溢出，则开中断，
                          OS_CRITICAL_EXIT();                     //返回错误类型为“计
                         *p_err = OS_ERR_SEM_OVF;                 //数值溢出”，返回0
                          return ((OS_SEM_CTR)0);                 //（有错误），不继续
                      }                                           //执行。
                      break;                                      

                 case 2u:
                      if (p_tcb->SemCtr == DEF_INT_16U_MAX_VAL) {
                          OS_CRITICAL_EXIT();
                         *p_err = OS_ERR_SEM_OVF;
                          return ((OS_SEM_CTR)0);
                      }
                      break;

                 case 4u:
                      if (p_tcb->SemCtr == DEF_INT_32U_MAX_VAL) {
                          OS_CRITICAL_EXIT();
                         *p_err = OS_ERR_SEM_OVF;
                          return ((OS_SEM_CTR)0);
                      }
                      break;

                 default:
                      break;
             }
             p_tcb->SemCtr++;                              //信号量计数值不溢出则加1
             ctr = p_tcb->SemCtr;                          //获取信号量的当前计数值
             OS_CRITICAL_EXIT();                           //退出临界段
             break;                                        //跳出

        case OS_TASK_STATE_PEND:                           //如果任务有等待状态
        case OS_TASK_STATE_PEND_TIMEOUT:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             if (p_tcb->PendOn == OS_TASK_PEND_ON_TASK_SEM) { //如果正等待任务信号量
                 OS_Post((OS_PEND_OBJ *)0,                    //发布信号量给目标任务
                         (OS_TCB      *)p_tcb,
                         (void        *)0,
                         (OS_MSG_SIZE  )0u,
                         (CPU_TS       )ts);
                 ctr = p_tcb->SemCtr;                         //获取信号量的当前计数值
                 OS_CRITICAL_EXIT_NO_SCHED();                 //退出临界段（无调度）
                 if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) { //如果选择了调度任务
                     OSSched();                               //调度任务
                 }
             } else {                                         //如果没等待任务信号量
                 switch (sizeof(OS_SEM_CTR)) {                         //判断是否将导致
                     case 1u:                                          //该信号量计数值
                          if (p_tcb->SemCtr == DEF_INT_08U_MAX_VAL) {  //溢出，如果溢出，
                              OS_CRITICAL_EXIT();                      //则开中断，返回
                             *p_err = OS_ERR_SEM_OVF;                  //错误类型为“计
                              return ((OS_SEM_CTR)0);                  //数值溢出”，返
                          }                                            //回0（有错误），
                          break;                                       //不继续执行。

                     case 2u:
                          if (p_tcb->SemCtr == DEF_INT_16U_MAX_VAL) {
                              OS_CRITICAL_EXIT();
                             *p_err = OS_ERR_SEM_OVF;
                              return ((OS_SEM_CTR)0);
                          }
                          break;

                     case 4u:
                          if (p_tcb->SemCtr == DEF_INT_32U_MAX_VAL) {
                              OS_CRITICAL_EXIT();
                             *p_err = OS_ERR_SEM_OVF;
                              return ((OS_SEM_CTR)0);
                          }
                          break;

                     default:
                          break;
                 }
                 p_tcb->SemCtr++;                            //信号量计数值不溢出则加1
                 ctr = p_tcb->SemCtr;                        //获取信号量的当前计数值
                 OS_CRITICAL_EXIT();                         //退出临界段
             }
             break;                                          //跳出

        default:                                             //如果任务状态超出预期
             OS_CRITICAL_EXIT();                             //退出临界段
            *p_err = OS_ERR_STATE_INVALID;                   //错误类型为“状态非法”
             ctr   = (OS_SEM_CTR)0;                          //清零 ctr
             break;                                          //跳出
    }
    return (ctr);                                            //返回信号量的当前计数值
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                   SUSPEND A TASK
*
* Description: This function is called to suspend a task.  The task can be the calling task if 'p_tcb' is a NULL pointer
*              or the pointer to the TCB of the calling task.
*
* Arguments  : p_tcb    is a pointer to the TCB to suspend.
*                       If p_tcb is a NULL pointer then, suspend the current task.
*
*              p_err    is a pointer to a variable that will receive an error code from this function.
*
*                           OS_ERR_NONE                      if the requested task is suspended
*                           OS_ERR_SCHED_LOCKED              you can't suspend the current task is the scheduler is
*                                                            locked
*                           OS_ERR_TASK_SUSPEND_ISR          if you called this function from an ISR
*                           OS_ERR_TASK_SUSPEND_IDLE         if you attempted to suspend the idle task which is not
*                                                            allowed.
*                           OS_ERR_TASK_SUSPEND_INT_HANDLER  if you attempted to suspend the idle task which is not
*                                                            allowed.
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application should not call it.
*
*              2) You should use this function with great care.  If you suspend a task that is waiting for an event
*                 (i.e. a message, a semaphore, a queue ...) you will prevent this task from running when the event
*                 arrives.
************************************************************************************************************************
*/

#if OS_CFG_TASK_SUSPEND_EN > 0u          //如果使能了函数 OSTaskSuspend()
void   OS_TaskSuspend (OS_TCB  *p_tcb,   //任务控制块指针
                       OS_ERR  *p_err)   //返回错误类型
{
    CPU_SR_ALLOC();  //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
									   //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
									   // SR（临界段关中断只需保存SR），开中断时将该值还原。

    CPU_CRITICAL_ENTER();                                   //关中断
    if (p_tcb == (OS_TCB *)0) {                             //如果 p_tcb 为空
        p_tcb = OSTCBCurPtr;                                //挂起自身
    }

    if (p_tcb == OSTCBCurPtr) {                             //如果是挂起自身
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {    //如果调度器被锁
            CPU_CRITICAL_EXIT();                            //开中断
           *p_err = OS_ERR_SCHED_LOCKED;                    //错误类型为“调度器被锁”
            return;                                         //返回，停止执行
        }
    }

   *p_err = OS_ERR_NONE;                                      //错误类型为“无错误”
    switch (p_tcb->TaskState) {                               //根据 p_tcb 的任务状态分类处理
        case OS_TASK_STATE_RDY:                               //如果是就绪状态
             OS_CRITICAL_ENTER_CPU_EXIT();                    //锁调度器，重开中断
             p_tcb->TaskState  =  OS_TASK_STATE_SUSPENDED;    //任务状态改为“挂起状态”
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;           //挂起前套数为1
             OS_RdyListRemove(p_tcb);                         //将任务从就绪列表移除
             OS_CRITICAL_EXIT_NO_SCHED();                     //开调度器，不进行调度
             break;                                           //跳出

        case OS_TASK_STATE_DLY:                               //如果是延时状态
             p_tcb->TaskState  = OS_TASK_STATE_DLY_SUSPENDED; //任务状态改为“延时中被挂起”
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;           //挂起前套数为1
             CPU_CRITICAL_EXIT();                             //开中断
             break;                                           //跳出

        case OS_TASK_STATE_PEND:                              //如果是无期限等待状态
             p_tcb->TaskState  = OS_TASK_STATE_PEND_SUSPENDED;//任务状态改为“无期限等待中被挂起”
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;           //挂起前套数为1
             CPU_CRITICAL_EXIT();                             //开中断
             break;                                           //跳出

        case OS_TASK_STATE_PEND_TIMEOUT:                      //如果是有期限等待状态
             p_tcb->TaskState  = OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED; //改为“有期限等待中被挂起”
             p_tcb->SuspendCtr = (OS_NESTING_CTR)1;           //挂起前套数为1
             CPU_CRITICAL_EXIT();                             //开中断
             break;                                           //跳出

        case OS_TASK_STATE_SUSPENDED:                         //如果状态中有挂起状态
        case OS_TASK_STATE_DLY_SUSPENDED:
        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             p_tcb->SuspendCtr++;                             //挂起嵌套数加1
             CPU_CRITICAL_EXIT();                             //开中断
             break;                                           //跳出

        default:                                              //如果任务状态超出预期
             CPU_CRITICAL_EXIT();                             //开中断
            *p_err = OS_ERR_STATE_INVALID;                    //错误类型为“状态非法”
             return;                                          //返回，停止执行
    } 

    OSSched();                                                //调度任务
}
#endif
