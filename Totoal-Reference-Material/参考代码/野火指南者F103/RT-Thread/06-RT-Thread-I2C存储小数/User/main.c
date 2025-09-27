/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   RT-Thread 3.0 + STM32 线程管理
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 

/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"


/*
*************************************************************************
*                               变量
*************************************************************************
*/
/* 定义线程控制块 */
static rt_thread_t test_thread = RT_NULL;
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void test_thread_entry(void* parameter);
static void iic_test(void);
/*
*************************************************************************
*                             main 函数
*************************************************************************
*/
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
    /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
  rt_kprintf("这是一个 RTT-EEPROM 读写小数和长整数实验！\n\n");
	test_thread =                          /* 线程控制块指针 */
    rt_thread_create( "test",              /* 线程名字 */
                      test_thread_entry,   /* 线程入口函数 */
                      RT_NULL,             /* 线程入口函数参数 */
                      2048,                 /* 线程栈大小 */
                      3,                   /* 线程的优先级 */
                      20);                 /* 线程时间片 */
                   
    /* 启动线程，开启调度 */
   if (test_thread != RT_NULL)
        rt_thread_startup(test_thread);
    else
        return -1;

}

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void test_thread_entry(void* parameter)
{	
	
    while (1)
    {
			iic_test();
			rt_thread_delay(1000);	
    }
}


static void iic_test(void)
{
uint8_t cal_flag = 0;
uint8_t k;

/*存储小数和整数的数组，各7个*/
long double double_buffer[7] = {0};
int int_bufffer[7] = {0};

#define DOUBLE_ADDR       10
#define LONGINT_ADDR      70
	
		/*读取数据标志位*/
I2C_EE_BufferRead(&cal_flag, 0, 1);

if( cal_flag != 0xCD )	/*若标志等于0xcd，表示之前已有写入数据*/
{      
		printf("\r\n没有检测到数据标志，FLASH没有存储数据，即将进行小数写入实验\r\n");
		cal_flag =0xCD;
		
		/*写入标志到0地址*/
		I2C_EE_BufferWrite(&cal_flag, 0, 1); 
		
		/*生成要写入的数据*/
		for( k=0; k<7; k++ )
		{
			 double_buffer[k] = k +0.1;
			 int_bufffer[k]=k*500+1 ;
		}

		/*写入小数数据到地址10*/
		I2C_EE_BufferWrite((void*)double_buffer,DOUBLE_ADDR, sizeof(double_buffer));
		/*写入整数数据到地址60*/
		I2C_EE_BufferWrite((void*)int_bufffer, LONGINT_ADDR, sizeof(int_bufffer));
					
		printf("向芯片写入数据：");
		/*打印到串口*/
		for( k=0; k<7; k++ )
		{
			printf("小数tx = %LF\r\n",double_buffer[k]);
			printf("整数tx = %d\r\n",int_bufffer[k]);
		}
		
		printf("\r\n请复位开发板，以读取数据进行检验\r\n");      

}    
else
{      
		 printf("\r\n检测到数据标志\r\n");

		/*读取小数数据*/
		I2C_EE_BufferRead((void*)double_buffer, DOUBLE_ADDR, sizeof(double_buffer));
		
		/*读取整数数据*/
		I2C_EE_BufferRead((void*)int_bufffer, LONGINT_ADDR, sizeof(int_bufffer));

	
		printf("\r\n从芯片读到数据：\r\n");			
		for( k=0; k<7; k++ )
		{
			printf("小数 rx = %LF \r\n",double_buffer[k]);
			printf("整数 rx = %d \r\n",int_bufffer[k]);				
		}
	
} 

}



/********************************END OF FILE****************************/
