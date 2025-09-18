# 第四十六章 RTC-万年历

## 1. 导入

RTC外设是个连续计数的计数器，利用它提供的时间戳，可通过程序转换输出实时时钟和日历的功能， 修改计数器的值则可以重新设置系统当前的时间和日期。由于它的时钟配置系统(RCC_BDCR寄存器)是在备份域，在系统复位或从待机模式唤醒后RTC的设置维持不变， 而且使用备份域电源可以使RTC计时器在主电源关掉的情况下仍然运行，保证时间的正确。

## 2. 硬件设计

开发板中与RTC相关的硬件设计见图

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/RTC004.jpg" title="" alt="" width="511">

原理图中的右上角是备份域的供电电路，在本开发板中提供了一个钮扣电池插槽，可以接入型号为CR1220的钮扣电池，该型号的钮扣电池电压为3.2V， 图中的BAT54C双向二极管可切换输入到STM32备份域电源引脚VBAT的供电，当主电源正常供电时，由稳压器输出的3.3V供电，当主电源掉电时，由钮扣电池供电 。

原理图下方的是本开发板采用的LSE晶振电路，此处使用的晶振频率为32.768KHz，RTC外设可以使用LSE作为时钟，把它进行分频得到1Hz 的RTC计时时钟。

本实验默认使用LSI内部时钟，使用内部时钟时，即使安装了钮扣电池，主电源掉电后时间是不会继续走的，只会保留上次断电的时间。 若要持续运行，需要修改brtc.h文件，使用RTC_CLOCK_SOURCE_LSE宏，切换成使用LSE外部时钟。

## 3. 软件设计

### 3.1 编程目标

1. 初始化RTC外设

2. 设置时间以及添加配置标志

3. 获取当前时间

4. 主函数测试

### 3.2 代码分析

#### 3.2.1 RTC宏定义设置

```c
// 是否使用LCD显示日期
#define USE_LCD_DISPLAY
// 使用 LSE 外部时钟或 LSI 内部时钟
// #define RTC_CLOCK_SOURCE_LSE
#define RTC_CLOCK_SOURCE_LSI
#define RTC_BKP_DRX BKP_DR1
// 写入到备份寄存器的数据宏定义
#define RTC_BKP_DATA 0xA5A5
//北京时间的时区秒数差
#define TIME_ZOOM    8*60*60)
```

以上代码定义的宏介绍如下：

- USE_LCD_DISPLAY：这个宏可以用于切换本工程是否使用液晶屏显示时间，把它注释掉可以关闭液晶显示，方便移植到没有液晶的应用中。

- RTC_CLOCK_SOURCE_LSE/LSI：这两个宏用于选择使用LSE作外部时钟还是LSI作外部时钟。 提供两种选择主要是因为STM32的LSE晶振在批量产品时容易不起振， 而LSI则在主电源关闭后计时时间不会继续增加。

- RTC_BKP_DRX和RTC_BKP_DATA：这两个宏用于在备份域寄存器设置RTC已配置标志，本实验中初始化RTC后，向备份域寄存器写入一个数字， 若下次芯片上电检测到该标志，说明RTC之前已经配置好时间，所以不应该再设置RTC，而如果备份域电源也掉电，备份域内记录的该标志也会丢失， 所以芯片上电后需要重新设置时间。这两个宏的值中，BKP_DR1是备份域的其中一个寄存器，而0xA5A5则是随意选择的数字，只要写入和检测一致即可。

- TIME_ZOOM：这个宏用于设置时区的秒数偏移，例如北京时间为(GMT+8) 时区，即相对于格林威治时间(GMT) 早8个小时， 此处使用的宏值即为8个小时的秒数（8*60*60），若使用其它时区，修改该宏即可。

关于这些宏的作用，在后面的C源代码中都会有体现。

#### 3.2.2 初始化RTC

```c
void RTC_Configuration(void)
{
    // 使能 PWR 和 Backup 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    // 允许访问 Backup 区域 
    PWR_BackupAccessCmd(ENABLE);
    // 复位 Backup 区域
    BKP_DeInit();

// 使用外部时钟还是内部时钟（在bsp_rtc.h文件定义）
// 使用外部时钟时，在有些情况下晶振不起振
// 批量产品的时候，很容易出现外部晶振不起振的情况，不太可靠
#ifdef  RTC_CLOCK_SOURCE_LSE
    // 使能 LSE
    RCC_LSEConfig(RCC_LSE_ON);
    // 等待 LSE 准备好
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
    }
    // 选择 LSE 作为 RTC 时钟源 
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    // 使能 RTC 时钟
    RCC_RTCCLKCmd(ENABLE);
    // 等待 RTC 寄存器 同步
    // 因为RTC时钟是低速的，内环时钟是高速的，所以要同步
    RTC_WaitForSynchro();
    // 确保上一次 RTC 的操作完成
    RTC_WaitForLastTask();
    // 使能 RTC 秒中断 
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    // 确保上一次 RTC 的操作完成
    RTC_WaitForLastTask();
    // 设置 RTC 分频: 使 RTC 周期为1s  
    // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) = 1HZ */
    RTC_SetPrescaler(32767);
    // 确保上一次 RTC 的操作完成 
    RTC_WaitForLastTask();
#else
    // 使能 LSI 
    RCC_LSICmd(ENABLE);
    // 等待 LSI 准备好
    while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
    }
    // 选择 LSI 作为 RTC 时钟源 
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    // 使能 RTC 时钟 
    RCC_RTCCLKCmd(ENABLE);
    // 等待 RTC 寄存器 同步
    // 因为RTC时钟是低速的，内环时钟是高速的，所以要同步
    RTC_WaitForSynchro();
    // 确保上一次 RTC 的操作完成 
    RTC_WaitForLastTask();
    // 使能 RTC 秒中断 
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    // 确保上一次 RTC 的操作完成 
    RTC_WaitForLastTask();
    // 设置 RTC 分频: 使 RTC 周期为1s ,LSI约为40KHz 
    // RTC period = RTCCLK/RTC_PR = (40 KHz)/(40000-1+1) = 1HZ 
    RTC_SetPrescaler(40000-1);
    // 确保上一次 RTC 的操作完成
    RTC_WaitForLastTask();
#endif

}
```

在这个初始化函数里，分成了使用LSE和LSI的初始化配置，这两种配置的初始化过程类似，都直接调用了前面介绍的各种RTC相关的库函数

这个初始化的流程如下：使用RCC_APB1PeriphClockCmd使能PWR和BKP区域（即备份域）的时钟系统，使用PWR_BackupAccessCmd设置允许对BKP区域的访问，使能LSE时钟或LSI时钟，选择LSE或LSI作为RTC的时钟源并使能RTC时钟，利用库函数RTC_WaitFor Synchro对备份域和APB进行同步，用RTC_ITConfig使能秒中断，使用RTC_SetPrescaler分频配置把RTC时钟频率设置为1Hz。那么RTC每个时钟周期都会产生一次中断对RTC的每一个初始化参数都是使用相应的库函数来配置的。

经过这样的配置后，RTC每秒产生一次中断事件，实验中在中断设置标志位以便更新时间。

---

当然了，出现了新的库函数，我们还是要介绍一下：

1. `RCC_LSICmd(ENABLE)`

- **功能**：使能低速内部振荡器（LSI）。
- **参数**：`ENABLE`表示开启LSI；`DISABLE`表示关闭LSI。
- **作用**：启动LSI振荡器，使其可以用作RTC时钟源。

2. `Rcc_GetFlagStatus(uint32_t RCC_FLAG)`

- **功能**：检查指定的RCC标志位状态。
- **参数**：`RCC_FLAG`是要检查的标志，如`RCC_FLAG_LSIRDY`表示LSI是否准备就绪。
- **返回值**：返回标志位的状态，`SET`表示标志位被置位，`RESET`表示未被置位。
- **作用**：在启用LSI后，通过此函数检查LSI是否已准备好，确保后续操作的稳定性。

3. `RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource)`

- **功能**：选择RTC时钟源。
- **参数**：`RCC_RTCCLKSource_LSI` 或 `RCC_RTCCLKSource_LSE`等，选择LSI或外部低速晶振（LSE）作为RTC的时钟源。
- **作用**：设定RTC使用LSI作为时钟源，以便进行时间计量。

4. `RCC_RTCCLKCmd(ENABLE)`

- **功能**：使能或禁用RTC时钟。
- **参数**：`ENABLE`表示启用RTC时钟；`DISABLE`表示禁用。
- **作用**：启用RTC模块，开始其时钟工作。

5. `RTC_WaitForSynchro()`

- **功能**：等待RTC寄存器同步。
- **作用**：确保RTC的设置在进行下一次操作之前已经完成，防止因操作不当导致数据不一致。

6. `RTC_WaitForLastTask()`

- **功能**：等待上一次RTC操作完成。
- **作用**：确保当前任务之前的所有RTC操作都已完成，这对于避免数据竞争和状态错误是必要的。

7. `RTC_ITConfig(FunctionalState NewState, uint32_t RTC_IT)`

- **功能**：配置RTC中断。
- **参数**：`RTC_IT`指定要使能的中断类型，例如`RTC_IT_SEC`表示秒中断；`NewState`为`ENABLE`或`DISABLE`。
- **作用**：启用RTC秒中断，允许在每秒产生一次中断请求，这对于需要定时操作的应用非常重要。

8. `RTC_SetPrescaler(uint32_t Prescaler)`

- **功能**：设置RTC的分频器。
- **参数**：`Prescaler`是RTC的分频值。根据选定的时钟源，设定分频器以实现所需的RTC周期。
- **作用**：通过配置分频器来设置RTC的更新频率，例如将LSI的频率从40kHz分频到1Hz，以便每秒更新一次时间。

---

#### 3.2.3 时间管理结构

RTC初始化完成后可以直接往它的计数器写入时间戳，但是时间戳对用户不友好，不方便配置和显示时间， 在本工程中，使用bsp_date.h文件的rtc_time结构体来管理时间

```c
struct rtc_time {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
};
```

这个类型的结构体具有时、分、秒、日、月、年及星期这7个成员。当需要给RTC的计时器重新配置或显示时间时，使用这种容易接受的时间表示方式。

在配置RTC时，使用这种类型的变量保存用户输入的时间，然后利用函数由该时间求出对应的UNIX时间戳，写入RTC的计数器；RTC正常运行后， 需要输出时间时，利用函数通过RTC的计数器获取UNIX时间戳，转化成这种友好的时间表示方式保存到变量输出。

其实在C语言标准库ANSI C中，也具有类似这样的结构体struct tm ，位于标准的time.h文件中， 而具有以上功能的转化函数则为mktime和localtime，它们分别把tm格式的时间转化成时间戳和用时间戳转化成tm格式。 而在本实验中直接使用了开源的万年历算法源码，便于修改和学习。

#### 3.2.4 时间格式转换

在本实验中，tm格式转时间戳使用mktimev函数，时间戳转tm格式使用to_tm函数，这两个函数都定义在bsp_date.c文件中

```c
u32 mktimev(struct rtc_time *tm)
{
    if (0 >= (int) (tm->tm_mon -= 2)) { /* 1..12 -> 11,12,1..10 */
        tm->tm_mon += 12;   /* Puts Feb last since it has leap day */
        tm->tm_year -= 1;
    }

    return (((
            (u32) (tm->tm_year/4 - tm->tm_year/100 + tm->tm_year/400 +
            367*tm->tm_mon/12 + tm->tm_mday) + tm->tm_year*365 - 719499
            )*24 + tm->tm_hour /* now have hours */
            )*60 + tm->tm_min /* now have minutes */
        )*60 + tm->tm_sec; /* finally seconds */
}



void to_tm(u32 tim, struct rtc_time * tm)
{
    register u32    i;
    register long   hms, day;

    day = tim / SECDAY;     /* 有多少天 */
    hms = tim % SECDAY;     /* 今天的时间，单位s */

    /* Hours, minutes, seconds are easy */
    tm->tm_hour = hms / 3600;
    tm->tm_min = (hms % 3600) / 60;
    tm->tm_sec = (hms % 3600) % 60;

    /* Number of years in days */ /*算出当前年份，起始的计数年份为1970年*/
    for (i = STARTOFTIME; day >= days_in_year(i); i++) {
        day -= days_in_year(i);
    }
    tm->tm_year = i;

    /* Number of months in days left */ /*计算当前的月份*/
    if (leapyear(tm->tm_year)) {
        days_in_month(FEBRUARY) = 29;
    }
    for (i = 1; day >= days_in_month(i); i++) {
        day -= days_in_month(i);
    }
    days_in_month(FEBRUARY) = 28;
    tm->tm_mon = i;

    /* Days are what is left over (+1) from all that. *//*计算当前日期*/
    tm->tm_mday = day + 1;

    /*
    * Determine the day of week
    */
    GregorianDay(tm);
}
```

关于日期计算的细节此处不作详细分析，其原理是以1970年1月1日0时0分0秒为计时基点，对日期和以秒数表示时间戳进行互相转化，转化重点在于闰年的计算。

这两个函数都是以格林威治时间(GMT)时区来计算的，在调用这些函数时我们会对输入参数加入时区偏移的运算，进行调整。

#### 3.2.5 配置时间

```c
void Time_Adjust(struct rtc_time *tm)
{

    // RTC 配置 
    RTC_Configuration();
    // 等待确保上一次操作完成 
    RTC_WaitForLastTask();
    // 由日期计算时间戳并写入到RTC计数寄存器 
    RTC_SetCounter(mktimev(tm)-TIME_ZOOM);
    // 等待确保上一次操作完成 
    RTC_WaitForLastTask();
}
```

Time_Adjust函数用于配置时间，它先调用前面的RTC_Configuration初始化RTC，接着调用库函数RTC_SetCounter向RTC计数器写入要设置时间的时间戳值， 而时间戳的值则使用mktimev函数通过输入参数tm来计算，计算后还与宏TIME_ZOOM运算，计算时区偏移值。此处的输入参数tm是北京时间， 所以“mktimev(tm)-TIME_ZOOM”计算后写入到RTC计数器的是格林威治时区的标准UNIX时间戳。

#### 3.2.6 检查并配置RTC

上面的Time_Adjust函数直接把参数写入到RTC中修改配置，但在芯片每次上电时，并不希望每次都修改系统时间， 所以我们增加了RTC_CheckAndConfig函数用于检查是否需要向RTC写入新的配置，见。

```c
void RTC_CheckAndConfig(struct rtc_time *tm)
{
    /*在启动时检查备份寄存器BKP_DR1，如果内容不是0xA5A5,
    则需重新配置时间并询问用户调整时间*/
    if (BKP_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA) {
        printf("\r\n\r\n RTC not yet configured....");
        printf("\r\n\r\n RTC configured....");
        // 使用tm的时间配置RTC寄存器 
        Time_Adjust(tm);
        //向BKP_DR1寄存器写入标志，说明RTC已在运行
        BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
    } else {
        // 使能 PWR 和 Backup 时钟 
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
        // 允许访问 Backup 区域 
        PWR_BackupAccessCmd(ENABLE);
        /*LSE启动无需设置新时钟*/
#ifdef RTC_CLOCK_SOURCE_LSI
        // 使能 LSI 
        RCC_LSICmd(ENABLE);
        // 等待 LSI 准备好 
        while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
        }
#endif
        // 检查是否掉电重启
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {
            printf("\r\n\r\n Power On Reset occurred....");
        }
        // 检查是否Reset复位
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {
            printf("\r\n\r\n External Reset occurred....");
        }
        printf("\r\n No need to configure RTC....");
        // 等待寄存器同步
        RTC_WaitForSynchro();
        // 允许RTC秒中断
        RTC_ITConfig(RTC_IT_SEC|RTC_IT_ALR, ENABLE);
        // 等待上次RTC寄存器写操作完成
        RTC_WaitForLastTask();
    }
    /*定义了时钟输出宏，则配置校正时钟输出到PC13*/
#ifdef RTCClockOutput_Enable
    // 禁止 Tamper 引脚 
    // 要输出 RTCCLK/64 到 Tamper 引脚,  tamper 功能必须禁止 
    BKP_TamperPinCmd(DISABLE);
    // 使能 RTC 时钟输出到 Tamper 引脚 
    BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
#endif
    // 清除复位标志 flags 
    RCC_ClearFlag();
}
```

在本函数中，会检测备份域寄存器RTC_BKP_DRX内的值是否等于RTC_BKP_DATA而分成两个分支。

若不等，说明之前没有配置RTC所以直接调用Time_Adjust函数初始化RTC并写入时间戳进行计时，配置完成后向备份域寄存器RTC_BKP_DRX写入值RTC_BKP_DATA作为标志， 这样该标志就可以指示RTC的配置情况了，因为备份域不掉电时，RTC和该寄存器的值都会保存完好，而如果备份域掉电，那么RTC配置和该标志都会一同丢失；

若本函数的标志判断相等，进入else分支，不再调用Time_Adjust函数初始化RTC，而只是使用RTC_WaitForSynchro和RTC_ITConfig同步RTC域和APB以及使能中断， 以便获取时间。如果使用的是LSI时钟，还需要使能LSI时钟，RTC才会正常运行，这是因为当主电源掉电和备份域的情况下，LSI会关闭，而LSE则会正常运行，驱动RTC计时。

#### 3.2.7 转换并输出时间

RTC正常运行后，可以使用Time_Display函数转换时间格式并输出到串口及液晶屏

```c
void Time_Display(uint32_t TimeVar,struct rtc_time *tm)
{
    static uint32_t FirstDisplay = 1;
    uint32_t BJ_TimeVar;
    uint8_t str[200]; // 字符串暂存
    // 把标准时间转换为北京时间
    BJ_TimeVar = TimeVar + TIME_ZOOM;
    to_tm(BJ_TimeVar, tm); // 把定时器的值转换为北京时间

    if ((!tm->tm_hour && !tm->tm_min && !tm->tm_sec)  || (FirstDisplay)) {
        GetChinaCalendar((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str);
        printf("\r\n 今天新历：%0.2d%0.2d,%0.2d,%0.2d", str[0], str[1], str[2],  str[3]);

        GetChinaCalendarStr((u16)tm->tm_year,(u8)tm->tm_mon,(u8)tm->tm_mday,str);
        printf("\r\n 今天农历：%s\r\n", str);

    if (GetJieQiStr((u16)tm->tm_year, (u8)tm->tm_mon, (u8)tm->tm_mday, str))
            printf("\r\n 今天农历：%s\r\n", str);

        FirstDisplay = 0;
    }

    // 输出时间戳，公历时间 
    rintf(" UNIX时间戳 = %d 当前时间为: %d年(%s年) %d月 %d日 (星期%s)  %0.2d:%0.2d:%0.2d\r",TimeVar,
    tm->tm_year, zodiac_sign[(tm->tm_year-3)%12], tm->tm_mon, tm->tm_mday,
        WEEK_STR[tm->tm_wday], tm->tm_hour,
        tm->tm_min, tm->tm_sec);

#ifdef  USE_LCD_DISPLAY
    // 时间戳
    sprintf((char *)str," UNIX TimeStamp = %d ",TimeVar);
    ILI9341_DispStringLine_EN(LINE(3),(char*)str);
    // 日期
    sprintf((char *)str," Date: %d-%d-%d       ",
            tm->tm_year,
            tm->tm_mon,
            tm->tm_mday);
    ILI9341_DispStringLine_EN(LINE(5),(char*)str);
    // 生肖
    sprintf((char *)str," Chinese %s year      ",en_zodiac_sign[(tm->tm_year-3)%12]);

    ILI9341_DispStringLine_EN(LINE(6),(char*)str);
    // 星期
    sprintf((char *)str," %s                  ",en_WEEK_STR[tm->tm_wday]);
    ILI9341_DispStringLine_EN(LINE(7),(char*)str);
    // 时间
    sprintf((char *)str," Time:  %0.2d:%0.2d:%0.2d",
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec);
    ILI9341_DispStringLine_EN(LINE(8),(char*)str);
#endif
}
```

主要是使用to_tm把时间戳转换成日常生活中使用的时间格式，to_tm以BJ_TimeVar作为输入参数， 而BJ_TimeVar对时间戳变量Time_Var进行了时区偏移，也就是说调用Time_Display函数时，以RTC计数器的值作为TimeVar作为输入参数即可， 最终会输出北京时间。

利用to_tm转换格式后，调用bsp_calendar.c文件中的日历计算函数，求出星期、农历、生肖等内容，然后使用串口和液晶屏显示出来。

#### 3.2.8 中断服务函数

一般来说，上面的Time_Display时间显示每秒中更新一次，而根据前面的配置，RTC每秒会进入一次中断

```c
void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
        // Clear the RTC Second interrupt 
        RTC_ClearITPendingBit(RTC_IT_SEC);

        /* Enable time update */
        TimeDisplay = 1;

    /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }
}
```

RTC的秒中断服务函数只是简单地对全局变量TimeDisplay置1，在main函数的while循环中会检测这个标志，当标志为1时， 就调用Time_Display函数显示一次时间，达到每秒钟更新当前时间的效果。

#### 3.2.9 主函数

```c
/*时间结构体，默认时间2000-01-01 00:00:00*/
struct rtc_time systmtime= {
    0,0,0,1,1,2000,0
};

extern __IO uint32_t TimeDisplay ;
int main()
{
// 可使用该宏设置是否使用液晶显示
#ifdef  USE_LCD_DISPLAY
    ILI9341_Init(); // LCD 初始化
    LCD_SetFont(&Font8x16);
    LCD_SetColors(RED,BLACK);
    ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
    ILI9341_DispStringLine_EN(LINE(0),"        BH RTC demo");
#endif
    USART_Config();
    Key_GPIO_Config();
    // 配置RTC秒中断优先级
    RTC_NVIC_Config();
    RTC_CheckAndConfig(&systmtime);
    while (1) {
        // 每过1s 更新一次时间
        if (TimeDisplay == 1) {
            // 当前时间 
            Time_Display(RTC_GetCounter(),&systmtime);
            TimeDisplay = 0;
        }
        // 按下按键，通过串口修改时间
        if ( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  ) {
            struct rtc_time set_time;
            // 使用串口接收设置的时间，输入数字时注意末尾要加回车
            Time_Regulate_Get(&set_time);
            // 用接收到的时间设置RTC*/
            Time_Adjust(&set_time);
            // 向备份寄存器写入标志
            BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
        }
    }
}
```

main函数的流程非常清晰，初始化了液晶、串口等外设后，调用RTC_CheckAndConfig函数初始化RTC，若RTC是第一次初始化，就使用变量systmtime中的默认时间配置， 若之前已配置好RTC，那么RTC_CheckAndConfig函数仅同步时钟系统，便于获取实时时间。在while循环里检查中断设置的TimeDisplay是否置1， 若置1了就调用Time_Display函数，它的输入参数是库函数RTC_GetCounter的返回值，也就是RTC计数器里的时间戳， Time_Display函数把该时间戳转化成北京时间显示到串口和液晶上。

#### 3.2.10 使用串口配置时间

在main函数的44-54行，是一个按键检测分支，当检测到开发板上的KEY1被按下时，会调用Time_Regulate_Get函数通过串口获取配置时间， 然后把获取得的时间输入到Time_Adjust函数把该时间写入到RTC计数器中，更新配置

```c
/*
* 函数名：Time_Regulate_Get
* 描述  ：保存用户使用串口设置的时间，
*         以便后面转化成时间戳存储到RTC 计数寄存器中。
* 输入  ：用于读取RTC时间的结构体指针
* 注意  ：在串口调试助手输入时，输入完数字要加回车
*/
void Time_Regulate_Get(struct rtc_time *tm)
{
    uint32_t temp_num = 0;
    uint8_t day_max=0 ;
    printf("\r\n=========================设置时间==================");
    do {
        printf("\r\n  请输入年份(Please Set Years),
            范围[1970~2038]，输入字符后请加回车:");
        scanf("%d",&temp_num);
        if (temp_num <1970 || temp_num >2038) {
            printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
        } else {
            printf("\n\r  年份被设置为: %d\n\r", temp_num);
            tm->tm_year = temp_num;
            break;
        }
    } while (1);

    do {
        printf("\r\n  请输入月份(Please Set Months):范围[1~12]，输入字符后请加回车:");
        scanf("%d",&temp_num);
        if (temp_num <1 || temp_num >12) {
            printf("\r\n 您输入的数字是:%d，不符合要求",temp_num);
        } else {
            printf("\n\r  月份被设置为: %d\n\r", temp_num);
            tm->tm_mon = temp_num;
            break;
        }
    } while (1);
    /*...以下省略日期、时间获取的代码*/
}
```

Time_Regulate_Get函数的本质是利用重定向到串口的C标准数据流输入函数scanf获取用户输入，若获取得的数据符合范围， 则赋值到tm结构体中，在main函数中再调用Time_Adjust函数把tm存储的时间写入到RTC计数器中。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/RTC005.jpg)

## 4. 小结

实际上使用RTC设置万年历还算比较简单，单色我们结合了LCD，所以代码有点多，下面我们简单回顾一下整个流程（使用串口）

### 4.1 实验目的

1. 学习如何使用RTC设置万年历。
2. 通过串口接收输入，能够动态修改RTC的时间和日期。

### 4.2 硬件准备

- STM32开发板（如STM32F4系列）
- 串口调试工具（如Tera Term或PuTTY）

### 4.3 实验步骤

#### 1. RTC初始化

```c
#include "stm32f4xx.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

void RTC_Config(void) {
    // 使能PWR时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    // 允许备份区域访问
    PWR_BackupAccessCmd(ENABLE);

    // 使能LSE（低速外部晶振）并等待就绪
    RCC_LSICmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

    // 选择LSE作为RTC时钟源
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RCC_RTCCLKCmd(ENABLE); // 使能RTC时钟

    // 配置RTC
    RTC_InitTypeDef RTC_InitStructure;
    RTC_StructInit(&RTC_InitStructure);
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);

    // 设置初始时间和日期
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;

    RTC_TimeStructure.RTC_Hours = 12;
    RTC_TimeStructure.RTC_Minutes = 0;
    RTC_TimeStructure.RTC_Seconds = 0;
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

    RTC_DateStructure.RTC_Year = 23; // 2023
    RTC_DateStructure.RTC_Month = RTC_Month_9; // 9月
    RTC_DateStructure.RTC_Date = 24; // 24日
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
}
```

#### 2. 串口初始化

```c
void USART_Config(void) {
    // 使能USART2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能GPIOA时钟

    // 配置GPIOA的引脚为复用功能（PA2为TX, PA3为RX）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 连接PA2到USART2_Tx和PA3到USART2_Rx
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // 配置USART参数
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE); // 使能USART
}
```

#### 3. 主循环与时间修改逻辑

在主循环中，我们会定期读取RTC的时间，并通过串口输出。同时，监听串口输入以修改时间。

```c
void USART_SendString(char *str) {
    while (*str) {
        while (!(USART_GetFlagStatus(USART2, USART_FLAG_TXE))); // 等待发送缓冲区空
        USART_SendData(USART2, *str++); // 发送字符
    }
}

void getTimeAndDate(void) {
    char buffer[50];
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;

    // 读取当前时间和日期
    RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);

    // 将时间和日期格式化为字符串
    sprintf(buffer, "当前时间: %02d:%02d:%02d, 当前日期: %04d-%02d-%02d\r\n",
            RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds,
            2000 + RTC_DateStructure.RTC_Year, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Date);

    USART_SendString(buffer); // 发送时间和日期
}

int main(void) {
    SystemInit(); // 系统初始化
    RTC_Config(); // RTC初始化
    USART_Config(); // 串口初始化

    while (1) {
        getTimeAndDate(); // 获取时间和日期并发送
        for (volatile int i = 0; i < 1000000; i++); // 延时
    }
}
```

---

2024.9.24 第一次修订，后期不再维护

2025.3.9 简化内容
