
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"

/*
 * t : 定时时间 
 * Ticks : 多少个时钟周期产生一次中断 
 * f : 时钟频率 72000000
 * t = Ticks * 1/f = (72000000/100000) * (1/72000000) = 10us 
 */ 

/*
************************************************************************************************************************
*                                             时间戳相关寄存器定义
************************************************************************************************************************
*/
/*
 在Cortex-M3里面有一个外设叫DWT(Data Watchpoint and Trace)，该外设有一个32位的寄存器叫CYCCNT，它是一个向上的
 计数器，记录的是内核时钟运行的个数，最长能记录的时间为：60s=2的32次方/72000000(假设内核频率为72M，内核跳一次的时间大概为1/72M=14ns)
 当CYCCNT溢出之后，会清0重新开始向上计数。
 使能CYCCNT计数的操作步骤：
 1、先使能DWT外设，这个由另外内核调试寄存器DEMCR的位24控制，写1使能
 2、使能CYCCNT寄存器之前，先清0
 3、使能CYCCNT寄存器，这个由DWT_CTRL(代码上宏定义为DWT_CR)的位0控制，写1使能
 */


#define  DWT_CR      *(uint32_t *)0xE0001000
#define  DWT_CYCCNT  *(uint32_t *)0xE0001004
#define  DEM_CR      *(uint32_t *)0xE000EDFC


#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)


/* 初始化时间戳 */
void CPU_TS_TmrInit(void)
{
    /* 使能DWT外设 */
	DEM_CR |= (uint32_t)DEM_CR_TRCENA;                
    
	/* DWT CYCCNT寄存器计数清0 */
	DWT_CYCCNT = (uint32_t)0u;
	
	/* 使能Cortex-M3 DWT CYCCNT寄存器 */
    DWT_CR |= (uint32_t)DWT_CR_CYCCNTENA;
}

uint32_t OS_TS_GET(void)
{	
	return ((uint32_t)DWT_CYCCNT);
}

/* CPU相关初始化 */
void  CPU_Init (void)
{
	/* 初始化时间戳 */
	CPU_TS_TmrInit();
}

uint32_t TimeStart = 0,TimeEnd = 0 ,TimeUse = 0;

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	
	
	/* LED 端口初始化 */
	LED_GPIO_Config();
	
	CPU_TS_TmrInit();
	
	for(;;)
	{
		LED1( ON );
		
		TimeStart = OS_TS_GET();		
		SysTick_Delay_Ms( 1000 );
		TimeEnd = OS_TS_GET();
		TimeUse = (TimeEnd - TimeStart) / ( 72000000 / 1000 ); /* 单位为ms */
		
		LED1( OFF );
	  
		LED2( ON );
		SysTick_Delay_Ms( 1000 );
		LED2( OFF );
	} 	
}


/*********************************************END OF FILE**********************/
