# 第六章 RCC介绍

## 1. 导入

    首先我们得了解RCC是什么：

    RCC ：reset clock control  也就是专门用来控制复位和时钟的，本章我们主要讲解时钟部分。

## 2. 为什么单片机需要系统时钟

时钟信号对于单片机（或微控制器）来说是至关重要的，它控制着微控制器内部和外部的所有时间相关操作。如果没有系统时钟，我们的程序就会发生混乱。

- 单片机内部的各个模块（如 CPU、内存、外设等）需要一个统一的时钟信号来进行同步操作。时钟信号决定了系统的时序，确保所有操作在正确的时间顺序执行。每个机器周期（Clock Cycle）对应于时钟信号的一个周期，CPU 和其他外设会根据时钟的边沿执行指令或操作。

- 单片机的中央处理单元（CPU）依赖时钟信号来驱动指令的执行。每个时钟周期，CPU 执行一条或多条指令，时钟频率决定了 CPU 的执行速度。时钟频率越高，CPU 每秒钟能处理的指令就越多，从而提高了单片机的处理能力。

- 除了 CPU，单片机的外设（如 ADC、PWM、USART 等）也需要时钟来进行定时操作。例如，ADC 模块需要时钟信号来驱动其转换过程，USART 需要时钟来控制数据的发送和接收速率。时钟信号确保这些外设以正确的速率和时序工作。

- 很多单片机应用需要实现定时器和计时器功能，这些功能的实现都依赖于时钟信号。定时器/计时器通过时钟周期计算时间间隔、触发中断等操作。没有时钟，单片机就无法完成定时、计时、延时等功能。

- 在数据通信中，时钟信号用于同步数据传输。例如，在 SPI、I2C 等通信协议中，时钟信号用于同步发送和接收的数据流。没有时钟信号，数据的发送与接收将无法保持同步，导致通信失败。

- 时钟系统的管理还涉及单片机的功耗控制。单片机可以根据需要关闭部分时钟源或降低时钟频率以降低功耗。通过时钟源的选择和分频，可以在性能与功耗之间做出平衡，特别是在低功耗模式下，时钟信号的管理变得尤为重要。

## 3. RCC主要作用-时钟部分

    设置系统时钟SYSCLK、设置AHB分频因子（决定HCLK等于多少）、设置APB2分频因子（决定PCLK2等于多少）、 设置APB1分频因子（决定PCLK1等于多少）、设置各个外设的分频因子；控制AHB、APB2和APB1这三条总线时钟的开启、 控制每个外设的时钟的开启。对于SYSCLK、HCLK、PCLK2、PCLK1这四个时钟的配置一般是： 

- PCLK2 = HCLK = SYSCLK=PLLCLK = 72M，这个时钟配置也是库函数的标准配置，我们用的最多的就是这个。

- PCLK1=HCLK/2 = 36M。

    控制AHB、APB2和APB1这三条总线时钟的开启、 控制每个外设的时钟的开启。这个我们应该很熟悉咯，已经用到过几次。想要使用GPIO就得先把外部时钟给开了。

    更多内容参考：[从小白到 Pro | RCC时钟基础知识和常见问题 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/145221712)

## 4. RCC框图剖析—时钟部分

     我们这里选取库函数时钟系统时钟函数：**SetSysClockTo72();** 以这个函数的编写流程来讲解时钟树， 这个函数也是我们用库的时候默认的系统时钟设置函数。 该函数的功能是利用HSE把时钟设置为：PCLK2 = HCLK = SYSCLK = 72M，PCLK1=HCLK/2 = 36M。 下面我们就以这个代码的流程为主线，来分析时钟树，对应的是图中的黄色部分，代码流程在时钟树中以数字的大小顺序标识。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/RCC002.png)

### 4.1 系统时钟

#### 4.1.1 HSE高速外部时钟信号

    HSE是高速的外部时钟信号，可以由有源晶振或者无源晶振提供，频率从4-16MHZ不等。当使用有源晶振时， 时钟从OSC_IN引脚进入，OSC_OUT引脚悬空，当选用无源晶振时，时钟从OSC_IN和OSC_OUT进入，并且要配谐振电容。

    HSE最常使用的就是8M的无源晶振。当确定PLL时钟来源的时候，HSE可以不分频或者2分频， 这个由时钟配置寄存器CFGR的位17：PLLXTPRE设置，我们设置为HSE不分频。

- PLL时钟源

    PLL时钟来源可以有两个，一个来自HSE，另外一个是HSI/2，具体用哪个由时钟配置寄存器CFGR的位16：PLLSRC设置。 HSI是内部高速的时钟信号，频率为8M，根据温度和环境的情况频率会有漂移，一般不作为PLL的时钟来源。这里我们选HSE作为PLL的时钟来源。

- PLL时钟PLLCLK

    通过设置PLL的倍频因子，可以对PLL的时钟来源进行倍频，倍频因子可以是:[2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]， 具体设置成多少，由时钟配置寄存器CFGR的位21-18：PLLMUL[3:0]设置。我们这里设置为9倍频， 因为上一步我们设置PLL的时钟来源为HSE=8M，所以经过PLL倍频之后的PLL时钟：PLLCLK = 8M *9 = 72M。 72M是ST官方推荐的稳定运行时钟，如果你想超频的话，增大倍频因子即可，最高为128M。 我们这里设置PLL时钟：PLLCLK = 8M *9 = 72M。

- 系统时钟SYSCLK

    系统时钟来源可以是：HSI、PLLCLK、HSE，具体的时钟配置寄存器CFGR的位1-0：SW[1:0]设置。 我们这里设置系统时钟：SYSCLK = PLLCLK = 72M。

- AHB总线时钟HCLK

    系统时钟SYSCLK经过AHB预分频器分频之后得到时钟叫APB总线时钟，即HCLK，分频因子可以是:[1,2,4，8，16，64，128，256，512]， 具体的由时钟配置寄存器CFGR的位7-4  ：HPRE[3:0]设置。片上大部分外设的时钟都是经过HCLK分频得到， 至于AHB总线上的外设的时钟设置为多少，得等到我们使用该外设的时候才设置， 我们这里只需粗线条的设置好APB的时钟即可。我们这里设置为1分频，即HCLK=SYSCLK=72M。

- APB2总线时钟PCLK2

    APB2总线时钟PCLK2由HCLK经过高速APB2预分频器得到，分频因子可以是:[1,2,4，8，16]，具体由时钟配置寄存器CFGR的位13-11：PPRE2[2:0]决定。 PCLK2属于高速的总线时钟，片上高速的外设就挂载到这条总线上，比如全部的GPIO、USART1、SPI1等。至于APB2总线上的外设的时钟设置为多少， 得等到我们使用该外设的时候才设置，我们这里只需粗线条的设置好APB2的时钟即可。我们这里设置为1分频，即PCLK2 = HCLK = 72M。

- APB1总线时钟PCLK1

    APB1总线时钟PCLK1由HCLK经过低速APB预分频器得到，分频因子可以是:[1,2,4，8，16]，具体的由时钟配置寄存器CFGR的位10-8：PRRE1[2:0]决定。 PCLK1属于低速的总线时钟，最高为36M，片上低速的外设就挂载到这条总线上，比如USART2/3/4/5、SPI2/3，I2C1/2等。 至于APB1总线上的外设的时钟设置为多少，得等到我们使用该外设的时候才设置，我们这里只需粗线条的设置好APB1的时钟即可。 我们这里设置为2分频，即PCLK1 = HCLK/2 = 36M。

    更多参考：[【STM32】系统时钟RCC详解(超详细，超全面)_rcc时钟-CSDN博客](https://blog.csdn.net/as480133937/article/details/98845509)

#### 4.1.2 设置系统时钟库函数

    上面的7个步骤对应的设置系统时钟库函数如下，该函数截取自固件库文件system_stm32f10x.c。为了方便阅读， 我已把互联型相关的代码删掉，把英文注释翻译成了中文，并把代码标上了序号，总共七个步骤。该函数是直接操作寄存器的， 有关寄存器部分请参考数据手册的RCC的寄存器描述部分。

```c
static void SetSysClockTo72(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    // ① 使能HSE，并等待HSE稳定
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);
    // 等待HSE启动稳定，并做超时处理
    do {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0)
        &&(StartUpCounter !=HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET) {
        HSEStatus = (uint32_t)0x01;
    } else {
        HSEStatus = (uint32_t)0x00;
    }
    // HSE启动成功，则继续往下处理
    if (HSEStatus == (uint32_t)0x01) {
        //-----------------------------------------------------------
        // 使能FLASH 预存取缓冲区 */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        // SYSCLK周期与闪存访问时间的比例设置，这里统一设置成2
        // 设置成2的时候，SYSCLK低于48M也可以工作，如果设置成0或者1的时候，
        // 如果配置的SYSCLK超出了范围的话，则会进入硬件错误，程序就死了
        // 0：0 < SYSCLK <= 24M
        // 1：24< SYSCLK <= 48M
        // 2：48< SYSCLK <= 72M */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;
        //------------------------------------------------------------
        // ② 设置AHB、APB2、APB1预分频因子
        // HCLK = SYSCLK
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
        //PCLK2 = HCLK
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
        //PCLK1 = HCLK/2
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
        // ③ 设置PLL时钟来源，设置PLL倍频因子，PLLCLK = HSE * 9 = 72 MHz
        RCC->CFGR &= (uint32_t)((uint32_t)
                                ~(RCC_CFGR_PLLSRC
                                | RCC_CFGR_PLLXTPRE
                                | RCC_CFGR_PLLMULL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE
                                | RCC_CFGR_PLLMULL9);
        // ④ 使能 PLL
        RCC->CR |= RCC_CR_PLLON;
        // ⑤ 等待PLL稳定
        while ((RCC->CR & RCC_CR_PLLRDY) == 0) {
        }
        // ⑥ 选择PLL作为系统时钟来源
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
        // ⑦ 读取时钟切换状态位，确保PLLCLK被选为系统时钟
        while ((RCC->CFGR&(uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08){
        }
    } else {// 如果HSE启动失败，用户可以在这里添加错误代码出来
    }
}
```

### 4.2 其他时钟

- USB时钟

    USB时钟是由PLLCLK经过USB预分频器得到，分频因子可以是：[1,1.5]，具体的由时钟配置寄存器CFGR的位22：USBPRE配置。 USB的时钟最高是48M，根据分频因子反推过来算，PLLCLK只能是48M或者是72M。一般我们设置PLLCLK=72M，USBCLK=48M。 USB对时钟要求比较高，所以PLLCLK只能是由HSE倍频得到，不能使用HSI倍频。

- Cortex系统时钟

    Cortex系统时钟由HCLK 8分频得到，等于9M， Cortex系统时钟用来驱动内核的系统定时器SysTick，SysTick一般用于操作系统的时钟节拍，也可以用做普通的定时。

- ADC时钟

    ADC时钟由PCLK2经过ADC预分频器得到，分频因子可以是[2,4,6,8]，具体的由时钟配置寄存器CFGR的位15-14：ADCPRE[1:0]决定。 很奇怪的是怎么没有1分频。ADC时钟最高只能是14M，如果采样周期设置成最短的1.5个周期的话，ADC的转换时间可以达到最短的1us。 如果真要达到最短的转换时间1us的话，那ADC的时钟就得是14M，反推PCLK2的时钟只能是：28M、56M、84M、112M， 鉴于PCLK2最高是72M，所以只能取28M和56M。

- RTC时钟、独立看门狗时钟

    RTC时钟可由HSE/128分频得到，也可由低速外部时钟信号LSE提供，频率为32.768KHZ，也可由低速内部时钟信号LSI提供， 具体选用哪个时钟由备份域控制寄存器BDCR的位9-8：RTCSEL[1:0]配置。独立看门狗的时钟由LSI提供， 且只能是由LSI提供，LSI是低速的内部时钟信号，频率为30~60KHZ直接不等，一般取40KHZ。

- MCO时钟输出

    MCO是microcontroller clock output的缩写，是微控制器时钟输出引脚，在STM32 F1系列中 由 PA8复用所得， 主要作用是可以对外提供时钟，相当于一个有源晶振。MCO的时钟来源可以是：PLLCLK/2、HSI、HSE、SYSCLK， 具体选哪个由时钟配置寄存器CFGR的位26-24：MCO[2:0]决定。除了对外提供时钟这个作用之外， 我们还可以通过示波器监控MCO引脚的时钟输出来验证我们的系统时钟配置是否正确。

## 5. 小结

STM32 微控制器的 **RCC**（Reset and Clock Control）模块负责管理和配置所有时钟源，包括系统时钟（SYSCLK）、外设时钟（APB、AHB 时钟）以及用于外设驱动的各类内部/外部时钟。时钟树配置是确保微控制器在正确的频率下运行、满足性能要求以及实现功耗优化的关键步骤。

一个典型的时钟树配置流程可能包括：

1. 启动 HSE（外部晶振），等待其稳定。
2. 配置 PLL（例如使用 HSE 作为 PLL 输入源，倍频器为 8）。
3. 启动 PLL，并等待其稳定。
4. 切换系统时钟源为 PLL 输出时钟。
5. 配置 AHB、APB 总线的时钟分频。
6. 启用外设时钟。

---

2024.8.19 第一次修订，后期不再维护

2024.12.7 修补内容，新增小结
