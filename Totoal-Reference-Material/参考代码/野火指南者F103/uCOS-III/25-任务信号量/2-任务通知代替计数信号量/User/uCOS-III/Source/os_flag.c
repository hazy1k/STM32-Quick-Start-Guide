/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                                EVENT FLAG MANAGEMENT
*
* File    : OS_FLAG.C
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
const  CPU_CHAR  *os_flag__c = "$Id: $";
#endif


#if OS_CFG_FLAG_EN > 0u

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 CREATE AN EVENT FLAG
*
* Description: This function is called to create an event flag group.
*
* Arguments  : p_grp          is a pointer to the event flag group to create
*
*              p_name         is the name of the event flag group
*
*              flags          contains the initial value to store in the event flag group (typically 0).
*
*              p_err          is a pointer to an error code which will be returned to your application:
*
*                                 OS_ERR_NONE                    if the call was successful.
*                                 OS_ERR_CREATE_ISR              if you attempted to create an Event Flag from an ISR.
*                                 OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the Event Flag after you
*                                                                   called OSSafetyCriticalStart().
*                                 OS_ERR_NAME                    if 'p_name' is a NULL pointer
*                                 OS_ERR_OBJ_CREATED             if the event flag group has already been created
*                                 OS_ERR_OBJ_PTR_NULL            if 'p_grp' is a NULL pointer
*
* Returns    : none
************************************************************************************************************************
*/

void  OSFlagCreate (OS_FLAG_GRP  *p_grp,  //事件标志组指针
                    CPU_CHAR     *p_name, //命名事件标志组
                    OS_FLAGS      flags,  //标志初始值
                    OS_ERR       *p_err)  //返回错误类型
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

#if OS_CFG_ARG_CHK_EN > 0u           //如果使能了参数检测
    if (p_grp == (OS_FLAG_GRP *)0) { //如果 p_grp 为空                                      
       *p_err = OS_ERR_OBJ_PTR_NULL; //错误类型为“创建对象为空”
        return;                      //返回，停止执行
    }
#endif

    OS_CRITICAL_ENTER();               //进入临界段
    p_grp->Type    = OS_OBJ_TYPE_FLAG; //标记创建对象数据结构为事件标志组
    p_grp->NamePtr = p_name;           //标记事件标志组的名称
    p_grp->Flags   = flags;            //设置标志初始值
    p_grp->TS      = (CPU_TS)0;        //清零事件标志组的时间戳
    OS_PendListInit(&p_grp->PendList); //初始化该事件标志组的等待列表  

#if OS_CFG_DBG_EN > 0u                 //如果使能了调试代码和变量
    OS_FlagDbgListAdd(p_grp);          //将该标志组添加到事件标志组双向调试链表
#endif
    OSFlagQty++;                       //事件标志组个数加1

    OS_CRITICAL_EXIT_NO_SCHED();       //退出临界段（无调度）
   *p_err = OS_ERR_NONE;               //错误类型为“无错误”
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                             DELETE AN EVENT FLAG GROUP
*
* Description: This function deletes an event flag group and readies all tasks pending on the event flag group.
*
* Arguments  : p_grp     is a pointer to the desired event flag group.
*
*              opt       determines delete options as follows:
*
*                            OS_OPT_DEL_NO_PEND           Deletes the event flag group ONLY if no task pending
*                            OS_OPT_DEL_ALWAYS            Deletes the event flag group even if tasks are waiting.
*                                                         In this case, all the tasks pending will be readied.
*
*              p_err     is a pointer to an error code that can contain one of the following values:
*
*                            OS_ERR_NONE                  The call was successful and the event flag group was deleted
*                            OS_ERR_DEL_ISR               If you attempted to delete the event flag group from an ISR
*                            OS_ERR_OBJ_PTR_NULL          If 'p_grp' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If you didn't pass a pointer to an event flag group
*                            OS_ERR_OPT_INVALID           An invalid option was specified
*                            OS_ERR_TASK_WAITING          One or more tasks were waiting on the event flag group.
*
* Returns    : == 0          if no tasks were waiting on the event flag group, or upon error.
*              >  0          if one or more tasks waiting on the event flag group are now readied and informed.
*
* Note(s)    : 1) This function must be used with care.  Tasks that would normally expect the presence of the event flag
*                 group MUST check the return code of OSFlagPost and OSFlagPend().
************************************************************************************************************************
*/

#if OS_CFG_FLAG_DEL_EN > 0u                 //如果使能了 OSFlagDel() 函数
OS_OBJ_QTY  OSFlagDel (OS_FLAG_GRP  *p_grp, //事件标志组指针
                       OS_OPT        opt,   //选项
                       OS_ERR       *p_err) //返回错误类型
{
    OS_OBJ_QTY        cnt;
    OS_OBJ_QTY        nbr_tasks;
    OS_PEND_DATA     *p_pend_data;
    OS_PEND_LIST     *p_pend_list;
    OS_TCB           *p_tcb;
    CPU_TS            ts;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return ((OS_OBJ_QTY)0);         //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数在中断中被调用
       *p_err = OS_ERR_DEL_ISR;                //错误类型为“在中断中删除对象”
        return ((OS_OBJ_QTY)0);                //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                //如果使能了参数检测
    if (p_grp == (OS_FLAG_GRP *)0) {      //如果 p_grp 为空 
       *p_err  = OS_ERR_OBJ_PTR_NULL;     //错误类型为“对象为空”
        return ((OS_OBJ_QTY)0);           //返回0（有错误），停止执行
    }
    switch (opt) {                        //根据选项分类处理
        case OS_OPT_DEL_NO_PEND:          //如果选项在预期内
        case OS_OPT_DEL_ALWAYS:
             break;                       //直接跳出

        default:                          //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;  //错误类型为“选项非法”
             return ((OS_OBJ_QTY)0);      //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u           //如果使能了对象类型检测
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) {//如果 p_grp 不是事件标志组类型
       *p_err = OS_ERR_OBJ_TYPE;          //错误类型为“对象类型有误”
        return ((OS_OBJ_QTY)0);           //返回0（有错误），停止执行
    }
#endif
    OS_CRITICAL_ENTER();                         //进入临界段
    p_pend_list = &p_grp->PendList;              //获取消息队列的等待列表
    cnt         = p_pend_list->NbrEntries;       //获取等待该队列的任务数
    nbr_tasks   = cnt;                           //按照任务数目逐个处理
    switch (opt) {                               //根据选项分类处理
        case OS_OPT_DEL_NO_PEND:                 //如果只在没任务等待时进行删除
             if (nbr_tasks == (OS_OBJ_QTY)0) {   //如果没有任务在等待该标志组
#if OS_CFG_DBG_EN > 0u                           //如果使能了调试代码和变量
                 OS_FlagDbgListRemove(p_grp);    //将该标志组从标志组调试列表移除
#endif
                 OSFlagQty--;                    //标志组数目减1
                 OS_FlagClr(p_grp);              //清除该标志组的内容

                 OS_CRITICAL_EXIT();             //退出临界段
                *p_err = OS_ERR_NONE;            //错误类型为“无错误”
             } else {
                 OS_CRITICAL_EXIT();             //退出临界段
                *p_err = OS_ERR_TASK_WAITING;    //错误类型为“有任务在等待标志组”
             }
             break;                              //跳出

        case OS_OPT_DEL_ALWAYS:                  //如果必须删除标志组
             ts = OS_TS_GET();                   //获取时间戳
             while (cnt > 0u) {                  //逐个移除该标志组等待列表中的任务
                 p_pend_data = p_pend_list->HeadPtr;
                 p_tcb       = p_pend_data->TCBPtr;
                 OS_PendObjDel((OS_PEND_OBJ *)((void *)p_grp),
                               p_tcb,
                               ts);
                 cnt--;
             }
#if OS_CFG_DBG_EN > 0u                           //如果使能了调试代码和变量 
             OS_FlagDbgListRemove(p_grp);        //将该标志组从标志组调试列表移除
#endif
             OSFlagQty--;                        //标志组数目减1
             OS_FlagClr(p_grp);                  //清除该标志组的内容
             OS_CRITICAL_EXIT_NO_SCHED();        //退出临界段（无调度）
             OSSched();                          //调度任务
            *p_err = OS_ERR_NONE;                //错误类型为“无错误”
             break;                              //跳出

        default:                                 //如果选项超出预期
             OS_CRITICAL_EXIT();                 //退出临界段
            *p_err = OS_ERR_OPT_INVALID;         //错误类型为“选项非法”
             break;                              //跳出
    }
    return (nbr_tasks);                          //返回删除标志组前等待其的任务数
}
#endif

/*
************************************************************************************************************************
*                                             WAIT ON AN EVENT FLAG GROUP
*
* Description: This function is called to wait for a combination of bits to be set in an event flag group.  Your
*              application can wait for ANY bit to be set or ALL bits to be set.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to wait for.
*                            The bits you want are specified by setting the corresponding bits in 'flags'.
*                            e.g. if your application wants to wait for bits 0 and 1 then 'flags' would contain 0x03.
*
*              timeout       is an optional timeout (in clock ticks) that your task will wait for the
*                            desired bit combination.  If you specify 0, however, your task will wait
*                            forever at the specified event flag group or, until a message arrives.
*
*              opt           specifies whether you want ALL bits to be set or ANY of the bits to be set.
*                            You can specify the 'ONE' of the following arguments:
*
*                                OS_OPT_PEND_FLAG_CLR_ALL   You will wait for ALL bits in 'flags' to be clear (0)
*                                OS_OPT_PEND_FLAG_CLR_ANY   You will wait for ANY bit  in 'flags' to be clear (0)
*                                OS_OPT_PEND_FLAG_SET_ALL   You will wait for ALL bits in 'flags' to be set   (1)
*                                OS_OPT_PEND_FLAG_SET_ANY   You will wait for ANY bit  in 'flags' to be set   (1)
*
*                            You can 'ADD' OS_OPT_PEND_FLAG_CONSUME if you want the event flag to be 'consumed' by
*                                      the call.  Example, to wait for any flag in a group AND then clear
*                                      the flags that are present, set 'wait_opt' to:
*
*                                      OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME
*
*                            You can also 'ADD' the type of pend with 'ONE' of the two option:
*
*                                OS_OPT_PEND_NON_BLOCKING   Task will NOT block if flags are not available
*                                OS_OPT_PEND_BLOCKING       Task will     block if flags are not available
*
*              p_ts          is a pointer to a variable that will receive the timestamp of when the event flag group was
*                            posted, aborted or the event flag group deleted.  If you pass a NULL pointer (i.e. (CPU_TS *)0)
*                            then you will not get the timestamp.  In other words, passing a NULL pointer is valid and
*                            indicates that you don't need the timestamp.
*
*              p_err         is a pointer to an error code and can be:
*
*                                OS_ERR_NONE                The desired bits have been set within the specified 'timeout'
*                                OS_ERR_OBJ_PTR_NULL        If 'p_grp' is a NULL pointer.
*                                OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                                OS_ERR_OPT_INVALID         You didn't specify a proper 'opt' argument.
*                                OS_ERR_PEND_ABORT          The wait on the flag was aborted.
*                                OS_ERR_PEND_ISR            If you tried to PEND from an ISR
*                                OS_ERR_PEND_WOULD_BLOCK    If you specified non-blocking but the flags were not
*                                                           available.
*                                OS_ERR_SCHED_LOCKED        If you called this function when the scheduler is locked
*                                OS_ERR_TIMEOUT             The bit(s) have not been set in the specified 'timeout'.
*
* Returns    : The flags in the event flag group that made the task ready or, 0 if a timeout or an error
*              occurred.
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPend (OS_FLAG_GRP  *p_grp,   //事件标志组指针
                      OS_FLAGS      flags,   //选定要操作的标志位
                      OS_TICK       timeout, //等待期限（单位：时钟节拍）
                      OS_OPT        opt,     //选项
                      CPU_TS       *p_ts,    //返回等到事件标志时的时间戳
                      OS_ERR       *p_err)   //返回错误类型
{
    CPU_BOOLEAN   consume;
    OS_FLAGS      flags_rdy;
    OS_OPT        mode;
    OS_PEND_DATA  pend_data;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return ((OS_FLAGS)0);           //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u          //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {  //如果该函数在中断中被调用
       *p_err = OS_ERR_PEND_ISR;                //错误类型为“在中断中中止等待”
        return ((OS_FLAGS)0);                   //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u               //如果使能了参数检测
    if (p_grp == (OS_FLAG_GRP *)0) {     //如果 p_grp 为空
       *p_err = OS_ERR_OBJ_PTR_NULL;     //错误类型为“对象为空”
        return ((OS_FLAGS)0);            //返回0（有错误），停止执行
    }
    switch (opt) {                       //根据选项分类处理
        case OS_OPT_PEND_FLAG_CLR_ALL:   //如果选项在预期内
        case OS_OPT_PEND_FLAG_CLR_ANY:
        case OS_OPT_PEND_FLAG_SET_ALL:
        case OS_OPT_PEND_FLAG_SET_ANY:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
        case OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING:
             break;                     //直接跳出

        default:                        //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;//错误类型为“选项非法”
             return ((OS_OBJ_QTY)0);    //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u            //如果使能了对象类型检测
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) { //如果 p_grp 不是事件标志组类型 
       *p_err = OS_ERR_OBJ_TYPE;           //错误类型为“对象类型有误”
        return ((OS_FLAGS)0);              //返回0（有错误），停止执行
    }
#endif

    if ((opt & OS_OPT_PEND_FLAG_CONSUME) != (OS_OPT)0) { //选择了标志位匹配后自动取反
        consume = DEF_TRUE;
    } else {                                             //未选择标志位匹配后自动取反
        consume = DEF_FALSE;
    }

    if (p_ts != (CPU_TS *)0) {      //如果 p_ts 非空
       *p_ts = (CPU_TS)0;           //初始化（清零）p_ts，待用于返回时间戳
    }

    mode = opt & OS_OPT_PEND_FLAG_MASK;                    //从选项中提取对标志位的要求
    CPU_CRITICAL_ENTER();                                  //关中断
    switch (mode) {                                        //根据事件触发模式分类处理
        case OS_OPT_PEND_FLAG_SET_ALL:                     //如果要求所有标志位均要置1
             flags_rdy = (OS_FLAGS)(p_grp->Flags & flags); //提取想要的标志位的值
             if (flags_rdy == flags) {                     //如果该值与期望值匹配
                 if (consume == DEF_TRUE) {                //如果要求将标志位匹配后取反
                     p_grp->Flags &= ~flags_rdy;           //清0标志组的相关标志位
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;        //保存让任务脱离等待的标志值
                 if (p_ts != (CPU_TS *)0) {                //如果 p_ts 非空
                    *p_ts  = p_grp->TS;                    //获取任务等到标志组时的时间戳
                 }
                 CPU_CRITICAL_EXIT();                      //开中断        
                *p_err = OS_ERR_NONE;                      //错误类型为“无错误”
                 return (flags_rdy);                       //返回让任务脱离等待的标志值
             } else {                                      //如果想要标志位的值与期望值不匹配                  
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) { //如果选择了不堵塞任务
                     CPU_CRITICAL_EXIT();                  //关中断
                    *p_err = OS_ERR_PEND_WOULD_BLOCK;      //错误类型为“渴求堵塞”  
                     return ((OS_FLAGS)0);                 //返回0（有错误），停止执行
                 } else {                                  //如果选择了堵塞任务
                     if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //如果调度器被锁
                         CPU_CRITICAL_EXIT();              //关中断
                        *p_err = OS_ERR_SCHED_LOCKED;      //错误类型为“调度器被锁”
                         return ((OS_FLAGS)0);             //返回0（有错误），停止执行
                     }
                 }
                 /* 如果调度器未被锁 */
                 OS_CRITICAL_ENTER_CPU_EXIT();             //进入临界段，重开中断           
                 OS_FlagBlock(&pend_data,                  //阻塞当前运行任务，等待事件标志组
                              p_grp,
                              flags,
                              opt,
                              timeout);
                 OS_CRITICAL_EXIT_NO_SCHED();              //退出临界段（无调度）
             }
             break;                                        //跳出

        case OS_OPT_PEND_FLAG_SET_ANY:                     //如果要求有标志位被置1即可
             flags_rdy = (OS_FLAGS)(p_grp->Flags & flags); //提取想要的标志位的值
             if (flags_rdy != (OS_FLAGS)0) {               //如果有位被置1         
                 if (consume == DEF_TRUE) {                //如果要求将标志位匹配后取反             
                     p_grp->Flags &= ~flags_rdy;           //清0湿巾标志组的相关标志位          
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;        //保存让任务脱离等待的标志值
                 if (p_ts != (CPU_TS *)0) {                //如果 p_ts 非空
                    *p_ts  = p_grp->TS;                    //获取任务等到标志组时的时间戳
                 }
                 CPU_CRITICAL_EXIT();                      //开中断                
                *p_err = OS_ERR_NONE;                      //错误类型为“无错误”
                 return (flags_rdy);                       //返回让任务脱离等待的标志值
             } else {                                      //如果没有位被置1                          
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) { //如果没设置堵塞任务
                     CPU_CRITICAL_EXIT();                  //关中断
                    *p_err = OS_ERR_PEND_WOULD_BLOCK;      //错误类型为“渴求堵塞”     
                     return ((OS_FLAGS)0);                 //返回0（有错误），停止执行
                 } else {                                  //如果设置了堵塞任务          
                     if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //如果调度器被锁
                         CPU_CRITICAL_EXIT();              //关中断
                        *p_err = OS_ERR_SCHED_LOCKED;      //错误类型为“调度器被锁”
                         return ((OS_FLAGS)0);             //返回0（有错误），停止执行
                     }
                 }
                 /* 如果调度器没被锁 */                                    
                 OS_CRITICAL_ENTER_CPU_EXIT();             //进入临界段，重开中断             
                 OS_FlagBlock(&pend_data,                  //阻塞当前运行任务，等待事件标志组
                              p_grp,
                              flags,
                              opt,
                              timeout);
                 OS_CRITICAL_EXIT_NO_SCHED();              //退出中断（无调度）
             }
             break;                                        //跳出

#if OS_CFG_FLAG_MODE_CLR_EN > 0u                           //如果使能了标志位清0触发模式
        case OS_OPT_PEND_FLAG_CLR_ALL:                     //如果要求所有标志位均要清0
             flags_rdy = (OS_FLAGS)(~p_grp->Flags & flags);//提取想要的标志位的值
             if (flags_rdy == flags) {                     //如果该值与期望值匹配
                 if (consume == DEF_TRUE) {                //如果要求将标志位匹配后清0
                     p_grp->Flags |= flags_rdy;            //置1标志组的相关标志位
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;        //保存让任务脱离等待的标志值
                 if (p_ts != (CPU_TS *)0) {                //如果 p_ts 非空
                    *p_ts  = p_grp->TS;                    //获取任务等到标志组时的时间戳
                 }
                 CPU_CRITICAL_EXIT();                      //开中断
                *p_err = OS_ERR_NONE;                      //错误类型为“无错误”
                 return (flags_rdy);                       //返回0（有错误），停止执行
             } else {                                      //如果想要标志位的值与期望值不匹配
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {  //如果选择了不堵塞任务
                     CPU_CRITICAL_EXIT();                  //关中断
                    *p_err = OS_ERR_PEND_WOULD_BLOCK;      //错误类型为“渴求堵塞”
                     return ((OS_FLAGS)0);                 //返回0（有错误），停止执行
                 } else {                                  //如果选择了堵塞任务
                     if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //如果调度器被锁
                         CPU_CRITICAL_EXIT();              //关中断           
                        *p_err = OS_ERR_SCHED_LOCKED;      //错误类型为“调度器被锁”
                         return ((OS_FLAGS)0);             //返回0（有错误），停止执行
                     }
                 }
                 /* 如果调度器未被锁 */                                          
                 OS_CRITICAL_ENTER_CPU_EXIT();             //进入临界段，重开中断      
                 OS_FlagBlock(&pend_data,                  //阻塞当前运行任务，等待事件标志组
                              p_grp,
                              flags,
                              opt,
                              timeout);
                 OS_CRITICAL_EXIT_NO_SCHED();             //退出临界段（无调度）
             }
             break;                                       //跳出

        case OS_OPT_PEND_FLAG_CLR_ANY:                    //如果要求有标志位被清0即可
             flags_rdy = (OS_FLAGS)(~p_grp->Flags & flags);//提取想要的标志位的值
             if (flags_rdy != (OS_FLAGS)0) {              //如果有位被清0
                 if (consume == DEF_TRUE) {               //如果要求将标志位匹配后取反 
                     p_grp->Flags |= flags_rdy;           //置1湿巾标志组的相关标志位  
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;       //保存让任务脱离等待的标志值 
                 if (p_ts != (CPU_TS *)0) {               //如果 p_ts 非空
                    *p_ts  = p_grp->TS;                   //获取任务等到标志组时的时间戳
                 }
                 CPU_CRITICAL_EXIT();                     //开中断 
                *p_err = OS_ERR_NONE;                     //错误类型为“无错误”
                 return (flags_rdy);                      //返回0（有错误），停止执行
             } else {                                     //如果没有位被清0
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) { //如果没设置堵塞任务
                     CPU_CRITICAL_EXIT();                 //开中断
                    *p_err = OS_ERR_PEND_WOULD_BLOCK;     //错误类型为“渴求堵塞”
                     return ((OS_FLAGS)0);                //返回0（有错误），停止执行
                 } else {                                 //如果设置了堵塞任务  
                     if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) { //如果调度器被锁
                         CPU_CRITICAL_EXIT();             //开中断
                        *p_err = OS_ERR_SCHED_LOCKED;     //错误类型为“调度器被锁”
                         return ((OS_FLAGS)0);            //返回0（有错误），停止执行
                     }
                 }
                 /* 如果调度器没被锁 */                                          
                 OS_CRITICAL_ENTER_CPU_EXIT();            //进入临界段，重开中断
                 OS_FlagBlock(&pend_data,                 //阻塞当前运行任务，等待事件标志组
                              p_grp,
                              flags,
                              opt,
                              timeout);
                 OS_CRITICAL_EXIT_NO_SCHED();             //退出中断（无调度）
             }
             break;                                       //跳出
#endif

        default:                                          //如果要求超出预期
             CPU_CRITICAL_EXIT();
            *p_err = OS_ERR_OPT_INVALID;                  //错误类型为“选项非法”
             return ((OS_FLAGS)0);                        //返回0（有错误），停止执行
    }

    OSSched();                                            //任务调度
    /* 任务等到了事件标志组后得以继续运行 */
    CPU_CRITICAL_ENTER();                                 //关中断
    switch (OSTCBCurPtr->PendStatus) {                    //根据运行任务的等待状态分类处理
        case OS_STATUS_PEND_OK:                           //如果等到了事件标志组
             if (p_ts != (CPU_TS *)0) {                   //如果 p_ts 非空
                *p_ts  = OSTCBCurPtr->TS;                 //返回等到标志组时的时间戳
             }
            *p_err = OS_ERR_NONE;                         //错误类型为“无错误”
             break;                                       //跳出

        case OS_STATUS_PEND_ABORT:                        //如果等待被中止
             if (p_ts != (CPU_TS *)0) {                   //如果 p_ts 非空
                *p_ts  = OSTCBCurPtr->TS;                 //返回等待被中止时的时间戳
             }
             CPU_CRITICAL_EXIT();                         //开中断
            *p_err = OS_ERR_PEND_ABORT;                   //错误类型为“等待被中止”
             break;                                       //跳出

        case OS_STATUS_PEND_TIMEOUT:                      //如果等待超时
             if (p_ts != (CPU_TS *)0) {                   //如果 p_ts 非空
                *p_ts  = (CPU_TS  )0;                     //清零 p_ts
             }
             CPU_CRITICAL_EXIT();                         //开中断
            *p_err = OS_ERR_TIMEOUT;                      //错误类型为“超时”
             break;                                       //跳出

        case OS_STATUS_PEND_DEL:                          //如果等待对象被删除
             if (p_ts != (CPU_TS *)0) {                   //如果 p_ts 非空
                *p_ts  = OSTCBCurPtr->TS;                 //返回对象被删时的时间戳
             }
             CPU_CRITICAL_EXIT();                         //开中断
            *p_err = OS_ERR_OBJ_DEL;                      //错误类型为“对象被删”
             break;                                       //跳出

        default:                                          //如果等待状态超出预期
             CPU_CRITICAL_EXIT();                         //开中断
            *p_err = OS_ERR_STATUS_INVALID;               //错误类型为“状态非法”
             break;                                       //跳出
    }
    if (*p_err != OS_ERR_NONE) {                          //如果有错误存在
        return ((OS_FLAGS)0);                             //返回0（有错误），停止执行
    }
    /* 如果没有错误存在 */
    flags_rdy = OSTCBCurPtr->FlagsRdy;                    //读取让任务脱离等待的标志值
    if (consume == DEF_TRUE) {                            //如果需要取反触发事件的标志位
        switch (mode) {                                   //根据事件触发模式分类处理
            case OS_OPT_PEND_FLAG_SET_ALL:                //如果是通过置1来标志事件的发生
            case OS_OPT_PEND_FLAG_SET_ANY:                                           
                 p_grp->Flags &= ~flags_rdy;              //清0标志组里触发事件的标志位
                 break;                                   //跳出

#if OS_CFG_FLAG_MODE_CLR_EN > 0u                          //如果使能了标志位清0触发模式
            case OS_OPT_PEND_FLAG_CLR_ALL:                //如果是通过清0来标志事件的发生
            case OS_OPT_PEND_FLAG_CLR_ANY:                 
                 p_grp->Flags |=  flags_rdy;              //置1标志组里触发事件的标志位
                 break;                                   //跳出
#endif
            default:                                      //如果触发模式超出预期
                 CPU_CRITICAL_EXIT();                     //开中断
                *p_err = OS_ERR_OPT_INVALID;              //错误类型为“选项非法”
                 return ((OS_FLAGS)0);                    //返回0（有错误），停止执行
        }
    }
    CPU_CRITICAL_EXIT();                                  //开中断
   *p_err = OS_ERR_NONE;                                  //错误类型为“无错误”
    return (flags_rdy);                                   //返回让任务脱离等待的标志值
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                          ABORT WAITING ON AN EVENT FLAG GROUP
*
* Description: This function aborts & readies any tasks currently waiting on an event flag group.  This function should
*              be used to fault-abort the wait on the event flag group, rather than to normally post to the event flag
*              group OSFlagPost().
*
* Arguments  : p_grp     is a pointer to the event flag group
*
*              opt       determines the type of ABORT performed:
*
*                            OS_OPT_PEND_ABORT_1          ABORT wait for a single task (HPT) waiting on the event flag
*                            OS_OPT_PEND_ABORT_ALL        ABORT wait for ALL tasks that are  waiting on the event flag
*                            OS_OPT_POST_NO_SCHED         Do not call the scheduler
*
*              p_err     is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE                  At least one task waiting on the event flag group and was
*                                                         readied and informed of the aborted wait; check return value
*                                                         for the number of tasks whose wait on the event flag group
*                                                         was aborted.
*                            OS_ERR_OBJ_PTR_NULL          If 'p_grp' is a NULL pointer.
*                            OS_ERR_OBJ_TYPE              If 'p_grp' is not pointing at an event flag group
*                            OS_ERR_OPT_INVALID           If you specified an invalid option
*                            OS_ERR_PEND_ABORT_ISR        If you called this function from an ISR
*                            OS_ERR_PEND_ABORT_NONE       No task were pending
*
* Returns    : == 0          if no tasks were waiting on the event flag group, or upon error.
*              >  0          if one or more tasks waiting on the event flag group are now readied and informed.
************************************************************************************************************************
*/

#if OS_CFG_FLAG_PEND_ABORT_EN > 0u                //如果使能了 OSFlagPendAbort() 函数
OS_OBJ_QTY  OSFlagPendAbort (OS_FLAG_GRP  *p_grp, //事件标志组指针
                             OS_OPT        opt,   //选项
                             OS_ERR       *p_err) //返回错误类型
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    OS_OBJ_QTY     nbr_tasks;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {          //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();  //执行安全检测异常函数
        return ((OS_OBJ_QTY)0u);         //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u           //如果使能了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0u) {  //如果该函数是在中断中被调用
       *p_err = OS_ERR_PEND_ABORT_ISR;           //错误类型为“在中断中创建对象”
        return ((OS_OBJ_QTY)0u);                 //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                 //如果使能了参数检测
    if (p_grp == (OS_FLAG_GRP *)0) {       //如果 p_grp 为空
       *p_err  =  OS_ERR_OBJ_PTR_NULL;     //错误类型为“创建对象为空”
        return ((OS_OBJ_QTY)0u);           //返回0（有错误），停止执行
    }
    switch (opt) {                         //根据选项分类处理  
        case OS_OPT_PEND_ABORT_1:          //如果选项在预期内
        case OS_OPT_PEND_ABORT_ALL:
        case OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED:
        case OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED:
             break;                        //直接跳出

        default:                           //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;   //错误类型为“选项非法”
             return ((OS_OBJ_QTY)0u);      //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u              //如果使能了对象类型检测
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) {   //如果 p_grp 不是事件标志组类型  
       *p_err = OS_ERR_OBJ_TYPE;             //错误类型为“对象类型有误” 
        return ((OS_OBJ_QTY)0u);             //返回0（有错误），停止执行
    }
#endif

    CPU_CRITICAL_ENTER();                            //关中断
    p_pend_list = &p_grp->PendList;                  //获取消息队列的等待列表
    if (p_pend_list->NbrEntries == (OS_OBJ_QTY)0u) { //如果没有任务在等待
        CPU_CRITICAL_EXIT();                         //开中断
       *p_err = OS_ERR_PEND_ABORT_NONE;              //错误类型为“没任务在等待”
        return ((OS_OBJ_QTY)0u);                     //返回0（有错误），停止执行
    }
    /* 如果有任务在等待 */
    OS_CRITICAL_ENTER_CPU_EXIT();                     //进入临界段，重开中断 
    nbr_tasks = 0u;                                   //准备计数被中止的等待任务
    ts        = OS_TS_GET();                          //获取时间戳
    while (p_pend_list->NbrEntries > (OS_OBJ_QTY)0u) {//如果还有任务在等待
        p_tcb = p_pend_list->HeadPtr->TCBPtr;         //获取头端（最高优先级）任务
        OS_PendAbort((OS_PEND_OBJ *)((void *)p_grp),  //中止该任务对 p_grp 的等待
                     p_tcb,
                     ts);
        nbr_tasks++;
        if (opt != OS_OPT_PEND_ABORT_ALL) {           //如果不是选择了中止所有等待任务
            break;                                    //立即跳出，不再继续中止
        }
    }
    OS_CRITICAL_EXIT_NO_SCHED();                      //退出临界段（无调度）

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
*                                       GET FLAGS WHO CAUSED TASK TO BECOME READY
*
* Description: This function is called to obtain the flags that caused the task to become ready to run.
*              In other words, this function allows you to tell "Who done it!".
*
* Arguments  : p_err     is a pointer to an error code
*
*                            OS_ERR_NONE       if the call was successful
*                            OS_ERR_PEND_ISR   if called from an ISR
*
* Returns    : The flags that caused the task to be ready.
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPendGetFlagsRdy (OS_ERR  *p_err)
{
    OS_FLAGS   flags;
    CPU_SR_ALLOC();



#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_FLAGS)0);
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {              /* See if called from ISR ...                             */
       *p_err = OS_ERR_PEND_ISR;                            /* ... can't get from an ISR                              */
        return ((OS_FLAGS)0);
    }
#endif

    CPU_CRITICAL_ENTER();
    flags = OSTCBCurPtr->FlagsRdy;
    CPU_CRITICAL_EXIT();
   *p_err = OS_ERR_NONE;
    return (flags);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                POST EVENT FLAG BIT(S)
*
* Description: This function is called to set or clear some bits in an event flag group.  The bits to set or clear are
*              specified by a 'bit mask'.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         If 'opt' (see below) is OS_OPT_POST_FLAG_SET, each bit that is set in 'flags' will
*                            set the corresponding bit in the event flag group.  e.g. to set bits 0, 4
*                            and 5 you would set 'flags' to:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*                            If 'opt' (see below) is OS_OPT_POST_FLAG_CLR, each bit that is set in 'flags' will
*                            CLEAR the corresponding bit in the event flag group.  e.g. to clear bits 0,
*                            4 and 5 you would specify 'flags' as:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*              opt           indicates whether the flags will be:
*
*                                OS_OPT_POST_FLAG_SET       set
*                                OS_OPT_POST_FLAG_CLR       cleared
*
*                            you can also 'add' OS_OPT_POST_NO_SCHED to prevent the scheduler from being called.
*
*              p_err         is a pointer to an error code and can be:
*
*                                OS_ERR_NONE                The call was successful
*                                OS_ERR_OBJ_PTR_NULL        You passed a NULL pointer
*                                OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                                OS_ERR_OPT_INVALID         You specified an invalid option
*
* Returns    : the new value of the event flags bits that are still set.
*
* Note(s)    : 1) The execution time of this function depends on the number of tasks waiting on the event flag group.
************************************************************************************************************************
*/

OS_FLAGS  OSFlagPost (OS_FLAG_GRP  *p_grp, //事件标志组指针
                      OS_FLAGS      flags, //选定要操作的标志位
                      OS_OPT        opt,   //选项
                      OS_ERR       *p_err) //返回错误类型
{
    OS_FLAGS  flags_cur;
    CPU_TS    ts;



#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return ((OS_FLAGS)0);           //返回0，停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u               //如果使能（默认使能）了参数检测
    if (p_grp == (OS_FLAG_GRP *)0) {     //如果参数 p_grp 为空 
       *p_err  = OS_ERR_OBJ_PTR_NULL;    //错误类型为“事件标志组对象为空”
        return ((OS_FLAGS)0);            //返回0，停止执行
    }
    switch (opt) {                       //根据选项分类处理
        case OS_OPT_POST_FLAG_SET:       //如果选项在预期之内
        case OS_OPT_POST_FLAG_CLR:
        case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED:
        case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED:
             break;                      //直接跳出

        default:                         //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID; //错误类型为“选项非法”
             return ((OS_FLAGS)0);       //返回0，停止执行
    }
#endif

#if OS_CFG_OBJ_TYPE_CHK_EN > 0u            //如果使能了对象类型检测
    if (p_grp->Type != OS_OBJ_TYPE_FLAG) { //如果 p_grp 不是事件标志组类型
       *p_err = OS_ERR_OBJ_TYPE;           //错误类型“对象类型有误”
        return ((OS_FLAGS)0);              //返回0，停止执行
    }
#endif

    ts = OS_TS_GET();                             //获取时间戳              
#if OS_CFG_ISR_POST_DEFERRED_EN > 0u              //如果使能了中断延迟发布
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) {    //如果该函数是在中断中被调用
        OS_IntQPost((OS_OBJ_TYPE)OS_OBJ_TYPE_FLAG,//将该标志组发布到中断消息队列
                    (void      *)p_grp,
                    (void      *)0,
                    (OS_MSG_SIZE)0,
                    (OS_FLAGS   )flags,
                    (OS_OPT     )opt,
                    (CPU_TS     )ts,
                    (OS_ERR    *)p_err);
        return ((OS_FLAGS)0);                     //返回0，停止执行
    }
#endif
    /* 如果没有使能中断延迟发布 */
    flags_cur = OS_FlagPost(p_grp,               //将标志组直接发布
                            flags,
                            opt,
                            ts,
                            p_err);

    return (flags_cur);                         //返回当前标志位的值
}

/*$PAGE*/
/*
************************************************************************************************************************
*                         SUSPEND TASK UNTIL EVENT FLAG(s) RECEIVED OR TIMEOUT OCCURS
*
* Description: This function is internal to uC/OS-III and is used to put a task to sleep until the desired
*              event flag bit(s) are set.
*
* Arguments  : p_pend_data    is a pointer to an object used to link the task being blocked to the list of task(s)
*              -----------    pending on the desired event flag group.
*
*              p_grp         is a pointer to the desired event flag group.
*              -----
*
*              flags         Is a bit pattern indicating which bit(s) (i.e. flags) you wish to check.
*                            The bits you want are specified by setting the corresponding bits in
*                            'flags'.  e.g. if your application wants to wait for bits 0 and 1 then
*                            'flags' would contain 0x03.
*
*              opt           specifies whether you want ALL bits to be set/cleared or ANY of the bits
*                            to be set/cleared.
*                            You can specify the following argument:
*
*                                OS_OPT_PEND_FLAG_CLR_ALL   You will check ALL bits in 'mask' to be clear (0)
*                                OS_OPT_PEND_FLAG_CLR_ANY   You will check ANY bit  in 'mask' to be clear (0)
*                                OS_OPT_PEND_FLAG_SET_ALL   You will check ALL bits in 'mask' to be set   (1)
*                                OS_OPT_PEND_FLAG_SET_ANY   You will check ANY bit  in 'mask' to be set   (1)
*
*              timeout       is the desired amount of time that the task will wait for the event flag
*                            bit(s) to be set.
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_FlagBlock (OS_PEND_DATA  *p_pend_data,  //等待列表元素
                    OS_FLAG_GRP   *p_grp,        //事件标志组
                    OS_FLAGS       flags,        //要操作的标志位
                    OS_OPT         opt,          //选项
                    OS_TICK        timeout)      //等待期限
{
    OSTCBCurPtr->FlagsPend = flags;          //保存需要等待的标志位              
    OSTCBCurPtr->FlagsOpt  = opt;            //保存对标志位的要求
    OSTCBCurPtr->FlagsRdy  = (OS_FLAGS)0;

    OS_Pend(p_pend_data,                     //阻塞任务，等待事件标志组
            (OS_PEND_OBJ *)((void *)p_grp),
             OS_TASK_PEND_ON_FLAG,
             timeout);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                      CLEAR THE CONTENTS OF AN EVENT FLAG GROUP
*
* Description: This function is called by OSFlagDel() to clear the contents of an event flag group
*

* Argument(s): p_grp     is a pointer to the event flag group to clear
*              -----
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_FlagClr (OS_FLAG_GRP  *p_grp)
{
    OS_PEND_LIST  *p_pend_list;



    p_grp->Type             = OS_OBJ_TYPE_NONE;
    p_grp->NamePtr          = (CPU_CHAR *)((void *)"?FLAG");    /* Unknown name                                       */
    p_grp->Flags            = (OS_FLAGS )0;
    p_pend_list             = &p_grp->PendList;
    OS_PendListInit(p_pend_list);
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                          INITIALIZE THE EVENT FLAG MODULE
*
* Description: This function is called by uC/OS-III to initialize the event flag module.  Your application MUST NOT call
*              this function.  In other words, this function is internal to uC/OS-III.
*
* Arguments  : p_err     is a pointer to an error code that can contain one of the following values:
*
*                            OS_ERR_NONE   The call was successful.
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_FlagInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSFlagDbgListPtr = (OS_FLAG_GRP *)0;
#endif

    OSFlagQty        = (OS_OBJ_QTY   )0;
   *p_err            = OS_ERR_NONE;
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                    ADD/REMOVE EVENT FLAG GROUP TO/FROM DEBUG LIST
*
* Description: These functions are called by uC/OS-III to add or remove an event flag group from the event flag debug
*              list.
*
* Arguments  : p_grp     is a pointer to the event flag group to add/remove
*
* Returns    : none
*
* Note(s)    : These functions are INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u                        //如果使能（默认使能）了调试代码和变量
void  OS_FlagDbgListAdd (OS_FLAG_GRP  *p_grp) //将事件标志组插入到事件标志组调试列表的最前端
{
    p_grp->DbgNamePtr                = (CPU_CHAR    *)((void *)" "); //先不指向任何任务的名称
    p_grp->DbgPrevPtr                = (OS_FLAG_GRP *)0;             //将该标志组作为列表的最前端
    if (OSFlagDbgListPtr == (OS_FLAG_GRP *)0) {                      //如果列表为空
        p_grp->DbgNextPtr            = (OS_FLAG_GRP *)0;             //该队列的下一个元素也为空
    } else {                                                         //如果列表非空
        p_grp->DbgNextPtr            =  OSFlagDbgListPtr;            //列表原来的首元素作为该队列的下一个元素
        OSFlagDbgListPtr->DbgPrevPtr =  p_grp;                       //原来的首元素的前面变为了该队列
    }
    OSFlagDbgListPtr                 =  p_grp;                       //该标志组成为列表的新首元素
}



void  OS_FlagDbgListRemove (OS_FLAG_GRP  *p_grp)
{
    OS_FLAG_GRP  *p_grp_next;
    OS_FLAG_GRP  *p_grp_prev;


    p_grp_prev = p_grp->DbgPrevPtr;
    p_grp_next = p_grp->DbgNextPtr;

    if (p_grp_prev == (OS_FLAG_GRP *)0) {
        OSFlagDbgListPtr = p_grp_next;
        if (p_grp_next != (OS_FLAG_GRP *)0) {
            p_grp_next->DbgPrevPtr = (OS_FLAG_GRP *)0;
        }
        p_grp->DbgNextPtr = (OS_FLAG_GRP *)0;

    } else if (p_grp_next == (OS_FLAG_GRP *)0) {
        p_grp_prev->DbgNextPtr = (OS_FLAG_GRP *)0;
        p_grp->DbgPrevPtr      = (OS_FLAG_GRP *)0;

    } else {
        p_grp_prev->DbgNextPtr =  p_grp_next;
        p_grp_next->DbgPrevPtr =  p_grp_prev;
        p_grp->DbgNextPtr      = (OS_FLAG_GRP *)0;
        p_grp->DbgPrevPtr      = (OS_FLAG_GRP *)0;
    }
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                                POST EVENT FLAG BIT(S)
*
* Description: This function is called to set or clear some bits in an event flag group.  The bits to set or clear are
*              specified by a 'bit mask'.
*
* Arguments  : p_grp         is a pointer to the desired event flag group.
*
*              flags         If 'opt' (see below) is OS_OPT_POST_FLAG_SET, each bit that is set in 'flags' will
*                            set the corresponding bit in the event flag group.  e.g. to set bits 0, 4
*                            and 5 you would set 'flags' to:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*                            If 'opt' (see below) is OS_OPT_POST_FLAG_CLR, each bit that is set in 'flags' will
*                            CLEAR the corresponding bit in the event flag group.  e.g. to clear bits 0,
*                            4 and 5 you would specify 'flags' as:
*
*                                0x31     (note, bit 0 is least significant bit)
*
*              opt           indicates whether the flags will be:
*
*                                OS_OPT_POST_FLAG_SET       set
*                                OS_OPT_POST_FLAG_CLR       cleared
*
*                            you can also 'add' OS_OPT_POST_NO_SCHED to prevent the scheduler from being called.
*
*              ts            is the timestamp of the post
*
*              p_err         is a pointer to an error code and can be:
*
*                                OS_ERR_NONE                The call was successful
*                                OS_ERR_OBJ_PTR_NULL        You passed a NULL pointer
*                                OS_ERR_OBJ_TYPE            You are not pointing to an event flag group
*                                OS_ERR_OPT_INVALID         You specified an invalid option
*
* Returns    : the new value of the event flags bits that are still set.
*
* Note(s)    : 1) The execution time of this function depends on the number of tasks waiting on the event flag group.
************************************************************************************************************************
*/

OS_FLAGS  OS_FlagPost (OS_FLAG_GRP  *p_grp, //事件标志组指针
                       OS_FLAGS      flags, //选定要操作的标志位
                       OS_OPT        opt,   //选项
                       CPU_TS        ts,    //时间戳
                       OS_ERR       *p_err) //返回错误类型
{
    OS_FLAGS        flags_cur;
    OS_FLAGS        flags_rdy;
    OS_OPT          mode;
    OS_PEND_DATA   *p_pend_data;
    OS_PEND_DATA   *p_pend_data_next;
    OS_PEND_LIST   *p_pend_list;
    OS_TCB         *p_tcb;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。 
	
    CPU_CRITICAL_ENTER();                                //关中断
    switch (opt) {                                       //根据选项分类处理
        case OS_OPT_POST_FLAG_SET:                       //如果要求将选定位置1
        case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED:
             p_grp->Flags |=  flags;                     //将选定位置1
             break;                                      //跳出

        case OS_OPT_POST_FLAG_CLR:                       //如果要求将选定位请0
        case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED:
             p_grp->Flags &= ~flags;                     //将选定位请0
             break;                                      //跳出

        default:                                         //如果选项超出预期
             CPU_CRITICAL_EXIT();                        //开中断
            *p_err = OS_ERR_OPT_INVALID;                 //错误类型为“选项非法”
             return ((OS_FLAGS)0);                       //返回0，停止执行
    }
    p_grp->TS   = ts;                                    //将时间戳存入事件标志组
    p_pend_list = &p_grp->PendList;                      //获取事件标志组的等待列表
    if (p_pend_list->NbrEntries == 0u) {                 //如果没有任务在等待标志组
        CPU_CRITICAL_EXIT();                             //开中断
       *p_err = OS_ERR_NONE;                             //错误类型为“无错误”
        return (p_grp->Flags);                           //返回事件标志组的标志值
    }
    /* 如果有任务在等待标志组 */
    OS_CRITICAL_ENTER_CPU_EXIT();                     //进入临界段，重开中断
    p_pend_data = p_pend_list->HeadPtr;               //获取等待列表头个等待任务
    p_tcb       = p_pend_data->TCBPtr;
    while (p_tcb != (OS_TCB *)0) {                    //从头至尾遍历等待列表的所有任务
        p_pend_data_next = p_pend_data->NextPtr;
        mode             = p_tcb->FlagsOpt & OS_OPT_PEND_FLAG_MASK; //获取任务的标志选项
        switch (mode) {                               //根据任务的标志选项分类处理
            case OS_OPT_PEND_FLAG_SET_ALL:            //如果要求任务等待的标志位都得置1
                 flags_rdy = (OS_FLAGS)(p_grp->Flags & p_tcb->FlagsPend); 
                 if (flags_rdy == p_tcb->FlagsPend) { //如果任务等待的标志位都置1了
                     OS_FlagTaskRdy(p_tcb,            //让该任务准备运行
                                    flags_rdy,
                                    ts);
                 }
                 break;                               //跳出

            case OS_OPT_PEND_FLAG_SET_ANY:            //如果要求任务等待的标志位有1位置1即可
                 flags_rdy = (OS_FLAGS)(p_grp->Flags & p_tcb->FlagsPend);
                 if (flags_rdy != (OS_FLAGS)0) {      //如果任务等待的标志位有置1的
                     OS_FlagTaskRdy(p_tcb,            //让该任务准备运行
                                    flags_rdy,
                                    ts);
                 }
                 break;                              //跳出

#if OS_CFG_FLAG_MODE_CLR_EN > 0u                     //如果使能了标志位清0触发模式
            case OS_OPT_PEND_FLAG_CLR_ALL:           //如果要求任务等待的标志位都得请0
                 flags_rdy = (OS_FLAGS)(~p_grp->Flags & p_tcb->FlagsPend);
                 if (flags_rdy == p_tcb->FlagsPend) {//如果任务等待的标志位都请0了
                     OS_FlagTaskRdy(p_tcb,           //让该任务准备运行
                                    flags_rdy,
                                    ts);
                 }
                 break;            //跳出

            case OS_OPT_PEND_FLAG_CLR_ANY:          //如果要求任务等待的标志位有1位请0即可
                 flags_rdy = (OS_FLAGS)(~p_grp->Flags & p_tcb->FlagsPend);
                 if (flags_rdy != (OS_FLAGS)0) {    //如果任务等待的标志位有请0的
                     OS_FlagTaskRdy(p_tcb,          //让该任务准备运行
                                    flags_rdy,
                                    ts);
                 }
                 break;                            //跳出
#endif
            default:                               //如果标志选项超出预期
                 OS_CRITICAL_EXIT();               //退出临界段
                *p_err = OS_ERR_FLAG_PEND_OPT;     //错误类型为“标志选项非法”
                 return ((OS_FLAGS)0);             //返回0，停止运行
        }
        p_pend_data = p_pend_data_next;            //准备处理下一个等待任务
        if (p_pend_data != (OS_PEND_DATA *)0) {    //如果该任务存在
            p_tcb = p_pend_data->TCBPtr;           //获取该任务的任务控制块
        } else {                                   //如果该任务不存在
            p_tcb = (OS_TCB *)0;                   //清空 p_tcb，退出 while 循环
        }
    }
    OS_CRITICAL_EXIT_NO_SCHED();                  //退出临界段（无调度）

    if ((opt & OS_OPT_POST_NO_SCHED) == (OS_OPT)0) {  //如果 opt 没选择“发布时不调度任务”
        OSSched();                                    //任务调度
    }

    CPU_CRITICAL_ENTER();        //关中断
    flags_cur = p_grp->Flags;    //获取事件标志组的标志值
    CPU_CRITICAL_EXIT();         //开中断
   *p_err     = OS_ERR_NONE;     //错误类型为“无错误”
    return (flags_cur);          //返回事件标志组的当前标志值
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                        MAKE TASK READY-TO-RUN, EVENT(s) OCCURRED
*
* Description: This function is internal to uC/OS-III and is used to make a task ready-to-run because the desired event
*              flag bits have been set.
*
* Arguments  : p_tcb         is a pointer to the OS_TCB of the task to remove
*              -----
*
*              flags_rdy     contains the bit pattern of the event flags that cause the task to become ready-to-run.
*
*              ts            is a timestamp associated with the post
*
* Returns    : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void   OS_FlagTaskRdy (OS_TCB    *p_tcb,            //任务控制块指针
                       OS_FLAGS   flags_rdy,        //让任务就绪的标志位    
                       CPU_TS     ts)               //事件标志组被发布时的时间戳
{
    p_tcb->FlagsRdy   = flags_rdy;                  //标记让任务就绪的事件标志位
    p_tcb->PendStatus = OS_STATUS_PEND_OK;          //清除任务的等待状态
    p_tcb->PendOn     = OS_TASK_PEND_ON_NOTHING;    //标记任务没有等待任何对象
    p_tcb->TS         = ts;                         //记录任务脱离等待时的时间戳
    switch (p_tcb->TaskState) {                     //根据任务的任务状态分类处理
        case OS_TASK_STATE_RDY:                     //如果任务是就绪状态
        case OS_TASK_STATE_DLY:                     //如果任务是延时状态
        case OS_TASK_STATE_DLY_SUSPENDED:           //如果任务是延时中被挂起状态
        case OS_TASK_STATE_SUSPENDED:               //如果任务是被挂起状态
             break;                                 //直接跳出，不需处理

        case OS_TASK_STATE_PEND:                    //如果任务是无期限等待状态
        case OS_TASK_STATE_PEND_TIMEOUT:            //如果任务是有期限等待状态
             OS_TaskRdy(p_tcb);                     //让任务进入就绪状态
             p_tcb->TaskState = OS_TASK_STATE_RDY;  //修改任务的状态为就绪状态
             break;                                 //跳出

        case OS_TASK_STATE_PEND_SUSPENDED:              //如果任务是无期限等待中被挂起状态
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:      //如果任务是有期限等待中被挂起状态
             p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;//修改任务的状态为被挂起状态
             break;                                     //跳出

        default:                                        //如果任务状态超出预期
             break;                                     //直接跳出
    }
    OS_PendListRemove(p_tcb);                           //将任务从等待列表移除       
}
#endif
