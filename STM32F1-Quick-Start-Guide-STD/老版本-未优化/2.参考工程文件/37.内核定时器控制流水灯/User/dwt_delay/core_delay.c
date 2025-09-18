#include "core_delay.h"   

/*
 在Cortex-M里面有一个外设叫DWT(Data Watchpoint and Trace)，
 该外设有一个32位的寄存器叫CYCCNT，它是一个向上的计数器，
 记录的是内核时钟运行的个数，最长能记录的时间为：
 60s=2的32次方/72000000
 (假设内核频率为72M，内核跳一次的时间大概为1/72M=13.8ns)
 当CYCCNT溢出之后，会清0重新开始向上计数。
 使能CYCCNT计数的操作步骤：
 1、先使能DWT外设，这个由另外内核调试寄存器DEMCR的位24控制，写1使能
 2、使能CYCCNT寄存器之前，先清0
 3、使能CYCCNT寄存器，这个由DWT_CTRL(代码上宏定义为DWT_CR)的位0控制，写1使能
*/
 
#if USE_DWT_DELAY // 宏定义，如果没有定义，则不使用DWT延时函数

#define  DWT_CR      *(__IO uint32_t *)0xE0001000 // DWT控制寄存器地址
#define  DWT_CYCCNT  *(__IO uint32_t *)0xE0001004 // DWT计数器地址
#define  DEM_CR      *(__IO uint32_t *)0xE000EDFC // 调试监视器控制寄存器地址
#define  DEM_CR_TRCENA     (1 << 24) // 调试监视器控制寄存器的位24，DWT使能位
#define  DWT_CR_CYCCNTENA  (1 <<  0) // DWT控制寄存器的位0，CYCCNT使能位

// 初始化时间戳
void CPU_TS_TmrInit(void)
{
    DEM_CR |= (uint32_t)DEM_CR_TRCENA; // 使能DWT外设
    DWT_CYCCNT = (uint32_t)0u; // 清0计数器
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA; // 使能CYCCNT计数器
}

// 读取当前时间戳
uint32_t CPU_TS_TmrRd(void)
{        
  return ((uint32_t)DWT_CYCCNT);
}

// 采用CPU的内部计数实现精确延时，32位计数器
void CPU_TS_Tmr_Delay_US(__IO uint32_t us)
{
  uint32_t ticks;
  uint32_t told,tnow,tcnt=0;

  /* 在函数内部初始化时间戳寄存器， */  
#if (CPU_TS_INIT_IN_DELAY_FUNCTION)  
  /* 初始化时间戳并清零 */
  CPU_TS_TmrInit();
#endif
  
  ticks = us * (GET_CPU_ClkFreq() / 1000000);  /* 需要的节拍数 */      
  tcnt = 0;
  told = (uint32_t)CPU_TS_TmrRd();         /* 刚进入时的计数器值 */
  while(1)
  {
    tnow = (uint32_t)CPU_TS_TmrRd();  
    if(tnow != told)
    { 
        /* 32位计数器是递增计数器 */    
      if(tnow > told)
      {
        tcnt += tnow - told;  
      }
      /* 重新装载 */
      else 
      {
        tcnt += UINT32_MAX - told + tnow; 
      } 
      told = tnow;
      /*时间超过/等于要延迟的时间,则退出 */
      if(tcnt >= ticks)break;
    }  
  }
}

#endif
