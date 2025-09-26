/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                             MEMORY PARTITION MANAGEMENT
*
* File    : OS_MEM.C
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
const  CPU_CHAR  *os_mem__c = "$Id: $";
#endif


#if OS_CFG_MEM_EN > 0u
/*
************************************************************************************************************************
*                                               CREATE A MEMORY PARTITION
*
* Description : Create a fixed-sized memory partition that will be managed by uC/OS-III.
*
* Arguments   : p_mem    is a pointer to a memory partition control block which is allocated in user memory space.
*
*               p_name   is a pointer to an ASCII string to provide a name to the memory partition.
*
*               p_addr   is the starting address of the memory partition
*
*               n_blks   is the number of memory blocks to create from the partition.
*
*               blk_size is the size (in bytes) of each block in the memory partition.
*
*               p_err    is a pointer to a variable containing an error message which will be set by this function to
*                        either:
*
*                            OS_ERR_NONE                    if the memory partition has been created correctly.
*                            OS_ERR_ILLEGAL_CREATE_RUN_TIME if you are trying to create the memory partition after you
*                                                             called OSSafetyCriticalStart().
*                            OS_ERR_MEM_INVALID_BLKS        user specified an invalid number of blocks (must be >= 2)
*                            OS_ERR_MEM_INVALID_P_ADDR      if you are specifying an invalid address for the memory
*                                                           storage of the partition or, the block does not align on a
*                                                           pointer boundary
*                            OS_ERR_MEM_INVALID_SIZE        user specified an invalid block size
*                                                             - must be greater than the size of a pointer
*                                                             - must be able to hold an integral number of pointers
* Returns    : none
************************************************************************************************************************
*/

void  OSMemCreate (OS_MEM       *p_mem,    //内存分区控制块
                   CPU_CHAR     *p_name,   //命名内存分区
                   void         *p_addr,   //内存分区首地址
                   OS_MEM_QTY    n_blks,   //内存块数目
                   OS_MEM_SIZE   blk_size, //内存块大小（单位：字节）
                   OS_ERR       *p_err)    //返回错误类型
{
#if OS_CFG_ARG_CHK_EN > 0u      
    CPU_DATA       align_msk;
#endif
    OS_MEM_QTY     i;
    OS_MEM_QTY     loops;
    CPU_INT08U    *p_blk;
    void         **p_link;               //二级指针，存放指针的指针
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {          //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();  //执行安全检测异常函数
        return;                          //返回，停止执行
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
       *p_err = OS_ERR_MEM_CREATE_ISR;         //错误类型为“在中断中创建对象”
        return;                                //返回，停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                             //如果使能了参数检测
    if (p_addr == (void *)0) {                         //如果 p_addr 为空      
       *p_err   = OS_ERR_MEM_INVALID_P_ADDR;           //错误类型为“分区地址非法”
        return;                                        //返回，停止执行
    }
    if (n_blks < (OS_MEM_QTY)2) {                      //如果分区的内存块数目少于2
       *p_err = OS_ERR_MEM_INVALID_BLKS;               //错误类型为“内存块数目非法”
        return;                                        //返回，停止执行
    }
    if (blk_size < sizeof(void *)) {                   //如果内存块空间小于指针的
       *p_err = OS_ERR_MEM_INVALID_SIZE;               //错误类型为“内存空间非法”
        return;                                        //返回，停止执行
    }
    align_msk = sizeof(void *) - 1u;                   //开始检查内存地址是否对齐
    if (align_msk > 0u) {
        if (((CPU_ADDR)p_addr & align_msk) != 0u){     //如果分区首地址没对齐
           *p_err = OS_ERR_MEM_INVALID_P_ADDR;         //错误类型为“分区地址非法”
            return;                                    //返回，停止执行
        }
        if ((blk_size & align_msk) != 0u) {            //如果内存块地址没对齐     
           *p_err = OS_ERR_MEM_INVALID_SIZE;           //错误类型为“内存块大小非法”
            return;                                    //返回，停止执行
        }
    }
#endif
    /* 将空闲内存块串联成一个单向链表 */
    p_link = (void **)p_addr;                          //内存分区首地址转为二级指针
    p_blk  = (CPU_INT08U *)p_addr;                     //首个内存块地址
    loops  = n_blks - 1u;
    for (i = 0u; i < loops; i++) {                     //将内存块逐个串成单向链表
        p_blk +=  blk_size;                            //下一内存块地址
       *p_link = (void  *)p_blk;                       //在当前内存块保存下一个内存块地址
        p_link = (void **)(void *)p_blk;               //下一个内存块的地址转为二级指针 
    }
   *p_link             = (void *)0;                    //最后一个内存块指向空

    OS_CRITICAL_ENTER();                               //进入临界段
    p_mem->Type        = OS_OBJ_TYPE_MEM;              //设置对象的类型   
    p_mem->NamePtr     = p_name;                       //保存内存分区的命名     
    p_mem->AddrPtr     = p_addr;                       //存储内存分区的首地址     
    p_mem->FreeListPtr = p_addr;                       //初始化空闲内存块池的首地址 
    p_mem->NbrFree     = n_blks;                       //存储空闲内存块的数目   
    p_mem->NbrMax      = n_blks;                       //存储内存块的总数目
    p_mem->BlkSize     = blk_size;                     //存储内存块的空间大小  

#if OS_CFG_DBG_EN > 0u            //如果使能了调试代码和变量 
    OS_MemDbgListAdd(p_mem);      //将内存管理对象插入内存管理双向调试列表
#endif

    OSMemQty++;                   //内存管理对象数目加1

    OS_CRITICAL_EXIT_NO_SCHED();  //退出临界段（无调度）
   *p_err = OS_ERR_NONE;          //错误类型为“无错误”
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                  GET A MEMORY BLOCK
*
* Description : Get a memory block from a partition
*
* Arguments   : p_mem   is a pointer to the memory partition control block
*
*               p_err   is a pointer to a variable containing an error message which will be set by this function to
*                       either:
*
*                       OS_ERR_NONE               if the memory partition has been created correctly.
*                       OS_ERR_MEM_INVALID_P_MEM  if you passed a NULL pointer for 'p_mem'
*                       OS_ERR_MEM_NO_FREE_BLKS   if there are no more free memory blocks to allocate to the caller
*
* Returns     : A pointer to a memory block if no error is detected
*               A pointer to NULL if an error is detected
************************************************************************************************************************
*/

void  *OSMemGet (OS_MEM  *p_mem, //内存管理对象
                 OS_ERR  *p_err) //返回错误类型
{
    void    *p_blk;
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {          //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();  //执行安全检测异常函数
        return ((void *)0);              //返回0（有错误），停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                 //如果使能了参数检测
    if (p_mem == (OS_MEM *)0) {            //如果 p_mem 为空            
       *p_err  = OS_ERR_MEM_INVALID_P_MEM; //错误类型为“内存分区非法”
        return ((void *)0);                //返回0（有错误），停止执行
    }
#endif

    CPU_CRITICAL_ENTER();                    //关中断
    if (p_mem->NbrFree == (OS_MEM_QTY)0) {   //如果没有空闲的内存块
        CPU_CRITICAL_EXIT();                 //开中断
       *p_err = OS_ERR_MEM_NO_FREE_BLKS;     //错误类型为“没有空闲内存块”  
        return ((void *)0);                  //返回0（有错误），停止执行
    }
    p_blk              = p_mem->FreeListPtr; //如果还有空闲内存块，就获取它
    p_mem->FreeListPtr = *(void **)p_blk;    //调整空闲内存块指针
    p_mem->NbrFree--;                        //空闲内存块数目减1
    CPU_CRITICAL_EXIT();                     //开中断
   *p_err = OS_ERR_NONE;                     //错误类型为“无错误”
    return (p_blk);                          //返回获取到的内存块
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                                 RELEASE A MEMORY BLOCK
*
* Description : Returns a memory block to a partition
*
* Arguments   : p_mem    is a pointer to the memory partition control block
*
*               p_blk    is a pointer to the memory block being released.
*
*               p_err    is a pointer to a variable that will contain an error code returned by this function.
*
*                            OS_ERR_NONE               if the memory block was inserted into the partition
*                            OS_ERR_MEM_FULL           if you are returning a memory block to an already FULL memory
*                                                      partition (You freed more blocks than you allocated!)
*                            OS_ERR_MEM_INVALID_P_BLK  if you passed a NULL pointer for the block to release.
*                            OS_ERR_MEM_INVALID_P_MEM  if you passed a NULL pointer for 'p_mem'
************************************************************************************************************************
*/

void  OSMemPut (OS_MEM  *p_mem,   //内存管理对象
                void    *p_blk,   //要退回的内存块
                OS_ERR  *p_err)   //返回错误类型
{
    CPU_SR_ALLOC(); //使用到临界段（在关/开中断时）时必需该宏，该宏声明和
                    //定义一个局部变量，用于保存关中断前的 CPU 状态寄存器
                    // SR（临界段关中断只需保存SR），开中断时将该值还原。

#ifdef OS_SAFETY_CRITICAL                //如果使能了安全检测
    if (p_err == (OS_ERR *)0) {          //如果错误类型实参为空
        OS_SAFETY_CRITICAL_EXCEPTION();  //执行安全检测异常函数
        return;                          //返回，停止执行
    }
#endif

#if OS_CFG_ARG_CHK_EN > 0u                  //如果使能了参数检测
    if (p_mem == (OS_MEM *)0) {             //如果 p_mem 为空                
       *p_err  = OS_ERR_MEM_INVALID_P_MEM;  //错误类型为“内存分区非法”
        return;                             //返回，停止执行
    }
    if (p_blk == (void *)0) {               //如果内存块为空
       *p_err  = OS_ERR_MEM_INVALID_P_BLK;  //错误类型为"内存块非法"
        return;                             //返回，停止执行
    }
#endif

    CPU_CRITICAL_ENTER();                    //关中断
    if (p_mem->NbrFree >= p_mem->NbrMax) {   //如果所有的内存块已经退出分区                
        CPU_CRITICAL_EXIT();                 //开中断
       *p_err = OS_ERR_MEM_FULL;             //错误类型为“内存分区已满”
        return;                              //返回，停止执行
    }
    *(void **)p_blk    = p_mem->FreeListPtr; //把内存块插入空闲内存块链表
    p_mem->FreeListPtr = p_blk;              //内存块退回到链表的最前端
    p_mem->NbrFree++;                        //空闲内存块数目加1
    CPU_CRITICAL_EXIT();                     //开中断
   *p_err              = OS_ERR_NONE;        //错误类型为“无错误”
}

/*$PAGE*/
/*
************************************************************************************************************************
*                                           ADD MEMORY PARTITION TO DEBUG LIST
*
* Description : This function is called by OSMemCreate() to add the memory partition to the debug table.
*
* Arguments   : p_mem    Is a pointer to the memory partition
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

#if OS_CFG_DBG_EN > 0u                                  //如果使能了调试代码和变量 
void  OS_MemDbgListAdd (OS_MEM  *p_mem)                 //将内存管理对象插入到内存管理调试列表的最前端
{
    p_mem->DbgPrevPtr               = (OS_MEM *)0;      //将该对象作为列表的最前端
    if (OSMemDbgListPtr == (OS_MEM *)0) {               //如果列表为空
        p_mem->DbgNextPtr           = (OS_MEM *)0;      //该队列的下一个元素也为空
    } else {                                            //如果列表非空
        p_mem->DbgNextPtr           =  OSMemDbgListPtr; //列表原来的首元素作为该队列的下一个元素
        OSMemDbgListPtr->DbgPrevPtr =  p_mem;           //原来的首元素的前面变为了该队列
    }
    OSMemDbgListPtr                 =  p_mem;           //该对象成为列表的新首元素
}
#endif

/*$PAGE*/
/*
************************************************************************************************************************
*                                           INITIALIZE MEMORY PARTITION MANAGER
*
* Description : This function is called by uC/OS-III to initialize the memory partition manager.  Your
*               application MUST NOT call this function.
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_MemInit (OS_ERR  *p_err)
{
#ifdef OS_SAFETY_CRITICAL
    if (p_err == (OS_ERR *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_CFG_DBG_EN > 0u
    OSMemDbgListPtr = (OS_MEM   *)0;
#endif

    OSMemQty        = (OS_OBJ_QTY)0;
   *p_err           = OS_ERR_NONE;
}
#endif
