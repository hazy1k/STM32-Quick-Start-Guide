# 第六十章 RTC-闹钟

## 1. 硬件设计

参考RTC-万年历实验

## 2. 软件设计

### 2.1 编程大纲

1. 时间结构体定义

2. 闹钟工作机制配置

3. 主函数测试

### 2.2 代码分析

其他地方与RTC-万年历类似，此处只分析main函数不同之处

#### 2.2.1 时间结构体配置

```c
/*时间结构体，默认时间2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};

/*时间结构体，闹钟时间2000-01-01 00:00:08*/
struct rtc_time clocktime=
{
8,0,0,1,1,2000,0
};
```

#### 2.2.2 闹钟工作机制配置

```c
int main()
{		
	
	uint32_t clock_timestamp;
	uint32_t current_timestamp;
	
//可使用该宏设置是否使用液晶显示
#ifdef  USE_LCD_DISPLAY
	
		ILI9341_Init ();         //LCD 初始化
		LCD_SetFont(&Font8x16);
		LCD_SetColors(RED,BLACK);

		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */

		ILI9341_DispStringLine_EN(LINE(0),"        BH RTC demo");
#endif
	
	  USART_Config();			
	
		Key_GPIO_Config();
	
		BEEP_GPIO_Config();
		
		/* 配置RTC秒中断优先级 */
	  RTC_NVIC_Config();
	  RTC_CheckAndConfig(&systmtime);
	
		/*设置闹钟寄存器*/
		clock_timestamp = mktimev(&clocktime)-TIME_ZOOM;
		RTC_SetAlarm(clock_timestamp);
		
	  while (1)
	  {
	    /* 每过1s 更新一次时间*/
	    if (TimeDisplay == 1)
	    {
				/* 当前时间 */
				current_timestamp = RTC_GetCounter();
	      Time_Display( current_timestamp,&systmtime); 
								
	      TimeDisplay = 0;
	    }
			
			//按下按键，通过串口修改时间
			if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
			{
				struct rtc_time set_time;

				/*使用串口接收设置的时间，输入数字时注意末尾要加回车*/
				Time_Regulate_Get(&set_time);
				/*用接收到的时间设置RTC*/
				Time_Adjust(&set_time);
				
				//向备份寄存器写入标志
				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);

			} 	
			
			//响铃
			if( TimeAlarm == 1)
			{
				BEEP(ON);
			}

			//按下按键，关闭蜂鸣器
			if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
			{
				BEEP(OFF);
				TimeAlarm = 0;
			}
	  }
}
```

## 3. 小结

### 3.1 流程概述：

1. **初始化 RTC**：
   
   - 配置 RTC 时钟源。
   - 配置 RTC 中的日期和时间。

2. **设置闹钟时间**：
   
   - 配置 RTC 的闹钟时间。
   - 配置 RTC 的闹钟中断。

3. **使能 RTC 中断**：
   
   - 配置 RTC 中断并启用相关 NVIC 中断服务。

4. **编写闹钟触发回调函数**：
   
   - 在中断服务程序里实现闹钟触发的响应，如点亮 LED、发出蜂鸣器等。

### 3.2 详细步骤与代码示例：

1. **RTC 初始化**：
   STM32F103 系列使用 LSE（低速外部晶振）作为 RTC 时钟源。

```c
#include "stm32f10x.h"

void RTC_Config(void) {
    // 启用 PWR 时钟，使能 RTC 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_LSEConfig(RCC_LSE_ON);  // 启用 LSE 时钟源
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);  // 等待 LSE 准备好
    
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  // 选择 LSE 作为 RTC 时钟源
    RCC_RTCCLKCmd(ENABLE);  // 启用 RTC 时钟

    // 启用 RTC
    PWR_BackupAccessCmd(ENABLE);  // 启用备份区域访问权限
    RTC_WaitForSynchro();  // 等待 RTC 同步
}

```

**设置当前时间**：
设置 RTC 中的时间（小时、分钟、秒）和日期（星期、日期、月份、年份）。

```c
void RTC_SetTime(uint8_t hour, uint8_t minute, uint8_t second) {
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_TimeStruct.RTC_Hours = hour;
    RTC_TimeStruct.RTC_Minutes = minute;
    RTC_TimeStruct.RTC_Seconds = second;
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStruct);  // 设置 RTC 时间
}

void RTC_SetDate(uint8_t year, uint8_t month, uint8_t date, uint8_t weekday) {
    RTC_DateTypeDef RTC_DateStruct;
    RTC_DateStruct.RTC_Year = year;
    RTC_DateStruct.RTC_Month = month;
    RTC_DateStruct.RTC_Date = date;
    RTC_DateStruct.RTC_WeekDay = weekday;
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStruct);  // 设置 RTC 日期
}

```

**设置闹钟时间**：
设置闹钟的时间，RTC 可以配置为闹钟触发时间。

```c
void RTC_SetAlarm(uint8_t hour, uint8_t minute, uint8_t second) {
    RTC_AlarmTypeDef RTC_AlarmStruct;
    RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours = hour;
    RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes = minute;
    RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds = second;
    RTC_AlarmStruct.RTC_AlarmDateWeekDay = RTC_AlarmDateWeekDay_1;  // 选择触发日期
    RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;  // 忽略日期
    RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStruct);  // 设置 RTC 闹钟时间
}

```

**使能闹钟中断**：
配置 RTC 的闹钟中断，使得当时间到达设定的闹钟时间时，可以触发中断。

```c
void RTC_AlarmInterruptConfig(void) {
    // 启用 RTC 闹钟中断
    RTC_ITConfig(RTC_IT_ALR, ENABLE);

    // 配置 NVIC 中断
    NVIC_EnableIRQ(RTC_Alarm_IRQn);  // 使能 RTC 闹钟中断
}

```

**RTC 中断服务程序**：
在 RTC 闹钟中断服务程序中，执行闹钟触发后的操作，例如点亮 LED 或发出蜂鸣器。

```c
void RTC_Alarm_IRQHandler(void) {
    if (RTC_GetITStatus(RTC_IT_ALR) != RESET) {
        RTC_ClearITPendingBit(RTC_IT_ALR);  // 清除中断标志

        // 执行闹钟触发后的操作（例如点亮 LED 或发出蜂鸣器）
        GPIO_SetBits(GPIOC, GPIO_Pin_13);  // 点亮 LED（假设连接到 PC13）
    }
}

```

**主函数**：
在主函数中调用以上函数来初始化 RTC，设置当前时间、日期，设置闹钟，并使能中断。

```c
int main(void) {
    // 初始化 STM32F103 的系统时钟
    SystemInit();

    // RTC 配置
    RTC_Config();

    // 设置当前时间 12:30:00
    RTC_SetTime(12, 30, 0);
    // 设置当前日期 2025年3月9日 星期日
    RTC_SetDate(25, 3, 9, RTC_Weekday_Sunday);

    // 设置闹钟时间 12:31:00
    RTC_SetAlarm(12, 31, 0);

    // 配置闹钟中断
    RTC_AlarmInterruptConfig();

    while (1) {
        // 进入低功耗模式
        __WFI();  // 等待中断
    }
}

```

### 3.3 总结：

- **RTC 初始化**：初始化 RTC 时钟源，配置时间和日期。
- **设置闹钟时间**：设置 RTC 闹钟时间。
- **中断配置**：使能 RTC 中断并编写中断处理程序。
- **中断响应**：在中断中执行闹钟响铃的操作，如点亮 LED 或发出蜂鸣器。
