#include "stm32f10x.h"
#include "ff.h" // FatFs文件系统头文件
#include "flash.h"
#include "usart.h"
#include "led.h"

FATFS fs;				   // FatFs文件系统对象
FIL fnew;				   // 文件对象
FRESULT res_flash;         // 文件操作结果
UINT fnum;            	   // 文件成功读写数量 
BYTE ReadBuffer[1024] = {0}; // 读缓冲区
BYTE WriteBuffer[] =  "Demo File System Test\r\n"; // 写缓冲区

int main(void)
{
	LED_Init();	
	LED_BLUE();
	USART_Config();	
  	printf("外设初始化正常");
	/*
		在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
		初始化函数调用流程如下
		f_mount()->find_volume()->disk_initialize->SPI_FLASH_Init()
	*/
	res_flash = f_mount(&fs, "1:", 1); // 挂载文件系统，函数参数：文件系统对象、挂载点、格式化标志	
	// 如果没有文件系统就格式化创建创建文件系统
	if(res_flash == FR_NO_FILESYSTEM)
	{
		printf("FLASH还没有文件系统，即将进行格式化...\r\n");
    	// 下面进行格式化操作
		res_flash = f_mkfs("1:", 0, 0); // 函数参数：挂载点、分区大小、工作区大小							
		if(res_flash == FR_OK) // 格式化成功
		{
			printf("FLASH已成功格式化文件系统。\r\n");
      		// 格式化后，先取消挂载
			res_flash = f_mount(NULL,"1:",1); // 函数参数：文件系统对象、挂载点、格式化标志		
      		// 重新挂载			
			res_flash = f_mount(&fs,"1:",1); // 函数参数：文件系统对象、挂载点、格式化标志
		}
		else
		{
			LED_RED();
			printf("《《格式化失败。》》\r\n");
			while(1);
		}
	}
  	else if(res_flash != FR_OK) // 其他错误
    {
    	printf("！！外部Flash挂载文件系统失败。(%d)\r\n",res_flash);
		while(1);
  	}
  	else
  	{
    	printf("文件系统挂载成功\r\n");
  	}
  
	// 文件系统测试：写测试
	// 打开文件，每次都以新建的形式打开，属性为可写 */
	printf("\r\n即将进行文件写入测试... \r\n");	
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt", FA_CREATE_ALWAYS | FA_WRITE ); // f_open()函数打开文件，参数：文件对象、文件名、属性
	if (res_flash == FR_OK)
	{
		printf("打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
    	// 将指定存储区内容写入到文件内
		res_flash = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum); // f_write()函数写入文件，参数：文件对象、写入数据、写入字节数、实际写入字节数
    	if(res_flash == FR_OK)
    	{
      		printf("文件写入成功，写入字节数据：%d\n",fnum);
      		printf("向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
    	}
    	else
    	{
      		printf("文件写入失败：(%d)\n",res_flash);
    	}    
    	f_close(&fnew); // f_close()函数关闭文件，参数：文件对象
	}
	else
	{	
		LED_RED();
		printf("打开/创建文件失败。\r\n");
	}
	// 文件系统测试：读测试
	printf("即将进行文件读取测试...\r\n");
	res_flash = f_open(&fnew, "1:FatFs读写测试文件.txt",FA_OPEN_EXISTING | FA_READ); // f_open()函数打开文件，参数：文件对象、文件名、属性
	if(res_flash == FR_OK)
	{
		LED_GREEN();
		printf("打开文件成功。\r\n");
		res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); // f_read()函数读取文件，参数：文件对象、读入数据、读入字节数、实际读入字节数
    	if(res_flash == FR_OK)
    	{
      		printf("文件读取成功,读到字节数据：%d\r\n",fnum);
      		printf("读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
    	}
    	else
    	{
      		printf("文件读取失败：(%d)\n",res_flash);
    	}			
	}
	else
	{
		LED_RED();
		printf("打开文件失败。\r\n");
	}
	// 测试完毕，关闭文件
	f_close(&fnew);	
	// 不再使用文件系统，取消挂载文件系统
	f_mount(NULL,"1:",1);
	while(1)
	{
	}
}
