# 第十七章 STM32F4 RTC时钟简介

## 1. RTC简单框图

STM32F4 的实时时钟（RTC） 相对于 STM32F1 来说，改进了不少，带了日历功能了， STM32F4 的 RTC，是一个独立的 BCD 定时器/计数器。 RTC 提供一个日历时钟（包含年月日时分秒信息）、两个可编程闹钟（ALARM A 和 ALARM B）中断，以及一个具有中断功能的周期性可编程唤醒标志。 RTC 还包含用于管理低功耗模式的自动唤醒单元。

两个 32 位寄存器（TR 和 DR）包含二进码十进数格式 (BCD) 的秒、分钟、小时（12 或24 小时制）、星期、日期、月份和年份。此外，还可提供二进制格式的亚秒值。

STM32F4 的 RTC 可以自动将月份的天数补偿为 28、 29（闰年）、 30 和 31 天。并且还可以进行夏令时补偿。

RTC 模块和时钟配置是在后备区域，即在系统复位或从待机模式唤醒后 RTC 的设置和时间维持不变，只要后备区域供电正常，那么 RTC 将可以一直运行。但是在系统复位后，会自动禁止访问后备寄存器和 RTC，以防止对后备区域(BKP)的意外写操作。所以在要设置时间之前，先要取消备份区域（BKP）写保护。

![STM32F4开发指南 HAL库版本 V1.2](https://img.picgo.net/2024/10/09/STM32F4-HAL_V1.2e45e8bec99c0fb3c.png)

## 2. STM32F4 RTC 时钟的使用

### 2.1 时钟和分频

先，我们看 STM32F4 的 RTC 时钟分频。 STM32F4 的 RTC 时钟源（RTCCLK）通过时钟控制器，可以从 LSE 时钟、 LSI 时钟以及 HSE 时钟三者中选择（通过 RCC_BDCR 寄存器选择）。一般我们选择 LSE，即外部 32.768Khz 晶振作为时钟源(RTCCLK)，而 RTC 时钟核心，要求提供 1Hz 的时钟，所以，我们要设置 RTC 的可编程预分配器。 STM32F4 的可编程预分配器（RTC_PRER）分为 2 个部分：

1. 一个通过 RTC_PRER 寄存器的 PREDIV_A 位配置的 7 位异步预分频器

2. 一个通过 RTC_PRER 寄存器的 PREDIV_S 位配置的 15 位同步预分频器

ck_spre 的时钟可由如下计算公式计算：

![屏幕截图 2024 10 09 091829](https://img.picgo.net/2024/10/09/-2024-10-09-091829420a12d531fa629f.png)

其中， Fck_spre 即可用于更新日历时间等信息。 PREDIV_A 和 PREDIV_S 为 RTC 的异步和同步分频器。且推荐设置 7 位异步预分频器（PREDIV_A）的值较大，以最大程度降低功耗。

要设置为 32768 分频，我们只需要设置： PREDIV_A=0X7F，即 128 分频； PREDIV_S=0XFF，即 256 分频， 即可得到 1Hz 的 Fck_spre。

另外，ck_apre 可作为 RTC 亚秒递减计数器（RTC_SSR）的时钟， Fck_apre的计算公式如下：

![屏幕截图 2024 10 09 092141](https://img.picgo.net/2024/10/09/-2024-10-09-09214161efe99a020f94d4.png)

当 RTC_SSR 寄存器递减到 0 的时候，会使用 PREDIV_S 的值重新装载 PREDIV_S。而PREDIV_S 一般为 255，这样，我们得到亚秒时间的精度是： 1/256 秒，即 3.9ms 左右，有了这个亚秒寄存器 RTC_SSR，就可以得到更加精确的时间数据。

### 2.2 日历时间（RTC_TR）和日期（RTC_DR）寄存器

STM32F4 的 RTC 日历时间（RTC_TR） 和日期（RTC_DR）寄存器，用于存储时间和日期（也可以用于设置时间和日期），可以通过与 PCLK1（APB1 时钟）同步的影子寄存器来访问，这些时间和日期寄存器也可以直接访问，这样可避免等待同步的持续时间。

每隔 2 个 RTCCLK 周期，当前日历值便会复制到影子寄存器，并置位 RTC_ISR 寄存器的RSF 位。我们可以读取 RTC_TR 和 RTC_DR 来得到当前时间和日期信息，不过需要注意的是：时间和日期都是以 BCD 码的格式存储的，读出来要转换一下，才可以得到十进制的数据。

### 2.3 可编程闹钟

STM32F4 提供两个可编程闹钟：闹钟 A（ALARM_A）和闹钟 B（ALARM_B）。通过 RTC_CR寄存器的 ALRAE 和 ALRBE 位置 1 来使能可编程闹钟功能。当日历的亚秒、秒、分、小时、日期分别与闹钟寄存器 RTC_ALRMASSR/RTC_ALRMAR 和 RTC_ALRMBSSR/RTC_ALRMBR中的值匹配时，则可以产生闹钟（需要适当配置）。本章我们将利用闹钟 A 产生闹铃，即设置RTC_ALRMASSR 和 RTC_ALRMAR 即可。

### 2.4 周期性自动唤醒

STM32F4 的 RTC 不带秒钟中断了，但是多了一个周期性自动唤醒功能。周期性唤醒功能，由一个 16 位可编程自动重载递减计数器（RTC_WUTR）生成，可用于周期性中断/唤醒。

我们可以通过 RTC_CR 寄存器中的 WUTE 位设置使能此唤醒功能。

唤醒定时器的时钟输入可以是： 2、 4、 8 或 16 分频的 RTC 时钟(RTCCLK)，也可以是 ck_spre时钟（一般为 1Hz）。

当选择 RTCCLK(假定 LSE 是： 32.768 kHz)作为输入时钟时，可配置的唤醒中断周期介于122us（因为 RTCCLK/2 时， RTC_WUTR 不能设置为 0）和 32 s 之间，分辨率最低为： 61us。

当选择 ck_spre（1Hz）作为输入时钟时，可得到的唤醒时间为 1s 到 36h 左右，分辨率为 1 秒。并且这个 1s~36h 的可编程时间范围分为两部分：

当 WUCKSEL[2:1]=10 时为： 1s 到 18h。

当 WUCKSEL[2:1]=11 时约为： 18h 到 36h。

在后一种情况下，会将 2^16 添加到 16 位计数器当前值（即扩展到 17 位，相当于最高位用WUCKSEL [1]代替）。

初始化完成后，定时器开始递减计数。在低功耗模式下使能唤醒功能时，递减计数保持有效。此外，当计数器计数到 0 时， RTC_ISR 寄存器的 WUTF 标志会置 1，并且唤醒寄存器会使用其重载值（RTC_WUTR 寄存器值）动重载，之后必须用软件清零 WUTF 标志。

通过将 RTC_CR 寄存器中的 WUTIE 位置 1 来使能周期性唤醒中断时，可以使 STM32F4退出低功耗模式。系统复位以及低功耗模式（睡眠、停机和待机）对唤醒定时器没有任何影响，它仍然可以正常工作，故唤醒定时器，可以用于周期性唤醒 STM32F4

## 3. RTC部分寄存器

### 3.1 时间寄存器（RTC_TR）

![屏幕截图 2024 10 09 134055](https://img.picgo.net/2024/10/09/-2024-10-09-134055a681a9dd88a312b8.png)

这个寄存器比较简单，注意数据保存是 BCD 格式的，读取之后需要稍加转换，才是十进制的时分秒等数据，在初始化模式下，对该寄存器进行写操作，可以设置时间。

下面是一个简单的代码示例，展示如何初始化 RTC，并设置当前时间：

```c
#include "stm32f4xx_hal.h"

// 假设已经初始化了系统时钟和 RTC

void RTC_Init(void) {
    // 配置 RTC 的时钟源等
    // 使能 RTC 时钟，具体实现依赖于所用系列的 HAL 库
    // 初始化 RTC 时间
    RTC_TimeTypeDef sTime = {0};
    // 设置当前时间为 12:30:45
    sTime.Hours = 12;     // 12点
    sTime.Minutes = 30;   // 30分
    sTime.Seconds = 45;   // 45秒
    sTime.TimeFormat = RTC_HOURFORMAT12_PM; // 12小时制，PM
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        // 错误处理
    }
}
int main(void)
{
    HAL_Init(); // 初始化 HAL 库
    RTC_Init(); // 初始化 RTC
    while (1) {
        // 主循环
    }
}
```

### 3.2 日期寄存器（RTC_DR）

![屏幕截图 2024 10 09 134720](https://img.picgo.net/2024/10/09/-2024-10-09-1347203f210bf44566bde4.png)

同样，该寄存器的的数据采用 BCD 码格式，其他的就比较简单了。同样，在初始化模式下，对该寄存器进行写操作，可以设置日期。

以下是一个简单的代码示例，展示如何初始化 RTC，并设置当前日期：

```c
#include "stm32f4xx_hal.h"
// 假设已经初始化了系统时钟和 RTC
void RTC_Init(void) {
    // 配置 RTC 的时钟源等
    // 使能 RTC 时钟，具体实现依赖于所用系列的 HAL 库
    // 初始化 RTC 日期
    RTC_DateTypeDef sDate = {0};
    // 设置当前日期为 2023年10月9日
    sDate.Date = 9;      // 日期
    sDate.Month = RTC_MONTH_OCTOBER; // 月份
    sDate.Year = 23;     // 年份 (2023)
    sDate.WeekDay = RTC_WEEKDAY_MONDAY; // 星期一
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        // 错误处理
    }
}

int main(void) {
    HAL_Init(); // 初始化 HAL 库
    RTC_Init(); // 初始化 RTC

    while (1) {
        // 主循环
    }
}
```

### 3.3 亚秒寄存器（RTC_SSR）

![屏幕截图 2024 10 09 135029](https://img.picgo.net/2024/10/09/-2024-10-09-135029f58e1db06c26569a.png)

该寄存器可用于获取更加精确的 RTC 时间。

#### 1. **寄存器结构**

`RTC_SSR` 寄存器一般包含以下字段：

- **Sub-second value (SS)**: 表示当前秒内的亚秒值，通常是一个 16 位的值，可以表示从 0 到 0xFFFF 的范围。

#### 2. **功能描述**

- **亚秒计数**: `RTC_SSR` 用于记录当前秒内的纳秒、微秒或毫秒部分。
- **定时器精度**: 通过配置 RTC 的分频器，可以将 RTC 的时钟信号分频到更高的精度，以便用户获取更细致的时间信息。

#### 3. **使用方法**

下面是一个简单的代码示例，展示如何读取和设置 RTC 的亚秒寄存器：

```c
#include "stm32f4xx_hal.h"
// 假设已经初始化了 RTC
void Get_Subsecond(void) {
    // 读取当前的亚秒值
    uint32_t subSecond = HAL_RTC_GetSubSecond(&hrtc);
    // 处理亚秒值
    // 例如打印或其他处理
}
void Set_Subsecond(uint32_t ss_value) {
    // 设置亚秒值，注意此操作通常在设置日期和时间后进行
    RTC_TimeTypeDef sTime = {0};
    sTime.Hours = 10; // 时
    sTime.Minutes = 30; // 分
    sTime.Seconds = 15; // 秒
    sTime.SubSeconds = ss_value; // 亚秒值
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        // 错误处理
    }
}

int main(void) {
    HAL_Init(); // 初始化 HAL 库
    RTC_Init(); // 初始化 RTC
    // 读取亚秒值
    Get_Subsecond();
    // 设置新的亚秒值
    Set_Subsecond(500); // 设置为500亚秒
    while (1) {
        // 主循环
    }
}
```

### 3.4 控制寄存器（RTC_CR）

![屏幕截图 2024 10 09 135337](https://img.picgo.net/2024/10/09/-2024-10-09-1353374e59c716787337fd.png)

#### 1. **寄存器结构**

`RTC_CR` 寄存器通常包含以下字段：

- **WUTIE (位 10)**: 唤醒定时器中断使能。
- **ALRAE (位 8)**: ALRA 事件使能。
- **ALRBIE (位 7)**: ALRB 中断使能。
- **TSIE (位 6)**: 时间戳中断使能。
- **TSE (位 5)**: 时间戳使能。
- **ADD1H (位 4)**: 加1小时使能。
- **SUB1H (位 3)**: 减1小时使能。
- **BKP (位 2)**: 后备使能。
- **CNF (位 0)**: 配置模式使能。

#### 2. **功能描述**

- **时钟源选择**: RTC 可以选择不同的时钟源。
- **闹钟和唤醒定时器**: 可以设置闹钟功能和唤醒定时器，使能相关中断。
- **时间戳功能**: 允许捕获特定事件的时间信息。
- **配置模式**: 允许进入配置状态以更改 RTC 设置。

#### 3. **使用方法**

下面是一个代码示例，展示如何配置 RTC 控制寄存器：

```c
#include "stm32f4xx_hal.h"
// 假设已经初始化了 RTC
void RTC_Config(void) {
    // 配置 RTC 控制寄存器
    // 使能唤醒定时器和闹钟中断
    hrtc.Instance->CR |= (RTC_CR_WUTIE | RTC_CR_ALRAE);
    // 其他配置...
}
int main(void) {
    HAL_Init(); // 初始化 HAL 库
    RTC_Init(); // 初始化 RTC
    // 配置 RTC 控制寄存器
    RTC_Config();
    while (1) {
        // 主循环
    }
}
```

该寄存器我们不详细介绍每个位了，重点介绍几个要用到的： WUTIE， ALRAIE 是唤醒定时器中断和闹钟 A 中断使能位，本章要用到，设置为 1 即可。 WUTE 和 ALRAE，则是唤醒定时器和闹钟 A 定时器使能位，同样设置为 1，开启。

FMT 为小时格式选择位，我们设置为 0，选择 24 小时制。最后 WUCKSEL[2:0]，用于唤醒时钟选择。

### 3.5 初始化和状态寄存器（RT_ISR）

![屏幕截图 2024 10 10 083600](https://img.picgo.net/2024/10/10/-2024-10-10-083600f58722d9f96cd2f7.png)

该寄存器中， WUTF、 ALRBF 和 ALRAF，分别是唤醒定时器闹钟 B 和闹钟 A 的中断标志位，当对应事件产生时，这些标志位被置 1，如果设置了中断，则会进入中断服务函数，这些位通过软件写 0 清除； INIT 为初始化模式控制位，要初始化 RTC 时，必须先设置 INIT=1； INITF为初始化标志位，当设置 INIT 为 1 以后，要等待 INITF 为 1，才可以更新时间、日期和预分频寄存器等； RSF 位为寄存器同步标志，仅在该位为 1 时，表示日历影子寄存器已同步，可以正确读取 RTC_TR/RTC_TR 寄存器的值了； WUTWF、 ALRBWF 和 ALRAWF 分别是唤醒定时器、闹钟 B 和闹钟 A 的写标志，只有在这些位为 1 的时候，才可以更新对应的内容，比如：要设置闹钟 A 的 ALRMAR 和 ALRMASSR，则必须先等待 ALRAWF 为 1，才可以设置。

### 3.6 预分频寄存器（RTC_PRER）

![屏幕截图 2024 10 10 083844](https://img.picgo.net/2024/10/10/-2024-10-10-0838447416dc99ab01fcfe.png)

该寄存器的配置，必须在初始化模式（INITF=1）下，才可以进行。

- **PREDIV_S**: 用于设置 RTC 计数器的秒分频器。它的值通常在 0 到 0xFFFF 之间，这个分频器会影响 RTC 的秒计数。

- **PREDIV_A**: 用于设置 RTC 的分频器 A。这个分频器通常用于生成 1Hz 信号，适用于实现闹钟和其他定时功能。

下面是一个示例代码，演示如何配置 `RTC_PRER` 寄存器：

```c
#include "stm32f4xx_hal.h"
// RTC 初始化函数
void RTC_Init(void) {
    RTC_HandleTypeDef hrtc;
    // 假设 hrtc 已经被初始化
    // 使能 RTC 时钟
    __HAL_RCC_RTC_ENABLE();
    // 配置 RTC 时间基准
    // 设置 PREDIV_S 和 PREDIV_A
    hrtc.Instance->PRER = (PREDIV_A_VALUE << 16) | PREDIV_S_VALUE; // 配置预分频
    // 其他 RTC 配置...
}
// 主函数
int main(void) {
    HAL_Init(); // 初始化 HAL 库
    RTC_Init(); // 初始化 RTC
    while (1) {
        // 主循环
    }
}
```

### 3.7 唤醒定时器寄存器（RTC_WUTR）

![屏幕截图 2024 10 10 084237](https://img.picgo.net/2024/10/10/-2024-10-10-0842374ee405ca2b36c697.png)

该寄存器用于设置自动唤醒重装载值，可用于设置唤醒周期。该寄存器的配置，必须等待RTC_ISR 的 WUTWF 为 1 才可以进行。

**WUT**: 这是一个16位的计数器，用于设定唤醒定时器的时间间隔。它的值决定了 RTC 计数到多少时触发唤醒事件。具体的时间间隔依赖于 RTC 的时钟配置（PREDIV_S 和 PREDIV_A）

下面是一个示例代码，演示如何设置 `RTC_WUTR` 寄存器以配置唤醒定时器：

```c
#include "stm32f4xx_hal.h"
// RTC 初始化函数
void RTC_Init(void) {
    RTC_HandleTypeDef hrtc;
    // 假设 hrtc 已经被初始化
    // 使能 RTC 时钟
    __HAL_RCC_RTC_ENABLE();
    // 配置 RTC 的预分频器
    hrtc.Instance->PRER = (PREDIV_A_VALUE << 16) | PREDIV_S_VALUE; // 配置预分频
    // 设置唤醒定时器值
    hrtc.Instance->WUTR = WAKEUP_TIMER_VALUE; // 设置唤醒定时器，值应根据实际需要配置
    // 使能唤醒定时器
    hrtc.Instance->CR |= RTC_CR_WUTEN; // 使能唤醒定时器
}
// 主函数
int main(void) {
    HAL_Init(); // 初始化 HAL 库
    RTC_Init(); // 初始化 RTC
    // 进入待机模式
    HAL_PWR_EnterSTANDBYMode();
    while (1) {
        // 主循环
    }
}
```

### 3.8 闹钟A寄存器（RTC_ALRMAR）

![屏幕截图 2024 10 10 084419](https://img.picgo.net/2024/10/10/-2024-10-10-0844199a0a254134f7a09f.png)

该寄存器用于设置闹铃 A，当 WDSEL 选择 1 时，使用星期制闹铃，本章我们选择星期制闹铃。该寄存器的配置，必须等待 RTC_ISR 的 ALRAWF 为 1 才可以进行。

### 3.9 写保护寄存器（RTC_WPR）

`RTC_WPR` 寄存器的结构如下：

| 位   | 名称  | 描述                   |
| --- | --- | -------------------- |
| 7-0 | WPR | 写保护值，写入特定值以解锁或锁定 RTC |

该寄存器比较简单，低八位有效。上电后，所有 RTC 寄存器都受到写保护（RTC_ISR[13:8]、 RTC_TAFCR 和 RTC_BKPxR 除外），必须依次写入： 0XCA、 0X53 两关键字到 RTC_WPR 寄存器，才可以解锁。 写一个错误的关键字将再次激活 RTC 的寄存器写保护。

以下示例代码展示如何使用 `RTC_WPR` 寄存器来设置写保护：

```c
#include "stm32f4xx_hal.h"
// RTC 写保护初始化函数
void RTC_WriteProtectionConfig(RTC_HandleTypeDef *hrtc, FunctionalState state) {
    if (state == ENABLE) {
        // 使能写保护，写入 0xFF
        hrtc->Instance->WPR = 0xFF;
    } else {
        // 禁用写保护，写入 0xCA 和 0x53
        hrtc->Instance->WPR = 0xCA; // 第一次写入
        hrtc->Instance->WPR = 0x53; // 第二次写入
    }
}
// 主函数示例
int main(void) {
    HAL_Init(); // 初始化 HAL 库
    RTC_HandleTypeDef hrtc;   
    // RTC 初始化代码这里省略
    // 使能写保护
    RTC_WriteProtectionConfig(&hrtc, ENABLE);
    // 如果需要修改 RTC 设置，可以先禁用写保护
    RTC_WriteProtectionConfig(&hrtc, DISABLE);
    // 修改 RTC 设置
    RTC_WriteProtectionConfig(&hrtc, ENABLE); // 重新使能写保护
    while (1) {
        // 主循环
    }
}
```

### 3.10 备份寄存器（RTC_BKPxR）

STM32F407 的 RTC 提供多达 20 个备份寄存器，分别命名为 `RTC_BKP0R` 到 `RTC_BKP19R`。每个寄存器的结构如下：

| 寄存器名称        | 描述       |
| ------------ | -------- |
| `RTC_BKP0R`  | 备份寄存器 0  |
| `RTC_BKP1R`  | 备份寄存器 1  |
| ...          | ...      |
| `RTC_BKP19R` | 备份寄存器 19 |

该寄存器组总共有 20 个，每个寄存器是 32 位的，可以存储 80 个字节的用户数据， 这些寄存器在备份域中实现，可在 VDD 电源关闭时通过 VBAT 保持上电状态。备份寄存器不会在系统复位或电源复位时复位，也不会在MCU 从待机模式唤醒时复位。

复位后，对 RTC 和 RTC 备份寄存器的写访问被禁止， 执行以下操作可以使能对 RTC 及 RTC 备份寄存器的写访问：

- 通过设置寄存器 RCC_APB1ENR 的 PWREN 位来打开电源接口时钟

- 电源控制寄存器(PWR_CR)的 DBP 位来使能对 RTC 及 RTC 备份寄存器的访问。

以下是如何使用 RTC 备份寄存器的示例代码：

```c
#include "stm32f4xx_hal.h"
// 写入备份寄存器示例
void WriteToBackupRegister(RTC_HandleTypeDef *hrtc, uint32_t backupRegIndex, uint32_t data) {
    switch (backupRegIndex) {
        case 0:
            hrtc->Instance->BKP0R = data;
            break;
        case 1:
            hrtc->Instance->BKP1R = data;
            break;
        // 继续为其他寄存器添加 case ...
        default:
            // 处理无效的寄存器索引
            break;
    }
}
// 读取备份寄存器示例
uint32_t ReadFromBackupRegister(RTC_HandleTypeDef *hrtc, uint32_t backupRegIndex) {
    switch (backupRegIndex) {
        case 0:
            return hrtc->Instance->BKP0R;
        case 1:
            return hrtc->Instance->BKP1R;
        // 继续为其他寄存器添加 case ...
        default:
            return 0; // 或者处理无效的寄存器索引
    }
}
// 主函数示例
int main(void) {
    HAL_Init(); // 初始化 HAL 库
    RTC_HandleTypeDef hrtc;
    // RTC 初始化代码省略
    // 写入备份寄存器
    WriteToBackupRegister(&hrtc, 0, 0x12345678);
    // 读取备份寄存器
    uint32_t data = ReadFromBackupRegister(&hrtc, 0);
    while (1) {
        // 主循环
    }
}
```

我们可以用 BKP 来存储一些重要的数据，相当于一个 EEPROM，不过这个 EEPROM 并不是真正的 EEPROM，而是需要电池来维持它的数据。

### 3.11 备份区域控制寄存器（RCC_BDCR）

![屏幕截图 2024 10 10 085205](https://img.picgo.net/2024/10/10/-2024-10-10-08520583ffa16d9ce621d1.png)

RTC 的时钟源选择及使能设置都是通过这个寄存器来实现的，所以我们在 RTC 操作之前先要通过这个寄存器选择 RTC 的时钟源，然后才能开始其他的操作。

## 4. 配置RTC基本步骤

### 4.1 使能电源时钟，并使能RTC及RTC后备寄存器写访问

前面已经介绍了，我们要访问 RTC 和 RTC 备份区域就必须先使能电源时钟，然后使能 RTC即后备区域访问。电源时钟使能， 通过 RCC_APB1ENR 寄存器来设置； RTC 及 RTC 备份寄存器的写访问，通过 PWR_CR 寄存器的 DBP 位设置。 HAL 库设置方法为：

```c
__HAL_RCC_PWR_CLK_ENABLE(); // 使能电源时钟 PWR
HAL_PWR_EnableBkUpAccess(); // 取消备份区域写保护
```

### 4.2 开启外部低速振荡器LSE，选择RTC时钟，并使能

配置开启 LSE 的函数为 HAL_RCC_OscConfig，使用方法为：

```c
RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE; // LSE 配置
RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
RCC_OscInitStruct.LSEState = RCC_LSE_ON; // RTC 使用 LSE
HAL_RCC_OscConfig(&RCC_OscInitStruct);
```

选择 RTC 时钟源为函数为 HAL_RCCEx_PeriphCLKConfig，使用方法为：

```c
PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC; // 外设为 RTC
PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; // RTC 时钟源为 LSE
HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
```

使能 RTC 时钟方法为：

```c
__HAL_RCC_RTC_ENABLE(); // RTC 时钟使能
```

### 4.3 初始化RTC，设置RTC的分频，以及配置RTC参数

在 HAL 中，初始化 RTC 是通过函数 HAL_RTC_Init 实现的，该函数声明为：

```c
HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *hrtc);
```

同样按照以前的方式，我们来看看 RTC 初始化参数结构体 RTC_HandleTypeDef 定义：

```c
typedef struct
{
    RTC_TypeDef *Instance;
    RTC_InitTypeDef Init;
    HAL_LockTypeDef Lock;
    __IO HAL_RTCStateTypeDef State;
}RTC_HandleTypeDef;
```

这里我们着重讲解成员变量 Init 含义，因为它是真正的 RTC 初始化变量，它是RTC_InitTypeDef 结构体类型，结构体 RTC_InitTypeDef 定义为：

```c
typedef struct
{
    uint32_t HourFormat; // 小时格式
    uint32_t AsynchPrediv; // 异步预分频系数
    uint32_t SynchPrediv; // 同步预分频系数
    uint32_t OutPut; // 选择连接到 RTC_ALARM 输出的标志
    uint32_t OutPutPolarity; // 设置 RTC_ALARM 的输出极性
    uint32_t OutPutType; // 设置 RTC_ALARM 的输出类型为开漏输出还是推挽输出
}RTC_InitTypeDef;
```

- HourFormat 用来设置小时格式，为 12 小时制或者 24 小时制，取值为RTC_HOURFORMAT_12 或者 RTC_HOURFORMAT_24。

- AsynchPrediv 用来设置 RTC 的异步预分频系数，也就是设置 RTC_PRER 寄存器的PREDIV_A 相关位，因为异步预分频系数是 7 位，所以最大值为 0x7F，不能超过这个值

- SynchPrediv用来设置RTC的同步预分频系数，也就是设置RTC_PRER寄存器的PREDIV_S相关位，因为同步预分频系数也是 15 位，所以最大值为 0x7FFF，不能超过这个值

- OutPut 用来选择要连接到 RTC_ALARM 输出的标志，取值为： RTC_OUTPUT_DISABLE （禁止输出）， RTC_OUTPUT_ALARMA（使能闹钟 A 输出）， RTC_OUTPUT_ALARMB（使能闹钟 B 输出）和 RTC_OUTPUT_WAKEUP（使能唤醒输出）。

- OutPutPolarity 用来设置 RTC_ALARM 的输出极性，与 Output 成员变量配合使用，取值为RTC_OUTPUT_POLARITY_HIGH（高电平）或 RTC_OUTPUT_POLARITY_LOW（低电平）。

- OutPutType 用来设置 RTC_ALARM 的输出类型为开漏输（RTC_OUTPUT_TYPE_OPENDRAIN）还是推挽输出（RTC_OUTPUT_TYPE_PUSHPULL），与成员变量 OutPut 和 OutPutPolarity 配合使用。

接下来我们看看 RTC 初始化的一般格式： 

```c
RTC_Handler.Instance = RTC;
RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24; // RTC 设置为 24 小时格式
RTC_Handler.Init.AsynchPrediv = 0X7F; // RTC 异步分频系数(1~0X7F)
RTC_Handler.Init.SynchPrediv = 0XFF;  // RTC 同步分频系数(0~7FFF)
RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;
RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
HAL_RTC_Init(&RTC_Handler);
```

同样， HAL 库也提供了 RTC 初始化 MSP 函数。函数声明为：

```c
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc);
```

该函数内部一般存放时钟使能，时钟源选择等操作程序。

### 4.4 设置RTC时间

HAL 库中，设置 RTC 时间的函数为：

```c
HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc,
                                  RTC_TimeTypeDef *sTime, uint32_t Format);
```

实际上，根据我们前面寄存器的讲解， RTC_SetTime 函数是用来设置时间寄存器 RTC_TR的相关位的值。

RTC_SetTime 函数的第三个参数 Format,用来设置输入的时间格式为 BIN 格式还是 BCD 格式，可选值为 RTC_FORMAT_BIN 和 RTC_FORMAT_BCD。

我们接下来看看第二个初始化参数结构体 RTC_TimeTypeDef 的定义：

```c
typedef struct
{    
    uint8_t Hours;
    uint8_t Minutes;
    uint8_t Seconds;
    uint8_t TimeFormat;
    uint32_t SubSeconds;
    uint32_t SecondFraction;
    uint32_t DayLightSaving;
    uint32_t StoreOperation;
}RTC_TimeTypeDef;
```

- **Hours**: 设置小时，范围为 0-23 或 1-12（根据时间格式）。
- **Minutes**: 设置分钟，范围为 0-59。
- **Seconds**: 设置秒，范围为 0-59。
- **TimeFormat**: 指定时间显示格式，12 小时制或24 小时制。
- **DayLightSaving**: 设置是否使用夏令时，通常有三种状态：启用（`RTC_DAYLIGHTSAVING_ENABLE`）、禁用（`RTC_DAYLIGHTSAVING_NONE`）、自动（`RTC_DAYLIGHTSAVING_AUTOMATIC`）。
- **StoreOperation**: 设置存储操作，通常有重置（`RTC_STOREOPERATION_RESET`）和存储（`RTC_STOREOPERATION_SET`）两种。
- SubSeconds 用来读取保存亚秒寄存器 RTC_SSR 的值， SecondFraction 用来读取保存同步预分频系数 的值，也就是 RTC_PRER的位 0~14

HAL_RTC_SetTime 函数参考实例如下：

```c
// 创建一个 RTC_TimeTypeDef 结构体实例，用于存储时间信息
RTC_TimeTypeDef RTC_TimeStructure;
// 设置小时、分钟和秒
RTC_TimeStructure.Hours = 1;      // 设置小时为 1
RTC_TimeStructure.Minutes = 1;    // 设置分钟为 1
RTC_TimeStructure.Seconds = 1;    // 设置秒为 1
// 设置时间格式为 12 小时制，PM 表示下午
RTC_TimeStructure.TimeFormat = RTC_HOURFORMAT12_PM; 
// 设置夏令时选项，这里选择不使用夏令时
RTC_TimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE; 
// 设置存储操作，选择重置存储操作
RTC_TimeStructure.StoreOperation = RTC_STOREOPERATION_RESET; 
// 调用 HAL 库函数设置 RTC 时间
// 第一个参数为 RTC 句柄，指向已初始化的 RTC_HandleTypeDef 结构
// 第二个参数为 RTC_TimeTypeDef 结构体，包含要设置的时间信息
// 第三个参数指定时间格式，此处使用二进制格式（RTC_FORMAT_BIN）
HAL_RTC_SetTime(&RTC_Handler, &RTC_TimeStructure, RTC_FORMAT_BIN);
```

### 4.5 设置RTC的日期

设置 RTC 的日期函数为：

```c
HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc,
                                  RTC_DateTypeDef *sDate, uint32_t Format);
```

实际上，根据我们前面寄存器的讲解， HAL_RTC_SetDate 设置日期函数是用来设置日期寄存器 RTC_DR 的相关位的值。

该函数有三个入口参数，我们着重讲解第二个入口参数 sData，它是结构体 RTC_DateTypeDef 指针类型变量，结构体 RTC_DateTypeDef 定义如下：

```c
typedef struct
{
    uint8_t WeekDay;// 星期几
    uint8_t Month;  // 月份
    uint8_t Date;   // 日期
    uint8_t Year;   // 年份
}RTC_DateTypeDef;
```

### 4.7 获取RTC当前日期和时间

获取当前 RTC 时间的函数为：

```c
HAL_StatusTypeDef HAL_RTC_GetTime(RTC_HandleTypeDef *hrtc,
                                  RTC_TimeTypeDef *sTime, uint32_t Format);
```

获取当前 RTC 日期的函数为：

```c
HAL_StatusTypeDef HAL_RTC_GetDate(RTC_HandleTypeDef *hrtc,
                                  RTC_DateTypeDef *sDate, uint32_t Format);
```

这两个函数非常简单，实际就是读取RTC_TR寄存器和RTC_DR寄存器的时间和日期的值，然后将值存放到相应的结构体中。
