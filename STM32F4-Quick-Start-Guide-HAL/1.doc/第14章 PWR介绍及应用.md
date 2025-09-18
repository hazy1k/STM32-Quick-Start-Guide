# 第十四章 PWR介绍及应用

## 1. 电源管理（PWR）简介

电源控制部分（PWR） 概述了不同电源域的电源架构以及电源配置控制器。 PWR 的内容比较多，我们把它们的主要特性概括为以下 3 点：

电源系统： USB 稳压器、内核域(VCORE)、 VDD 域、备份域、模拟域（VDDA）。

电源监控： POR/PDR 监控器、 BOR 监控器、 PVD 监控器、 AVD 监控器、 VBAT 阈值、温度阈值。

电源管理： VBAT 电池充电、工作模式、电压调节控制、低功耗模式。

### 1.1 电源系统

为了方便对电源系统进行管理，设计者把 STM32 的内核和外设等器件根据功能划分了不同的电源区域

![屏幕截图 2025-08-05 093744.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/05-09-38-24-屏幕截图%202025-08-05%20093744.png)

在电源概述框图中我们划分了 3 个区域①②③，分别是独立的 A/D 转换器供电和参考电压、电压调节器、电池备份区域。下面分别进行简单介绍：

① 电池备份区域（后备供电区域）

电池备份区域也就是后备供电区域，使用电池或者其他电源连接到 VBAT脚上，当 VDD断电时，可以保存备份寄存器的内容和维持 RTC 的功能。同时 VBAT 引脚也为 RTC、 SRAM 和 LSE振荡器供电，这保证了当主要电源被切断时， RTC 能够继续工作。切换到 VBAT供电由复位模块中的掉电复位功能控制。

② 电压调节器（VDD /1.2V 供电区域）

嵌入式线性调压器为备份域和待机电路以外的所有数字电路供电。调压器输出电压约为1.2 V。此调压器需要将两个外部电容连接到专用引脚 VCAP_1 和 VCAP_2，所有封装都配有这两个引脚。为激活或停用调压器，必须将特定引脚连接到 VSS 或 VDD。具体引脚与封装有关。通过软件激活时，调压器在复位后始终处于使能状态。

根据应用模式的不同，可采用三种不同的模式工作。 在运转模式下，调节器以正常工号模式为内核、内存和外设提供 1.2V；在停止模式下，调节器以低功耗模式提供 1.2V 电源，以保存寄存器和 SRAM 的内容。在待机模式下，调节器停止供电，除了备用电路和备份域外，寄存器和 SRAM 的内容全部丢失。

③ 独立的 A/D 转换器供电和参考电压（VDDA 供电区域）

为了提高转换精度， ADC 配有独立电源，可以单独滤波并屏蔽 PCB 上的噪声。 ADC 电源电压从单独的 VDDA 引脚接入， VSSA 提供了独立的电源接地连接。为了确保测量低电压时具有更高的精度，用户可以再 VREF 上连接到单独的 ADC 外部参考电压输入， VREF 电压介于1.8V 到 VDDA 之间。

### 1.2 电源管理

电源管理的部分我们要关注低功耗模式，在 STM32 的正常工作中，具有四种工作模式，运行、睡眠、停止以及待机。在上电复位后， STM32 处于运行状态时，当内核不需要继续运行，就可以选择进入后面的三种模式降低功耗。这三种低功耗模式电源消耗不同、唤醒时间不同和唤醒源不同，我们要根据自身的需要选择合适的低功耗模式。

| 特性      | 睡眠模式（Sleep） | 停机模式（Stop）     | 待机模式（Standby）   |
| ------- | ----------- | -------------- | --------------- |
| 内核状态    | 停止          | 停止             | 停止（完全关闭）        |
| RAM 保持  | 是           | 是              | 否（除非备份域供电）      |
| 寄存器保持   | 是           | 是              | 否               |
| 功耗      | 中等          | 低（μA级）         | 极低（nA级）         |
| 唤醒时间    | 极快（几周期）     | 快（μs级）         | 慢（需系统重启）        |
| 唤醒后运行状态 | 继续执行        | 继续执行           | 重新启动（复位）        |
| 唤醒源     | 任意中断/事件     | EXTI、RTC、WKUP等 | WKUP引脚、RTC闹钟、复位 |

## 2. PWR基础使用示例

### 2.1 PVD电压监控

开发板供电正常的话， LCD 屏会显示"PVD Voltage OK!"。当供电电压过低，则会通过 PVD中断服务函数将 LED1 点亮；当供电电压正常，会在 PVD 中断服务函数将 LED1 熄灭。 LED0闪烁，提示程序运行。

#### 2.1.1 PVD初始化

```c
void pwr_pvd_init(uint32_t pls)
{
    PWR_PVDTypeDef pvd_handle = {0};
    __HAL_RCC_PWR_CLK_ENABLE();
    pvd_handle.PVDLevel = pls; // 设置PVD电压监视器等级
    HAL_PWR_ConfigPVD(&pvd_handle); // 配置PVD
    HAL_NVIC_SetPriority(PVD_IRQn,3,3);
    HAL_NVIC_EnableIRQ(PVD_IRQn); // 使能PVD中断
    HAL_PWR_EnablePVD();
}
```

#### 2.1.2 PVD中断函数

```c
void PVD_IRQHandler(void)
{
    HAL_PWR_PVD_IRQHandler();
}

void HAL_PWR_PVDCallback(void)
{
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO)) // 电压比PLS所选电压更低，触发中断
    {
        LCD_ShowString(30,130,200,16,16,"PVD Low Voltage Detected!");
        LED_ON(LED1_GPIO_Pin);
    }
    else
    {
        LCD_ShowString(30,130,200,16,16,"PVD Normal Voltage!");
        LED_OFF(LED1_GPIO_Pin);
    }
}
```

#### 2.1.3 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "pvd.h"

int main(void)
{
  uint8_t i = 0;
  bsp_init();
  LCD_ShowString(30,50,200,16,16,"PVD Test");    
  pwr_pvd_init(PWR_PVDLEVEL_7); // PVD 2.9V检  
    while(1)
  {
    if((i%20) == 0)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
    }
    delay_ms(10);
    i++;
  }
}
```

### 2.2 睡眠模式测试

#### 2.2.1 唤醒按键初始化

```c
// 低功耗模式下按键初始化，用于唤醒
void pwr_wkup_key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    WKUP_GPIO_CLK_ENABLE();
    GPIO_InitStructure.Pin = WKUP_GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING; // 上升沿触发
    GPIO_InitStructure.Pull = GPIO_PULLDOWN; // 下拉
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(WKUP_GPIO_Port, &GPIO_InitStructure);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
```

#### 2.2.2 中断函数

```c
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(WKUP_GPIO_Pin);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == WKUP_GPIO_Pin)
    {
        printf("WKUP Key pressed\r\n");
    }
    /* HAL_GPIO_EXTI_IRQHandler()函数已经为我们清除了中断标志位，所以我们进了回调函数可以不做任何事 */
}
```

#### 2.2.3 进入睡眠模式

```c
// 进入睡眠模式
void pwr_enter_sleep(void)
{
    HAL_SuspendTick(); // 关闭Tick中断
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); // 进入低功耗模式
}
```

#### 2.2.4 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "pwr.h"

int main(void)
{
  uint8_t i = 0;
  uint8_t key;
  bsp_init();
  pwr_wkup_key_init();
  LCD_ShowString(30,50,200,16,16,"PWR Sleep Test");    
    while(1)
  {
    key = key_scan(0);
    if(key == KEY0_Press) // 按下KEY0进入睡眠模式，按下WKUP键唤醒
    {
      LED_ON(LED1_GPIO_Pin);
      printf("Enter Sleep Mode\r\n");
      delay_ms(1000);
      pwr_enter_sleep(); // 进入睡眠模式
      HAL_ResumeTick();
      LED_OFF(LED1_GPIO_Pin);
    }
    if((i%20) == 0)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
    }
    delay_ms(10);
    i++;
  }
}
```

### 2.3 停止模式测试

#### 2.3.1 进入停止模式

```c
// 进入停止模式
void pwr_enter_stop(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_SuspendTick(); // 关闭Tick中断
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}
```

#### 2.3.2 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "pwr.h"

int main(void)
{
  uint8_t i = 0;
  uint8_t key;
  bsp_init();
  pwr_wkup_key_init();
  LCD_ShowString(30,50,200,16,16,"PWR Stop Test");    
    while(1)
  {
    key = key_scan(0);
    if(key == KEY0_Press) // 按下KEY0进入停止模式，按下WKUP键唤醒
    {
      LED_ON(LED1_GPIO_Pin);
      printf("Enter Stop Mode\r\n");
      delay_ms(1000);
      pwr_enter_stop(); // 进入停止模式

      /*从停止模式唤醒，需要重新配置时钟*/
      sys_stm32_clock_init(336,8,2,7);
      HAL_ResumeTick();
      LED_OFF(LED1_GPIO_Pin);
    }
    if((i%20) == 0)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
    }
    delay_ms(10);
    i++;
  }
}
```

### 2.4 待机模式测试

#### 2.4.1 进入待机模式

```c
// 进入待机模式
void pwr_enter_standby(void)
{
    __HAL_RCC_AHB1_FORCE_RESET(); // 强制复位所有AHB1总线
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_BACKUPRESET_FORCE();
    HAL_PWR_EnableBkUpAccess();
    /* STM32F4,当开启了RTC相关中断后,必须先关闭RTC中断,再清中断标志位,然后重新设置 */
    /* RTC中断,再进入待机模式才可以正常唤醒,否则会有问题. */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    // __HAL_RTC_WRITEPROTECTION_DISABLE(&g_rtc_handle);   /* 关闭RTC写保护 */
    /* 关闭RTC相关中断，可能在RTC实验打开了 */
    // __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&g_rtc_handle, RTC_IT_WUT);
    // __HAL_RTC_TIMESTAMP_DISABLE_IT(&g_rtc_handle, RTC_IT_TS);
    // __HAL_RTC_ALARM_DISABLE_IT(&g_rtc_handle, RTC_IT_ALRA|RTC_IT_ALRB);

    /* 清除RTC相关中断标志位 */
    // __HAL_RTC_ALARM_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
    // __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_TSF); 
    // __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&g_rtc_handle, RTC_FLAG_WUTF);

    __HAL_RCC_BACKUPRESET_RELEASE();                    /* 备份区域复位结束 */
    // __HAL_RTC_WRITEPROTECTION_ENABLE(&g_rtc_handle); /* 使能RTC写保护 */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  /* 清除Wake_UP标志 */

    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           /* 设置WKUP用于唤醒 */
    HAL_PWR_EnterSTANDBYMode();                         /* 进入待机模式 */
}
```

#### 2.4.2 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "pwr.h"

int main(void)
{
  uint8_t i = 0;
  uint8_t key;
  bsp_init();
  pwr_wkup_key_init();
  LCD_ShowString(30,50,200,16,16,"PWR Standby Test");	
	while(1)
  {
    key = key_scan(0);
    if(key == KEY0_Press) // 按下KEY0进入停止模式，按下WKUP键唤醒
    {
      LED_ON(LED1_GPIO_Pin);
      printf("Enter Standby Mode\r\n");
      delay_ms(1000);
      pwr_enter_standby(); // 进入待机模式
      /* 从待机模式唤醒相当于系统重启(复位), 因此不会执行到这里 */
    }
    if((i%20) == 0)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
    }
    delay_ms(10);
    i++;
  }
}

```

## 3. PWR常见函数（HAL库）

### 3.1 PWR 初始化与配置

#### 3.1.1 `HAL_PWR_DeInit()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_PWR_DeInit(void)
```

**功能**:

- 重置 PWR 寄存器到默认值

- 禁用 PVD、唤醒引脚等所有 PWR 功能

**示例**:

```c
if (HAL_PWR_DeInit() != HAL_OK) {
  Error_Handler();
}
```

### 3.2 低功耗模式控制

#### 3.2.1 睡眠模式

**函数原型**:

```c
void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry)
```

**参数**:

- `Regulator`:
  
  - `PWR_MAINREGULATOR_ON`: 主调节器开启
  
  - `PWR_LOWPOWERREGULATOR_ON`: 低功耗调节器开启

- `SLEEPEntry`:
  
  - `PWR_SLEEPENTRY_WFI`: 通过 WFI 指令进入
  
  - `PWR_SLEEPENTRY_WFE`: 通过 WFE 指令进入

**功能**:

- 进入睡眠模式（最低功耗模式）

**示例**:

```c
HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
```

#### 3.2.2 停止模式

**函数原型**:

```c
void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry)
```

**参数**:

- `Regulator`:
  
  - `PWR_MAINREGULATOR_ON`: 主调节器开启
  
  - `PWR_LOWPOWERREGULATOR_ON`: 低功耗调节器开启

- `STOPEntry`:
  
  - `PWR_STOPENTRY_WFI`: 通过 WFI 指令进入
  
  - `PWR_STOPENTRY_WFE`: 通过 WFE 指令进入

**功能**:

- 进入停止模式（深度睡眠模式）

- 唤醒后需要重新配置时钟系统

**示例**:

```c
// 进入停止模式
HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

// 唤醒后重新配置时钟
SystemClock_Config();
```

#### 3.2.3 待机模式

**函数原型**:

```c
void HAL_PWR_EnterSTANDBYMode(void)
```

**功能**:

- 进入待机模式（最低功耗模式）

- 唤醒后相当于系统复位

**示例**:

```c
// 配置唤醒源
HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

// 进入待机模式
HAL_PWR_EnterSTANDBYMode();
```

### 3.3 可编程电压检测器 (PVD)

#### 3.3.1 `HAL_PWR_ConfigPVD()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_PWR_ConfigPVD(PWR_PVDTypeDef *sConfigPVD)
```

**参数结构体**:

```c
typedef struct {
 uint32_t PVDLevel; // PVD 检测阈值
 uint32_t Mode; // PVD 模式
} PWR_PVDTypeDef;
```

**阈值选项**:

```c
PWR_PVDLEVEL_0 // 2.0V
PWR_PVDLEVEL_1 // 2.2V
PWR_PVDLEVEL_2 // 2.4V
PWR_PVDLEVEL_3 // 2.5V
PWR_PVDLEVEL_4 // 2.6V
PWR_PVDLEVEL_5 // 2.8V
PWR_PVDLEVEL_6 // 2.9V
PWR_PVDLEVEL_7 // 3.0V
```

**模式选项**:

```c
PWR_PVD_MODE_NORMAL // 正常模式
PWR_PVD_MODE_IT_RISING // 上升沿中断
PWR_PVD_MODE_IT_FALLING // 下降沿中断
PWR_PVD_MODE_IT_RISING_FALLING // 双边沿中断
PWR_PVD_MODE_EVENT_RISING // 上升沿事件
PWR_PVD_MODE_EVENT_FALLING // 下降沿事件
PWR_PVD_MODE_EVENT_RISING_FALLING // 双边沿事件
```

**示例配置**:

```c
PWR_PVDTypeDef sConfigPVD;
sConfigPVD.PVDLevel = PWR_PVDLEVEL_6; // 2.9V
sConfigPVD.Mode = PWR_PVD_MODE_IT_FALLING; // 电压低于阈值时触发中断

HAL_PWR_ConfigPVD(&sConfigPVD);
HAL_PWR_EnablePVD();
```

#### 3.3.2 PVD 控制函数

```c
// 使能 PVD
void HAL_PWR_EnablePVD(void);

// 禁用 PVD
void HAL_PWR_DisablePVD(void);
```

#### 3.3.3 PVD 中断回调

```c
// 用户需重写此函数
void HAL_PWR_PVDCallback(void)
{
 // 处理 PVD 事件
 if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO)) {
 // 电压低于阈值
 HandleLowVoltage();
 }
}
```

### 3.4 唤醒控制

#### 3.4.1 唤醒引脚控制

```c
// 使能唤醒引脚
void HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinx);

// 禁用唤醒引脚
void HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx);
```

**引脚选项**:

```c
PWR_WAKEUP_PIN1 // PA0
PWR_WAKEUP_PIN2 // PC13
PWR_WAKEUP_PIN3 // PE6 // 仅适用于特定封装
PWR_WAKEUP_PIN4 // PA2 // 仅适用于特定封装
PWR_WAKEUP_PIN5 // PC5 // 仅适用于特定封装
```

**示例**:

```c
// 使能 PA0 作为唤醒引脚
HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
```

#### 3.4.2 清除唤醒标志

```c
void HAL_PWR_ClearFlag(uint32_t Flag);
```

**标志选项**:

```c
PWR_FLAG_WU // 唤醒标志
PWR_FLAG_SB // 待机标志
PWR_FLAG_PVDO // PVD 输出标志
PWR_FLAG_VOS // 电压调节器就绪标志
```

### 3.5 备份域控制

#### 3.5.1 备份域访问控制

```c
// 使能备份域访问
void HAL_PWR_EnableBkUpAccess(void);

// 禁用备份域访问
void HAL_PWR_DisableBkUpAccess(void);
```

**重要**:

- 访问备份寄存器或 RTC 前必须使能

- 需要先使能 PWR 时钟: `__HAL_RCC_PWR_CLK_ENABLE()`

#### 3.5.2 备份寄存器操作

```c
// 写入备份寄存器
void HAL_PWR_EnableBkUpReg(void);
void HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister, uint32_t Data);

// 读取备份寄存器
uint32_t HAL_RTCEx_BKUPRead(RTC_HandleTypeDef *hrtc, uint32_t BackupRegister);
```

**示例**:

```c
// 写入备份寄存器
HAL_PWR_EnableBkUpAccess();
HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x1234);
HAL_PWR_DisableBkUpAccess();

// 读取备份寄存器
HAL_PWR_EnableBkUpAccess();
uint32_t data = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0);
HAL_PWR_DisableBkUpAccess();
```

### 3.6 电压调节器控制

#### 3.6.1 过驱动模式 (高性能)

```c
// 使能过驱动模式
HAL_StatusTypeDef HAL_PWREx_EnableOverDrive(void);

// 禁用过驱动模式
HAL_StatusTypeDef HAL_PWREx_DisableOverDrive(void);
```

**功能**:

- 提高系统时钟频率上限（最高 168MHz）

- 增加功耗，提升性能

**使用流程**:

```c
// 1. 使能过驱动
HAL_PWREx_EnableOverDrive();

// 2. 等待过驱动使能就绪
while (__HAL_PWR_GET_FLAG(PWR_FLAG_ODRDY) == RESET) {}

// 3. 配置高系统时钟
SystemClock_Config_OverDrive();

// 4. 禁用过驱动
// HAL_PWREx_DisableOverDrive();
```

#### 3.6.2 主调节器输出电压

```c
// 设置调节器输出电压
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling);
```

**电压选项**:

```c
PWR_REGULATOR_VOLTAGE_SCALE1 // 高性能模式 (1.8V)
PWR_REGULATOR_VOLTAGE_SCALE2 // 平衡模式 (1.5V)
PWR_REGULATOR_VOLTAGE_SCALE3 // 低功耗模式 (1.2V)
```

### 3.7 FLASH 电源控制

#### 3.7.1 FLASH 深度睡眠控制

```c
// 使能 FLASH 深度睡眠
HAL_StatusTypeDef HAL_PWREx_EnableFlashPowerDown(void);

// 禁用 FLASH 深度睡眠
HAL_StatusTypeDef HAL_PWREx_DisableFlashPowerDown(void);
```

**功能**:

- 在停止模式下进一步降低功耗

- 唤醒后需要等待 FLASH 就绪

### 3.8 电源状态监控

#### 3.8.1 获取电源标志

```c
// 检查电源标志
FlagStatus HAL_PWR_GetFlag(uint32_t Flag);

// 清除电源标志
void HAL_PWR_ClearFlag(uint32_t Flag);
```

**常用标志**:

```c
PWR_FLAG_WU // 唤醒标志
PWR_FLAG_SB // 待机标志
PWR_FLAG_PVDO // PVD输出标志
PWR_FLAG_BRR // 备份调节器就绪标志
PWR_FLAG_VOSRDY // 电压调节器就绪标志
```

#### 3.8.2 电源状态检查

```c
// 检查调节器是否就绪
if (__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY) {
 // 调节器已就绪
}

// 检查PVD状态
if (__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO)) {
 // 电压低于阈值
}
```

| **模式**   | **功耗** | **唤醒时间** | **数据保留** | **唤醒源**       |
| -------- | ------ | -------- | -------- | ------------- |
| **运行模式** | 最高     | 立即       | 全部       | N/A           |
| **睡眠模式** | 中等     | 立即       | 全部       | 任意中断          |
| **停止模式** | 低      | 快速       | SRAM+寄存器 | 外部中断、RTC等     |
| **待机模式** | 最低     | 慢        | 备份寄存器    | WKUP引脚、RTC闹钟等 |

---
