# 第六十六章 读写内部FLASH

## 1. 硬件设计

本实验仅操作了STM32芯片内部的FLASH空间，无需额外的硬件。

## 2. 软件设计

### 2.1 编程大纲

1. 对内部FLASH解锁

2. 找出空闲页，擦除目标页

3. 进行读写测试

### 2.2 代码分析

#### 2.2.1 硬件定义

读写内部FLASH不需要用到任何外部硬件，不过在编写测试时我们要先确定内部FLASH的页大小以及要往哪些地址写入数据

```c
/* STM32大容量产品每页大小2KByte，中、小容量产品每页大小1KByte */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)	//2048
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)	//1024
#endif

// 写入的起始地址与结束地址
#define WRITE_START_ADDR  ((uint32_t)0x08008000)
```

代码中首先根据芯片类型定义了宏FLASH_PAGE_SIZE，由于本工程使用的是STM32VET6芯片，在工程的C/C++选项中包含了STM32F10X_HD的定义， 所以FLASH_PAGE_SIZE被定义成0x800，即2048字节。

另外，WRITE_START_ADDR和WRITE_END_ADDR定义了后面本工程测试读写内部FLASH的起始地址与结束地址，这部分区域与map文件指示的程序本身占用的空间不重合， 所以在后面修改这些地址的内容时，它不会把自身的程序修改掉。

#### 2.2.2 读写内部FLASH

一切准备就绪，可以开始对内部FLASH进行擦写，这个过程不需要初始化任何外设，只要按解锁、擦除及写入的流程走就可以了

```c
// 对内部FLASH进行读写测试
int InternalFlash_Test(void)
{
	uint32_t EraseCounter = 0x00; // 记录要擦除多少页
	uint32_t Address = 0x00;			// 记录写入的地址
	uint32_t Data = 0x3210ABCD;		// 记录写入的数据
	uint32_t NbrOfPage = 0x00;		// 记录写入多少页
	FLASH_Status FLASHStatus = FLASH_COMPLETE; // 记录每次擦除的结果	
	TestStatus MemoryProgramStatus = PASSED;   // 记录整个测试结果

  FLASH_Unlock(); // 解锁FLASH
  /* 计算要擦除多少页 */
  NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;
  /* 清空所有标志位 */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
  /* 按页擦除*/
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
	}
  /* 向内部FLASH写入数据 */
  Address = WRITE_START_ADDR;
  while((Address < WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramWord(Address, Data);
    Address = Address + 4;
  }
  FLASH_Lock(); // 锁定FLASH
  /* 检查写入的数据是否正确 */
  Address = WRITE_START_ADDR;
  while((Address < WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
  {
    if((*(__IO uint32_t*) Address) != Data)
    {
      MemoryProgramStatus = FAILED;
    }
    Address += 4;
  }
	return MemoryProgramStatus;
}
```

该函数的执行过程如下：

1. 调用FLASH_Unlock解锁；

2. 根据起始地址及结束地址计算要擦除多少页；

3. 调用FLASH_ClearFlag清除各种标志位；

4. 使用循环调用FLASH_ErasePage擦除页，每次擦除一页；

5. 使用循环调用FLASH_ProgramWord函数向起始地址至结束地址的存储区域都写入变量 “Data” 存储的数值数值；

6. 调用FLASH_Lock上锁；

7. 使用指针读取写入的数据内容并校验。

#### 2.2.3 主函数测试

```c
int main(void)
{ 	
  USART_Config();
	LED_Init();
	LED_BLUE();
	if(InternalFlash_Test()== PASSED)
	{
		LED_GREEN();
		printf("读写内部FLASH测试成功\r\n");
	}
	else
	{
		printf("读写内部FLASH测试失败\r\n");
		LED_RED();
	}	
  while(1); 
}
```

main函数中初始化了用于指示调试信息的LED及串口后，直接调用了InternalFlash_Test函数， 进行读写测试并根据测试结果输出调试信息。


