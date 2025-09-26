/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                               PEND ON MULTIPLE OBJECTS
*
* File    : OS_PEND_MULTI.C
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
const  CPU_CHAR  *os_pend_multi__c = "$Id: $";
#endif


#if (((OS_CFG_Q_EN > 0u) || (OS_CFG_SEM_EN > 0u)) && (OS_CFG_PEND_MULTI_EN > 0u))
/*
************************************************************************************************************************
*                                               PEND ON MULTIPLE OBJECTS
*
* Description: This function pends on multiple objects.  The objects pended on MUST be either semaphores or message
*              queues.  If multiple objects are ready at the start of the pend call, then all available objects that
*              are ready will be indicated to the caller.  If the task must pend on the multiple events then, as soon
*              as one of the object is either posted, aborted or deleted, the task will be readied.
*
*              This function only allows you to pend on semaphores and/or message queues.
*
* Arguments  : p_pend_data_tbl   is a pointer to an array of type OS_PEND_DATA which contains a list of all the
*                                objects we will be waiting on.  The caller must declare an array of OS_PEND_DATA
*                                and initialize the .PendObjPtr (see below) with a pointer to the object (semaphore or
*                                message queue) to pend on.
*
*                                    OS_PEND_DATA  MyPendArray[?];
*
*                                The OS_PEND_DATA field are as follows:
*
*                                    OS_PEND_DATA  *PrevPtr;      Used to link OS_PEND_DATA objects
*                                    OS_PEND_DATA  *NextPtr;      Used to link OS_PEND_DATA objects
*                                    OS_TCB        *TCBPtr;       Pointer to the TCB that is pending on multiple objects
*                                    OS_PEND_OBJ   *PendObjPtr;   USER supplied field which is a pointer to the
*                                                                 semaphore or message queue you want to pend on.  When
*                                                                 you call OSPendMulti() you MUST fill this field for
*                                                                 each of the objects you want to pend on.
*                                    OS_PEND_OBJ   *RdyObjPtr;    OSPendMulti() will return the object that was posted,
*                                                                 aborted or deleted in this field.
*                                    void          *RdyMsgPtr;    OSPendMulti() will fill in this field if the object
*                                                                 posted was a message queue.  This corresponds to the
*                                                                 message posted.
*                                    OS_MSG_SIZE    RdyMsgSize;   OSPendMulti() will fill in this field if the object
*                                                                 posted was a message queue.  This corresponds to the
*                                                                 size of the message posted.
*                                    CPU_TS         RdyTS;        OSPendMulti() will fill in this field if the object
*                                                                 was a message queue.  This corresponds to the time
*                                                                 stamp when the message was posted.  However, if the
*                                                                 object is a semaphore and the object is already ready
*                                                                 the this field will be set to (CPU_TS)0 because it's
*                                                                 not possible to know when the semaphore was posted.
*
*              tbl_size      is the size (in number of elements) of the OS_PEND_DATA array passed to this function.  In
*                            other words, if the called needs to pend on 4 separate objects (semaphores and/or queues)
*                            then you would pass 4 to this call.
*
*              timeout       is an optional timeout period (in clock ticks).  If non-zero, your task will wait any of
*                            the objects up to the amount of time specified by this argument. If you specify 0, however,
*                            your task will wait forever for the specified objects or, until an object is posted,
*                            aborted or deleted.
*
*              opt           determines whether the user wants to block if none of the objects are available.
*
*                                OS_OPT_PEND_BLOCKING
*                                OS_OPT_PEND_NON_BLOCKING
*
*              p_err         is a pointer to where an error message will be deposited.  Possible error messages are:
*
*                                OS_ERR_NONE              The call was successful and your task owns the resources or,
*                                                         the objects you are waiting for occurred. Check the .RdyObjPtr
*                                                         fields to know which objects have been posted.
*                                OS_ERR_OBJ_TYPE          If any of the .PendPtr is NOT a semaphore or a message queue
*                                OS_ERR_OPT_INVALID       If you specified an invalid option for 'opt'
*                                OS_ERR_PEND_ABORT        The wait on the events was aborted; check the .RdyObjPtr fields
*                                                         for which objects were aborted.
*                                OS_ERR_PEND_DEL          The wait on the events was aborted; check the .RdyObjPtr fields
*                                                         for which objects were aborted.
*                                OS_ERR_PEND_ISR          If you called this function from an ISR
*                                OS_ERR_PEND_LOCKED       If you called this function when the scheduler is locked.
*                                OS_ERR_PEND_WOULD_BLOCK  If the caller didn't want to block and no object ready
*                                OS_ERR_STATUS_INVALID    Invalid pend status
*                                OS_ERR_PTR_INVALID       If you passes a NULL pointer of 'p_pend_data_tbl'
*                                OS_ERR_TIMEOUT           The objects were not posted within the specified 'timeout'.
*
* Returns    : >  0          the number of objects returned as ready, aborted or deleted
*              == 0          if no events are returned as ready because of timeout or upon error.
************************************************************************************************************************
*/
/*$PAGE*/
OS_OBJ_QTY  OSPendMulti (OS_PEND_DATA  *p_pend_data_tbl,  //等待对象（数组）
                         OS_OBJ_QTY     tbl_size,         //等待对象的数目
                         OS_TICK        timeout,          //超时（单位：时钟借配）
                         OS_OPT         opt,              //选项
                         OS_ERR        *p_err)            //返回错误类型
{
    CPU_BOOLEAN   valid;
    OS_OBJ_QTY    nbr_obj_rdy;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和定义一个局部变
                    //量，用于保存关中断前的 CPU 状态寄存器 SR（临界段关中断只需保存SR）
                    //，开中断时将该值还原。 
	
#ifdef OS_SAFETY_CRITICAL               //如果使能（默认禁用）了安全检测
    if (p_err == (OS_ERR *)0) {         //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION(); //执行安全检测异常函数
        return ((OS_OBJ_QTY)0);         //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_CALLED_FROM_ISR_CHK_EN > 0u         //如果使能（默认使能）了中断中非法调用检测
    if (OSIntNestingCtr > (OS_NESTING_CTR)0) { //如果该函数是在中断中被调用
       *p_err = OS_ERR_PEND_ISR;               //错误类型为“在中断函数中定时”
        return ((OS_OBJ_QTY)0);                //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                       //如果使能（默认使能）了参数检测
    if (p_pend_data_tbl == (OS_PEND_DATA *)0) {  //如果参数 p_pend_data_tbl 为空
       *p_err = OS_ERR_PTR_INVALID;              //错误类型为“等待对象不可用”
        return ((OS_OBJ_QTY)0);                  //返回0（有错误），停止执行
    }
    if (tbl_size == (OS_OBJ_QTY)0) {             //如果 tbl_size 为0
       *p_err = OS_ERR_PTR_INVALID;              //错误类型为“等待对象不可用”
        return ((OS_OBJ_QTY)0);                  //返回0（有错误），停止执行
    }
    switch (opt) {                               //根据选项分类处理
        case OS_OPT_PEND_BLOCKING:               //如果选项在预期内
        case OS_OPT_PEND_NON_BLOCKING:
             break;                              //直接跳出

        default:                                 //如果选项超出预期
            *p_err = OS_ERR_OPT_INVALID;         //错误类型为“选项非法”
             return ((OS_OBJ_QTY)0);             //返回0（有错误），停止执行
    }
#endif
    /* 证实等待对象是否只有多值信号量或消息队列 */ 
    valid = OS_PendMultiValidate(p_pend_data_tbl,          
                                 tbl_size);
    if (valid == DEF_FALSE) {              //如果等待对象不是只有多值信号量或消息队列  
       *p_err = OS_ERR_OBJ_TYPE;           //错误类型为“对象类型有误”                     
        return ((OS_OBJ_QTY)0);            //返回0（有错误），停止执行
    }
    /* 如果等待对象确实只有多值信号量或消息队列 */ 
/*$PAGE*/
    CPU_CRITICAL_ENTER();                                 //关中断
    nbr_obj_rdy = OS_PendMultiGetRdy(p_pend_data_tbl,     //查看是否有对象被提交了
                                     tbl_size);
    if (nbr_obj_rdy > (OS_OBJ_QTY)0) {                    //如果有对象被发布
        CPU_CRITICAL_EXIT();                              //开中断
       *p_err = OS_ERR_NONE;                              //错误类型为“无错误”
        return ((OS_OBJ_QTY)nbr_obj_rdy);                 //返回被发布的等待对象的数目
    }
    /* 如果目前没有对象被发布 */
    if ((opt & OS_OPT_PEND_NON_BLOCKING) != (OS_OPT)0) {  //如果选择了不阻塞任务
        CPU_CRITICAL_EXIT();                              //开中断
       *p_err = OS_ERR_PEND_WOULD_BLOCK;                  //错误类型为“渴望阻塞”
        return ((OS_OBJ_QTY)0);                           //返回0（有错误），停止执行
    } else {                                              //如果选择了阻塞任务
        if (OSSchedLockNestingCtr > (OS_NESTING_CTR)0) {  //如果调度器被锁
            CPU_CRITICAL_EXIT();                          //开中断
           *p_err = OS_ERR_SCHED_LOCKED;                  //错误类型为“调度器被锁”
            return ((OS_OBJ_QTY)0);                       //返回0（有错误），停止执行
        }
    }
                                                            
    OS_CRITICAL_ENTER_CPU_EXIT();                         //锁调度器，重开中断
                                                           
    OS_PendMultiWait(p_pend_data_tbl,                     //挂起当前任务等到有对象被发布或超时
                     tbl_size,
                     timeout);

    OS_CRITICAL_EXIT_NO_SCHED();                          //解锁调度器（无调度）

    OSSched();                                            //调度任务
    /* 任务等到了（一个）对象后得以继续运行 */
    CPU_CRITICAL_ENTER();                                 //关中断
    switch (OSTCBCurPtr->PendStatus) {                    //根据当前任务的等待状态分类处理
        case OS_STATUS_PEND_OK:                           //如果任务已经等到了对象
            *p_err = OS_ERR_NONE;                         //错误类型为“无错误”
             break;                                       //跳出

        case OS_STATUS_PEND_ABORT:                        //如果任务的等待被中止
            *p_err = OS_ERR_PEND_ABORT;                   //错误类型为“等待对象被中止”
             break;                                       //跳出

        case OS_STATUS_PEND_TIMEOUT:                      //如果等待超时
            *p_err = OS_ERR_TIMEOUT;                      //错误类型为“等待超时”
             break;                                       //跳出

        case OS_STATUS_PEND_DEL:                          //如果任务等待的对象被删除
            *p_err = OS_ERR_OBJ_DEL;                      //错误类型为“等待对象被删”
            break;                                        //跳出

        default:                                          //如果任务的等待状态超出预期
            *p_err = OS_ERR_STATUS_INVALID;               //错误类型为“状态非法”
             break;                                       //跳出
    }

    OSTCBCurPtr->PendStatus = OS_STATUS_PEND_OK;         //复位任务的等待状态
    CPU_CRITICAL_EXIT();                                 //开中断

    return ((OS_OBJ_QTY)1);                              //返回被发布对象数目为1
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                              GET A LIST OF OBJECTS READY
*
* Description: This function is called by OSPendMulti() to obtain the list of object that are ready.
*
* Arguments  : p_pend_data_tbl   is a pointer to an array of OS_PEND_DATA
*              ---------------
*
*              tbl_size          is the size of the array
*
* Returns    :  > 0              the number of objects ready
*              == 0              if no object ready
*
* Note       : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

OS_OBJ_QTY  OS_PendMultiGetRdy (OS_PEND_DATA  *p_pend_data_tbl, //等待对象
                                OS_OBJ_QTY     tbl_size)        //等待对象数目
{
    OS_OBJ_QTY   i;
    OS_OBJ_QTY   nbr_obj_rdy;
#if OS_CFG_Q_EN > 0u
    OS_ERR       err;
    OS_MSG_SIZE  msg_size;
    OS_Q        *p_q;
    void        *p_void;
    CPU_TS       ts;
#endif
#if OS_CFG_SEM_EN  > 0u
    OS_SEM      *p_sem;
#endif



    nbr_obj_rdy = (OS_OBJ_QTY)0;
    for (i = 0u; i < tbl_size; i++) {                          //逐个检测等待对象
        p_pend_data_tbl->RdyObjPtr  = (OS_PEND_OBJ  *)0;       //清零等待对象的所有数据
        p_pend_data_tbl->RdyMsgPtr  = (void         *)0;
        p_pend_data_tbl->RdyMsgSize = (OS_MSG_SIZE   )0;
        p_pend_data_tbl->RdyTS      = (CPU_TS        )0;
        p_pend_data_tbl->NextPtr    = (OS_PEND_DATA *)0;
        p_pend_data_tbl->PrevPtr    = (OS_PEND_DATA *)0;
        p_pend_data_tbl->TCBPtr     = (OS_TCB       *)0;
#if OS_CFG_Q_EN > 0u                                           //如果使能了消息队列          
        p_q = (OS_Q *)((void *)p_pend_data_tbl->PendObjPtr);   //将该对象视为消息队列处理
        if (p_q->Type == OS_OBJ_TYPE_Q) {                      //如果该对象确为消息队列类型
            p_void = OS_MsgQGet(&p_q->MsgQ,                    //从该消息队列获取消息
                                &msg_size,
                                &ts,
                                &err);
            if (err == OS_ERR_NONE) {                          //如果获取消息成功
                p_pend_data_tbl->RdyObjPtr  = p_pend_data_tbl->PendObjPtr;
                p_pend_data_tbl->RdyMsgPtr  = p_void;          //保存接收到的消息
                p_pend_data_tbl->RdyMsgSize = msg_size;
                p_pend_data_tbl->RdyTS      = ts;
                nbr_obj_rdy++;
            }
        }
#endif

#if OS_CFG_SEM_EN > 0u                                           //如果使能了多值信号量
        p_sem = (OS_SEM *)((void *)p_pend_data_tbl->PendObjPtr); //将该对象视为多值信号量处理
        if (p_sem->Type == OS_OBJ_TYPE_SEM) {                    //如果该对象确为多值信号量类型
            if (p_sem->Ctr > 0u) {                               //如果该信号量可用
                p_sem->Ctr--;                                    //等待任务可以获得信号量
                p_pend_data_tbl->RdyObjPtr  = p_pend_data_tbl->PendObjPtr;
                p_pend_data_tbl->RdyTS      = p_sem->TS;
                nbr_obj_rdy++;
            }
        }
#endif

        p_pend_data_tbl++;                                       //检测下一个等待对象
    }
    return (nbr_obj_rdy);                                        //返回被发布的对象的数目
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                 VERIFY THAT OBJECTS PENDED ON ARE EITHER SEMAPHORES or QUEUES
*
* Description: This function is called by OSPendMulti() to verify that we are multi-pending on either semaphores or
*              message queues.
*
* Arguments  : p_pend_data_tbl    is a pointer to an array of OS_PEND_DATA
*              ---------------
*
*              tbl_size           is the size of the array
*
* Returns    : TRUE               if all objects pended on are either semaphores of queues
*              FALSE              if at least one object is not a semaphore or queue.
*
* Note       : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

CPU_BOOLEAN  OS_PendMultiValidate (OS_PEND_DATA  *p_pend_data_tbl, //等待对象
                                   OS_OBJ_QTY     tbl_size)        //等待对象数目
{
    OS_OBJ_QTY  i;
    OS_OBJ_QTY  ctr;
#if OS_CFG_SEM_EN  > 0u
    OS_SEM      *p_sem;
#endif
#if OS_CFG_Q_EN > 0u
    OS_Q        *p_q;
#endif


    for (i = 0u; i < tbl_size; i++) {                           //逐个判断等待对象
        if (p_pend_data_tbl->PendObjPtr == (OS_PEND_OBJ *)0) {  //如果等待对象不存在
            return (DEF_FALSE);                                 //返回，验证失败
        }

        ctr = 0u;                                               //置0 ctr
#if OS_CFG_SEM_EN  > 0u                                         //如果使能了多值信号量
        p_sem = (OS_SEM *)((void *)p_pend_data_tbl->PendObjPtr);//获取等待对象
        if (p_sem->Type == OS_OBJ_TYPE_SEM) {                   //如果该对象是多值信号量类型        
            ctr++;                                              //ctr 为1
        }
#endif

#if OS_CFG_Q_EN > 0u                                            //如果使能了消息队列
        p_q = (OS_Q *)((void *)p_pend_data_tbl->PendObjPtr);    //获取等待对象
        if (p_q->Type == OS_OBJ_TYPE_Q) {                       //如果该对象是消息队列类型 
            ctr++;                                              //ctr 为1
        }
#endif

        if (ctr == (OS_OBJ_QTY)0) {                             //如果 ctr = 0
            return (DEF_FALSE);                                 //返回，验证失败
        }
        p_pend_data_tbl++;                                      //验证下一个等待对象
    }
    return (DEF_TRUE);                                          //返回，验证成功
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                 MAKE TASK WAIT FOR ANY OF MULTIPLE EVENTS TO OCCUR
*
* Description: This function is called by OSPendMulti() to suspend a task because any one of multiple objects that have
*              not been posted to.
*
* Arguments  : p_pend_data_tbl    is a pointer to an array of OS_PEND_DATA
*              ---------------
*
*              tbl_size           is the size of the array
*
*              timeout            is the timeout to wait in case none of the objects become ready
*
* Returns    : none
*
* Note       : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_PendMultiWait (OS_PEND_DATA  *p_pend_data_tbl, //等待对象
                        OS_OBJ_QTY     tbl_size,        //等待对象数目
                        OS_TICK        timeout)         //超时（单位：时钟节拍）
{
    OS_OBJ_QTY      i;
    OS_PEND_LIST   *p_pend_list;

#if OS_CFG_Q_EN > 0u
    OS_Q           *p_q;
#endif

#if OS_CFG_SEM_EN > 0u
    OS_SEM         *p_sem;
#endif



    OSTCBCurPtr->PendOn             = OS_TASK_PEND_ON_MULTI;   //等待对象不可用，开始等待
    OSTCBCurPtr->PendStatus         = OS_STATUS_PEND_OK;
    OSTCBCurPtr->PendDataTblEntries = tbl_size;
    OSTCBCurPtr->PendDataTblPtr     = p_pend_data_tbl;

    OS_TaskBlock(OSTCBCurPtr,                                   //阻塞当前运行任务
                 timeout);                                    

    for (i = 0u; i < tbl_size; i++) {                           //逐个将等待对象插入等待列表
        p_pend_data_tbl->TCBPtr = OSTCBCurPtr;                  //所有的等待对象都绑定当前任务

#if OS_CFG_SEM_EN > 0u                                          //如果使能了多值信号量
        p_sem = (OS_SEM *)((void *)p_pend_data_tbl->PendObjPtr);//将对象视为多值信号量处理
        if (p_sem->Type == OS_OBJ_TYPE_SEM) {                   //如果该对象确为多值信号量
            p_pend_list = &p_sem->PendList;                     //获取该信号量的等待列表
            OS_PendListInsertPrio(p_pend_list,                  //将当前任务插入该等待列表
                                  p_pend_data_tbl);
        }
#endif

#if OS_CFG_Q_EN > 0u                                            //如果使能了消息队列
        p_q = (OS_Q *)((void *)p_pend_data_tbl->PendObjPtr);    //将对象视为消息队列处理
        if (p_q->Type == OS_OBJ_TYPE_Q) {                       //如果该对象确为消息队列
            p_pend_list = &p_q->PendList;                       //获取该消息队列的等待列表
            OS_PendListInsertPrio(p_pend_list,                  //将当前任务插入该等待列表
                                  p_pend_data_tbl);
        }
#endif

        p_pend_data_tbl++;                                      //处理下一个等待对象
    }
}

#endif
