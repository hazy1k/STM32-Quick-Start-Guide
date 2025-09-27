/**
  ******************************************************************************
  *                              头文件
  ******************************************************************************
  */
#include "board.h" 
/*
*************************************************************************
*                               变量
*************************************************************************
*/
FATFS fs;													/* FatFs文件系统对象 */
FIL fnew;													/* 文件对象 */
FRESULT res_flash;                /* 文件操作结果 */
UINT fnum;            					  /* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};        /* 读缓冲区 */
BYTE WriteBuffer[] =              /* 写缓冲区*/
"\n欢迎使用野火STM32开发板，今天是个好日子\n"	
"适合学习野火开发版与RT-Thread的例程\n"	
"——SPI-FatFs文件系统\n"	;
//"RT-Thread是世界第一牛逼的操作系统\n"	
//"那是真的牛逼，野火也是世界第一，欢迎大家学习\n"	
//"我们的官网是：http://www.firebbs.cn\n "	
//"RT-Thread的官网是：https://www.rt-thread.org/\n"	
//"本次新建文件系统测试文件完毕\n"; 


/**
  ******************************************************************
  * @brief   文件系统初始化
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
void FileSystem_Init(void)
{
	/* 打开中断，如果不加互斥量可以在board.c中进行初始化
		 则必须打开中断，在任务中初始化的时候则不需要打开中断 */
	rt_base_t level = 0;
//	rt_hw_interrupt_enable(level);	
	level = rt_hw_interrupt_disable();

	//在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	//初始化函数调用流程如下
	//f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
	res_flash = f_mount(&fs,"1:",1);
	
/*----------------------- 格式化测试 -----------------*/  
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if(res_flash == FR_NO_FILESYSTEM)
	{
		rt_kprintf("》FLASH还没有文件系统，即将进行格式化...\r\n");
    /* 格式化 */
		res_flash=f_mkfs("1:",0,0);							
		
		if(res_flash == FR_OK)
		{
			rt_kprintf("》FLASH已成功格式化文件系统。\r\n");
      /* 格式化后，先取消挂载 */
			res_flash = f_mount(NULL,"1:",1);			
      /* 重新挂载	*/			
			res_flash = f_mount(&fs,"1:",1);
		}
		else
		{
			LED_RED;
			rt_kprintf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  else if(res_flash!=FR_OK)
  {
    rt_kprintf("！！外部Flash挂载文件系统失败。(%d)\r\n",res_flash);
    rt_kprintf("！！可能原因：SPI Flash初始化不成功。\r\n");
		while(1);
  }
  else
  {
    rt_kprintf("》文件系统挂载成功，可以进行读写测试\r\n");
  }
	rt_hw_interrupt_enable(level);
	/* 关闭中断，如果不加互斥量可以在board.c中进行初始化
		 则必须关闭中断，在任务中初始化的时候则不需要关闭中断 */
//		rt_hw_interrupt_disable();
}

/**
  ******************************************************************
  * @brief   文件系统读写测试
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
void FileSystem_Test(void)
{
/*----------------------- 文件系统测试：写测试 -------------------*/
	/* 打开文件，每次都以新建的形式打开，属性为可写 */
	rt_kprintf("\r\n****** 即将进行文件写入测试... ******\r\n");	
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_flash == FR_OK )
	{
		rt_kprintf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
    /* 将指定存储区内容写入到文件内 */
		res_flash=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_flash==FR_OK)
    {
      rt_kprintf("》文件写入成功，写入字节数据：%d\n",fnum);
//      rt_kprintf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      rt_kprintf("！！文件写入失败：(%d)\n",res_flash);
    }    
//		/* 不再读写，关闭文件 */
    f_close(&fnew);
	}
	else
	{	
		LED_RED;
		rt_kprintf("！！打开/创建文件失败。\r\n");
		rt_kprintf("er = %d \n",res_flash);
	}
	
/*------------------- 文件系统测试：读测试 --------------------------*/
	rt_kprintf("****** 即将进行文件读取测试... ******\r\n");
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",FA_OPEN_EXISTING | FA_READ); 	 
	if(res_flash == FR_OK)
	{
		LED_GREEN;
		rt_kprintf("》打开文件成功。\r\n");
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_flash==FR_OK)
    {
      rt_kprintf("》文件读取成功,读到字节数据：%d\r\n",fnum);
//      rt_kprintf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      rt_kprintf("！！文件读取失败：(%d)\n",res_flash);
    }		
	}
	else
	{
		LED_RED;
		rt_kprintf("！！打开文件失败。\r\n");
	}
	/* 不再读写，关闭文件 */
	f_close(&fnew);	
  
	/* 不再使用文件系统，取消挂载文件系统 */
//	f_mount(NULL,"1:",1);
  
  /* 操作完成，停机 */
	
}


