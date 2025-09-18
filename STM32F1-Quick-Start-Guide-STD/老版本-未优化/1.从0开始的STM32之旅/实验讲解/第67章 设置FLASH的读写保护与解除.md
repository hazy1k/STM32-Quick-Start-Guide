# 第六十七章 设置FLASH的读写保护与解除

## 1. 前提须知

本实验要进行的操作比较特殊，由于设置成读写保护状态后，若不解除保护状态或者解除代码工作不正常，将无法给芯片的FLASH下载新的程序， 所以本程序在开发过程中使用内部SRAM调试的方式开发，便于测试程序（读写保护只影响FLASH，SRAM调试时程序下载到SRAM中，不受影响）。 工程中，提供了FLASH和SRAM调试的版本，见图

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHp002.png)

工程的FLASH版本程序包含完整的保护及解除方案，程序下载到内部FLASH后，它自身可以正常地进行保护及解除。另外， 在学习过程中如果您想亲自修改该代码进行测试，也不用担心把解除操作的代码修改至工作不正常而导致芯片无法解锁报废， 处于这种情况时，只要使用本工程的SRAM版本下载到芯片中，即可实现解锁。只要具备前面章节介绍的SRAM调试知识并备份了SRAM版本的工程即可大胆尝试。

## 2. 硬件设计

本实验完全针对内部FLASH的操作，对外部硬件无特殊要求。即使是在SRAM调试模式下，由于是使用Debug强制加载PC和SP指针，所以也无需设置BOOT0和BOOT1的引脚。

## 3. 软件设计

### 3.1 编程大纲

1. 设置写保护及解除

2. 设置读保护及解除

3. 主函数测试

### 3.2 代码分析

#### 3.2.1 设置写保护及解除

```c
// 反转写保护的配置,若芯片处于写保护状态，则解除，若不是写保护状态，则设置成写保护
void WriteProtect_Toggle(void)
{
	/* 获取写保护寄存器的值进行判断，寄存器位为0表示有保护，为1表示无保护 */
	/*  若不等于0xFFFFFFFF，则说明有部分页被写保护了 */
	if(FLASH_GetWriteProtectionOptionByte() != 0xFFFFFFFF )
	{
		FLASH_DEBUG("芯片处于写保护状态，即将执行解保护过程...");
		
		//解除对FLASH_CR寄存器的访问限制
		FLASH_Unlock();
		/* 擦除所有选项字节的内容 */
		FLASH_EraseOptionBytes();
		/* 对所有页解除 */
		FLASH_EnableWriteProtection(0x00000000);	
		FLASH_DEBUG("配置完成，芯片将自动复位加载新配置，复位后芯片会解除写保护状态\r\n");
		/* 复位芯片，以使选项字节生效 */
		NVIC_SystemReset();
	}
	else //无写保护
	{
		FLASH_DEBUG("芯片处于无写保护状态，即将执行写保护过程...");
		// 解除对FLASH_CR寄存器的访问限制
		FLASH_Unlock();
		/* 先擦除所有选项字节的内容，防止因为原有的写保护导致无法写入新的保护配置 */
		FLASH_EraseOptionBytes();
		/* 对所有页进行写保护 */
		FLASH_EnableWriteProtection(FLASH_WRProt_AllPages);
		FLASH_DEBUG("配置完成，芯片将自动复位加载新配置，复位后芯片会处于写保护状态\r\n");
		/* 复位芯片，以使选项字节生效 */
		NVIC_SystemReset();		
	}
}
```

本函数主要演示写保护和解除功能，若芯片本身处于写保护状态，则解除保护，若芯片本身处于无写保护状态，则设置加入写保护。

WriteProtect_Toggle在操作前会先使用库函数FLASH_GetWriteProtectionOptionByte检测芯片当前的写保护状态，该函数的返回值为FLASH_WRPR寄存器的内容， 它反映了选项字节WRP0/1/2/3的配置。所以在代码中，它判断该函数的返回值不等于0xFFFFFFFF时，可知道芯片至少存在一页被写保护，则程序开始执行解除保护分支。

在解除保护分支中，先调用FLASH_Unlock解除FLASH_CR的访问限制，再使用参数0调用前面介绍的FLASH_EnableWriteProtection函数对所有页解除写保护， 解除配置写入完成后，调用库函数NVIC_SystemReset使芯片产生系统复位，从而使配置生效。

若WriteProtect_Toggle在执行判断时发现芯片本身处于无写保护的状态，则以上述同样的过程向选项字节写入配置， 调用FLASH_EnableWriteProtection函数时使用FLASH_WRProt_AllPages宏，对所有FLASH页加入写保护，最后同样调用NVIC_SystemReset产生系统复位使配置生效。

#### 3.2.2 设置读保护及解除

```c
// 反转读保护的配置,若芯片处于读保护状态，则解除，若不是读保护状态，则设置成读保护
void ReadProtect_Toggle(void)
{
	if(FLASH_GetReadOutProtectionStatus () == SET )
	{
		FLASH_DEBUG("芯片处于读保护状态\r\n");
		// 解除对FLASH_CR寄存器的访问限制
		FLASH_Unlock();
		FLASH_DEBUG("即将解除读保护，解除读保护会把FLASH的所有内容清空");
		FLASH_DEBUG("由于解除后程序被清空，所以后面不会有任何提示输出");
		FLASH_DEBUG("等待20秒后即可给芯片下载新的程序...\r\n");
		FLASH_ReadOutProtection (DISABLE);		
		//即使在此处加入printf串口调试也不会执行的，因为存储程序的整片FLASH都已被擦除。
		FLASH_DEBUG("由于FLASH程序被清空，所以本代码不会被执行，串口不会有本语句输出（SRAM调试模式下例外）\r\n");
	}
	else
	{
		FLASH_DEBUG("芯片处于无读保护状态，即将执行读保护过程...\r\n");
		// 解除对FLASH_CR寄存器的访问限制
		FLASH_Unlock();				
		FLASH_ReadOutProtection (ENABLE);
		printf("芯片已被设置为读保护，上电复位后生效（必须重新给开发板上电，只按复位键无效）\r\n");
		printf("处于保护状态下无法正常下载新程序，必须要先解除保护状态再下载\r\n");
	}
}
```

类似地，本函数主要演示读保护和解除功能，若芯片本身处于读保护状态，则解除保护，若芯片本身处于无读保护状态，则设置加入读保护。

ReadProtect_Toggle在操作前会先使用库函数FLASH_GetReadOutProtectionStatus检测芯片当前的写保护状态，该函数内部通过判断选项字节的RDP值， 返回SET（读保护状态）和RESET（无读保护状态）。

判断后，若进入到解除保护分支，会先调用FLASH_Unlock解除FLASH_CR的访问限制，然后使用前面介绍的FLASH_ReadOutProtection函数以DISABLE作为参数解除读保护。

必须注意的是，该函数执行后，所有存储在内部FLASH时的代码都会被删除，以防止原程序被读出，而由于自身代码已被清除， 所以代码中在FLASH_ReadOutProtection(DISABLE)语句后的串口输出是不会被执行的，因为此时这个程序已经不存在了， 但如果使用SRAM版本的程序测试，它是会有输出的，因为这时本程序自身是存储在内部SRAM空间的。

由于解除保护后会触发芯片FLASH的整片擦除操作，所以要稍等一段时间，等待20秒后，解除操作完成，可以重新给芯片的FLASH下载新的程序。

若ReadProtect_Toggle在执行判断时发现芯片本身处于无读保护的状态，它会使用FLASH_ReadOutProtection(ENABLE)语句把芯片设置为读保护状态。 仔细对比读写保护的配置函数，可以发现读保护设置后并没有调用NVIC_SystemReset函数使芯片产生系统复位，这是因为读保护的设置与解除， 是要使用上电复位才能生效的（即重新给芯片上电），系统复位不会产生效应。

#### 3.2.3 主函数测试

```c
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./key/bsp_key.h"  
#include "./led/bsp_led.h"
#include "./protect/bsp_readWriteProtect.h"   

void Delay(__IO uint32_t nCount);

//【 ！！】注意事项：
//1.当芯片处于读写保护状态时，均无法下载新的程序，需要先解除保护状态后再下载
//2.本工程包含两个版本，可在MDK的“Load”下载按钮旁边的下拉框选择：
//	FLASH版本：	接上串口调试助手后，直接点击MDK的“Load”按钮把程序下载到STM32的FLASH中，
//				复位运行，串口会输出当前芯片的保护状态，可使用KEY1和KEY2切换。切换写保护
//				状态时，芯片会自动复位，程序重新执行；切换读保护状态时，按键后需要重新给
//				开发板上电复位，配置才会有效（断电时，串口与电脑的连接会断开，所以上电后
//				注意重新打开串口调试助手），若是执行解除读保护过程，运行后芯片FLASH中自身
//				的代码都会消失，所以要重新给开发板下载程序。
//	RAM版本  ：	若无SRAM调试程序的经验，请先学习前面的《SRAM调试》章节。接上串口调试助手后，
//				只能使用MDK的“Debug”按钮把程序下载到STM32的内部SRAM中，然后点击全速运行，
//				可在串口查看调试输出。由于SRAM调试状态下，复位会使芯片程序乱飞，所以每次切
//				换状态后，都要重新点击“Debug”按钮下载SRAM程序，再全速运行查看输出。
//3.若自己修改程序导致使芯片处于读写保护状态而无法下载，
//  且 FALSH程序自身又不包含自解除状态的程序，可以使用本工程的“RAM版本”解除，解除即可重新下载。

int main(void)
{ 	
	  /*初始化USART，配置模式为 115200 8-N-1*/
  USART_Config();
	LED_GPIO_Config();
	Key_GPIO_Config();
	LED_BLUE;
	/* 获取写保护寄存器的值进行判断，寄存器位为0表示有保护，为1表示无保护 */
	/*  若不等于0xFFFFFFFF，则说明有部分页被写保护了 */
	if(FLASH_GetWriteProtectionOptionByte() !=0xFFFFFFFF )
	{
		printf("\r\n目前芯片处于写保护状态，按Key1键解除保护\r\n");
		printf("写保护寄存器的值：WRPR=0x%x\r\n",FLASH_GetWriteProtectionOptionByte());
	}
	else // 无写保护
	{
		printf("\r\n目前芯片无 写 保护，按 Key1 键可设置成 写 保护\r\n");
		printf("写保护寄存器的值：WRPR=0x%x\r\n",FLASH_GetWriteProtectionOptionByte());
	}
	/*  若等于SET，说明处于读保护状态 */
	if(FLASH_GetReadOutProtectionStatus () == SET )
	{
		printf("\r\n目前芯片处于读保护状态，按Key2键解除保护\r\n");
	}
	else
	{
		printf("\r\n目前芯片无 读 保护，按 Key2 键可设置成 读 保护\r\n");
	}
	while(1)                            
	{	   
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{
			LED1_TOGGLE;
			WriteProtect_Toggle();
		} 
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			LED2_TOGGLE;
			ReadProtect_Toggle();			
		}		
	}	
}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

```

在main函数中，初始化了串口、LED、按键等外设后，根据芯片当前的保护状态输出调试信息，接着循环轮询按键， 若按了KEY1按键，则执行前面的WriteProtect_Toggle反转写保护状态，若按了KEY2键，则执行前面的ReadProtect_Toggle反转读保护状态。


