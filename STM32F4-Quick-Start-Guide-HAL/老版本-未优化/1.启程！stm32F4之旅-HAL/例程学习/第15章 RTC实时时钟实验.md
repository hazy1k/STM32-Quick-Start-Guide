# 第十五章 RTC实时时钟实验

## 1. 硬件设计

本实验用到的硬件资源有：

- 指示灯 DS0 

- 串口

- TFTLCD 模块

- RTC

前面 3 个都介绍过了，而 RTC 属于 STM32F4 内部资源，其配置也是通过软件设置好就可以了。不过 RTC 不能断电，否则数据就丢失了， 我们如果想让时间在断电后还可以继续走，那么必须确保开发板的电池有电（ALIENTEK 探索者 STM32F4 开发板标配是有电池的）。

## 2. 软件设计

### 2.1 RTC时间设置

```c
//RTC时间设置
//返回值:SUCEE(1),成功
//      ERROR(0),进入初始化模式失败 
HAL_StatusTypeDef RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeStructure;
	
	RTC_TimeStructure.Hours = hour;
	RTC_TimeStructure.Minutes = min;
	RTC_TimeStructure.Seconds = sec;
	RTC_TimeStructure.TimeFormat = ampm;
	RTC_TimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStructure.StoreOperation = RTC_STOREOPERATION_RESET;
	return HAL_RTC_SetTime(&RTC_Handler, &RTC_TimeStructure,RTC_FORMAT_BIN);	
}
```

### 2.2 RTC日期设置

```c
//RTC日期设置
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
HAL_StatusTypeDef RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	RTC_DateTypeDef RTC_DateStructure;
    
	RTC_DateStructure.Date = date;
	RTC_DateStructure.Month = month;
	RTC_DateStructure.WeekDay = week;
	RTC_DateStructure.Year = year;
	return HAL_RTC_SetDate(&RTC_Handler, &RTC_DateStructure,RTC_FORMAT_BIN);
}
```

### 2.3 RTC初始化

```c
//RTC初始化
//返回值:0,初始化成功;
//       2,进入初始化模式失败;
u8 RTC_Init(void)
{      

	RTC_Handler.Instance=RTC;
    RTC_Handler.Init.HourFormat=RTC_HOURFORMAT_24;// RTC设置为24小时格式 
    RTC_Handler.Init.AsynchPrediv=0X7F;           // RTC异步分频系数(1~0X7F)
    RTC_Handler.Init.SynchPrediv=0XFF;            // RTC同步分频系数(0~7FFF)   
    RTC_Handler.Init.OutPut=RTC_OUTPUT_DISABLE;   // RTC输出禁止
    RTC_Handler.Init.OutPutPolarity=RTC_OUTPUT_POLARITY_HIGH; // RTC输出极性高
    RTC_Handler.Init.OutPutType=RTC_OUTPUT_TYPE_OPENDRAIN; // RTC输出类型开漏
    if(HAL_RTC_Init(&RTC_Handler)!=HAL_OK) return 2;
      
    if(HAL_RTCEx_BKUPRead(&RTC_Handler,RTC_BKP_DR0)!=0X5050)//是否第一次配置
    { 
        RTC_Set_Time(17,41,0,RTC_HOURFORMAT12_PM);	        //设置时间 ,根据实际时间修改
		RTC_Set_Date(2024,10,11,2);		                    //设置日期
        HAL_RTCEx_BKUPWrite(&RTC_Handler,RTC_BKP_DR0,0X5050);//标记已经初始化过了
    }
    return 0;
}
```

### 2.4 RTC时钟配置

```c
//RTC底层驱动，时钟配置
//此函数会被HAL_RTC_Init()调用
//hrtc:RTC句柄
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    __HAL_RCC_PWR_CLK_ENABLE(); // 使能电源时钟PWR
	HAL_PWR_EnableBkUpAccess(); // 取消备份区域写保护
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE; // LSE配置
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // LSE不作为PLL来源
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;       //RTC使用LSE
    HAL_RCC_OscConfig(&RCC_OscInitStruct); // 配置外部时钟
    PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC; // 外设为RTC
    PeriphClkInitStruct.RTCClockSelection=RCC_RTCCLKSOURCE_LSE; // RTC时钟源为LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct); // 配置RTC时钟
    __HAL_RCC_RTC_ENABLE();//RTC时钟使能
}
```

### 2.5 设置闹钟时间

```c
//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7) @ref  RTC_WeekDay_Definitions
//hour,min,sec:小时,分钟,秒钟
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{ 
    RTC_AlarmTypeDef RTC_AlarmSturuct;
    
    RTC_AlarmSturuct.AlarmTime.Hours=hour;  // 小时
    RTC_AlarmSturuct.AlarmTime.Minutes=min; // 分钟
    RTC_AlarmSturuct.AlarmTime.Seconds=sec; // 秒
    RTC_AlarmSturuct.AlarmTime.SubSeconds=0; // 设置亚秒为0
    RTC_AlarmSturuct.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM; // 24小时制
    RTC_AlarmSturuct.AlarmMask=RTC_ALARMMASK_NONE; // 精确匹配星期，时分秒
    RTC_AlarmSturuct.AlarmSubSecondMask=RTC_ALARMSUBSECONDMASK_NONE; // 无亚秒匹配
    RTC_AlarmSturuct.AlarmDateWeekDaySel=RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;//按星期
    RTC_AlarmSturuct.AlarmDateWeekDay=week; // 星期
    RTC_AlarmSturuct.Alarm=RTC_ALARM_A;     // 闹钟A
    HAL_RTC_SetAlarm_IT(&RTC_Handler,&RTC_AlarmSturuct,RTC_FORMAT_BIN);
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn,0x01,0x02); //抢占优先级1,子优先级2
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}
```

### 2.6 RTC周期唤醒函数

```c
//周期性唤醒定时器设置  
/*wksel:  @ref RTCEx_Wakeup_Timer_Definitions
#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
void RTC_Set_WakeUp(u32 wksel, u16 cnt)
{ 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handler, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志
	HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler,cnt,wksel);          //设置重装载值和时钟 
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn,0x02,0x02); // 抢占优先级1,子优先级2
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}
```

### 2.7 RTC中断相关函数

```c
//RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RTC_Handler);
}
    
//RTC闹钟A中断处理回调函数
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    printf("ALARM A!\r\n");
}

//RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&RTC_Handler); 
}

//RTC WAKE UP中断处理
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    LED1=!LED1;
}
```

### 2.8 主函数

```c
int main(void)
{
	RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    u8 tbuf[40];
	u8 t=0;
	
    HAL_Init();                  // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7); // 设置时钟,168Mhz
	delay_init(168);             // 初始化延时函数
	uart_init(115200);           // 初始化USART
	LED_Init();				     // 初始化LED	
 	LCD_Init();           	     // 初始化LCD
	usmart_dev.init(84); 		 // 初始化USMART	
    RTC_Init();                  // 初始化RTC 
    RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0); // 配置WAKE UP中断,1秒钟中断一次  
    POINT_COLOR=RED; // 设置背景色为红色
	LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");	
	LCD_ShowString(30,70,200,16,16,"RTC TEST");	
	LCD_ShowString(30,90,200,16,16,"by hazy1k");
	LCD_ShowString(30,110,200,16,16,"2024/10/11");		   
    while(1)
    {
		t++;
		if((t%10)==0) // 每100ms更新一次显示数据
		{
            HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN); // 获取当前时间
			sprintf((char*)tbuf,"Time:%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); // 打印时间
			LCD_ShowString(30,140,210,16,16,tbuf); // 显示时间
            HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN); // 获取当前日期
			sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date); // 打印日期 
			LCD_ShowString(30,160,210,16,16,tbuf); // 显示日期
			sprintf((char*)tbuf,"Week:%d",RTC_DateStruct.WeekDay); // 打印星期
			LCD_ShowString(30,180,210,16,16,tbuf); // 显示星期
		} 
		if((t%20)==0)LED0=!LED0; // 每200ms,翻转一次LED0 
        delay_ms(10);
	}
}
```

## 3. 小结

下面我们简单回顾一下：

```c
#include "stm32f4xx_hal.h"  // 引入 HAL 库
// RTC 句柄
RTC_HandleTypeDef hrtc;
// USART 句柄（用于串口通信）
UART_HandleTypeDef huart2;
// 错误处理函数
void Error_Handler(void) {
    // 用户可以添加自己的错误处理代码
    while(1) {
        // 进入死循环，等待调试
    }
}
// RTC 初始化函数
void RTC_Init(void) {
    // 启用 RTC 时钟
    __HAL_RCC_RTC_ENABLE();
    // 配置 RTC
    hrtc.Instance = RTC;  // 设置 RTC 实例
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;  // 设置为24小时制
    hrtc.Init.AsynchPrediv = 127;               // 设置异步预分频器
    hrtc.Init.SynchPrediv = 255;                 // 设置同步预分频器
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;       // 禁用输
    // 初始化 RTC，检查返回值以确保成功
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        Error_Handler();  // 初始化失败，调用错误处理函数
    }
    // 设置初始时间和日期
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    sTime.Hours = 12;  // 初始小时
    sTime.Minutes = 0; // 初始分钟
    sTime.Seconds = 0; // 初始秒钟
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);  // 设置时间
    sDate.Year = 24;   // 设置年份 (2024)
    sDate.Month = RTC_MONTH_JANUARY; // 设置月份 (1月)
    sDate.Date = 1;    // 设置日期 (1号)
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);  // 设置日期
}

// USART2 初始化函数
void USART2_Init(void) {
    __HAL_RCC_USART2_CLK_ENABLE();  // 启用 USART2 时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();    // 启用 GPIOA 时钟
    // 配置 USART2 引脚 (PA2 为 TX, PA3 为 RX)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3; // TX 和 RX 引脚
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;        // 复用推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;            // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // 高速
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;   // 复用功能
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);         // 初始化引脚
    // 配置 USART 参数
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;                  // 波特率
    huart2.Init.WordLength = UART_WORDLENGTH_8B;   // 数据位长度
    huart2.Init.StopBits = UART_STOPBITS_1;        // 停止位
    huart2.Init.Parity = UART_PARITY_NONE;          // 无奇偶校验
    huart2.Init.Mode = UART_MODE_TX_RX;             // 收发模式
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;    // 无硬件流控制
    huart2.Init.OverSampling = UART_OVERSAMPLING_16; // 过采样
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();  // 初始化失败，调用错误处理函数
    }
}
// 主函数
int main(void) {
    HAL_Init();          // 初始化 HAL 库
    SystemClock_Config(); // 配置系统时钟
    USART2_Init();       // 初始化 USART2
    RTC_Init();          // 初始化 RTC
    // 主循环
    while (1) {
        RTC_TimeTypeDef sTime;
        char buffer[50];
        // 获取当前时间
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        // 格式化时间字符串
        snprintf(buffer, sizeof(buffer), "Current Time: %02d:%02d:%02d\r\n",
                 sTime.Hours, sTime.Minutes, sTime.Seconds);
   
        // 通过串口发送时间
        HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
        // 延时 1 秒
        HAL_Delay(1000);
    }
}
// 系统时钟配置函数 (根据具体需要实现)
void SystemClock_Config(void) {
    // 这里需要配置系统时钟，具体实现取决于你的硬件设计
}

```


