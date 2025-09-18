# 第二十四章 ADC介绍

## 1. ADC功能框图剖析

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/ADC002.png" title="" alt="" width="413">

掌握了ADC的功能框图，就可以对ADC有一个整体的把握，在编程的时候可以做到了然如胸，不会一知半解。 框图讲解采用从左到右的方式，跟ADC采集数据，转换数据，传输数据的方向大概一致。

### 2.1 电压输入范围

ADC输入范围为：V<sub>REF</sub>- ≤ V<sub>IN</sub> ≤ V<sub>REF+</sub>。由V<sub>REF-</sub>、 V<sub>REF+</sub> 、V<sub>DDA</sub> 、V<sub>SSA</sub>这四个外部引脚决定。

我们在设计原理图的时候一般把V<sub>SSA</sub>和V<sub>REF-</sub>接地， 把V<sub>REF+</sub>和V<sub>DDA</sub>接3V3，得到ADC的输入电压范围为：0~3.3V。

如果我们想让输入的电压范围变宽，去到可以测试负电压或者更高的正电压，我们可以在外部加一个电压调理电路， 把需要转换的电压抬升或者降压到0~3.3V，这样ADC就可以测量。

### 2.2 输入通道

我们确定好ADC输入电压之后，那么电压怎么输入到ADC？这里我们引入通道的概念，STM32的ADC多达18个通道， 其中外部的16个通道就是框图中的ADCx_IN0、ADCx_IN1…ADCx_IN5。这16个通道对应着不同的IO口，具体是哪一个IO口可以从手册查询到。 其中ADC1/2/3还有内部通道：ADC1的通道16连接到了芯片内部的温度传感器，Vrefint连接到了通道17。 ADC2的模拟通道16和17连接到了内部的VSS。ADC3的模拟通道9、14、15、16和17连接到了内部的VSS。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/ADC003.png)

外部的16个通道在转换的时候又分为规则通道和注入通道，其中规则通道最多有16路，注入通道最多有4路。那这两个通道有什么区别？在什么时候使用？

- 规则通道

规则通道：顾名思意，规则通道就是很规矩的意思，我们平时一般使用的就是这个通道，或者应该说我们用到的都是这个通道，没有什么特别要注意的可讲。

- 注入通道

注入，可以理解为插入，插队的意思，是一种不安分的通道。它是一种在规则通道转换的时候强行插入要转换的一种通道。 如果在规则通道转换过程中，有注入通道插队，那么就要先转换完注入通道，等注入通道转换完成后，再回到规则通道的转换流程。 这点跟中断程序很像，都是不安分的主。所以，注入通道只有在规则通道存在时才会出现。

### 2.3 转换顺序

- 规则顺序

规则序列寄存器有3个，分别为SQR3、SQR2、SQR1。SQR3控制着规则序列中的第一个到第六个转换， 对应的位为：SQ1[4:0]~SQ6[4:0]，第一次转换的是位4:0 SQ1[4:0]，如果通道16想第一次转换，那么在SQ1[4:0]写16即可。 SQR2控制着规则序列中的第7到第12个转换， 对应的位为：SQ7[4:0]~SQ12[4:0]，如果通道1想第8个转换，则SQ8[4:0]写1即可。SQR1控制着规则序列中的第13到第16个转换， 对应位为：SQ13[4:0]~SQ16[4:0]，如果通道6想第10个转换，则SQ10[4:0]写6即可。 具体使用多少个通道，由SQR1的位L[3:0]决定，最多16个通道。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/ADC004.png)

- 注入序列

注入序列寄存器JSQR只有一个，最多支持4个通道，具体多少个由JSQR的JL[1:0]决定。如果JL的 值小于4的话， 则JSQR跟SQR决定转换顺序的设置不一样，第一次转换的不是JSQR1[4:0]，而是JCQRx[4:0] ，x = 4-JL），跟SQR刚好相反。 如果JL=00（1个转换），那么转换的顺序是从JSQR4[4:0]开始，而不是从JSQR1[4:0]开始，这个要注意，编程的时候不要搞错。当JL等于4时，跟SQR一样。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/ADC005.png)

### 2.4 触发源

通道选好了，转换的顺序也设置好了，那接下来就该开始转换了。ADC转换可以由ADC控制寄存器2: ADC_CR2的ADON这个位来控制， 写1的时候开始转换，写0的时候停止转换，这个是最简单也是最好理解的开启ADC转换的控制方式，理解起来没啥技术含量。

除了这种庶民式的控制方法，ADC还支持触发转换，这个触发包括内部定时器触发和外部IO触发。触发源有很多，具体选择哪一种触发源， 由ADC控制寄存器2:ADC_CR2的EXTSEL[2:0]和JEXTSEL[2:0]位来控制。EXTSEL[2:0]用于选择规则通道的触发源， JEXTSEL[2:0]用于选择注入通道的触发源。选定好触发源之后，触发源是否要激活，则由ADC控制寄存器2:ADC_CR2的EXTTRIG和JEXTTRIG这两位来激活。 其中ADC3的规则转换和注入转换的触发源与ADC1/2的有所不同，在框图上已经表示出来。

### 2.5 转换时间

- ADC时钟

ADC输入时钟ADC_CLK由PCLK2经过分频产生，最大是14M，分频因子由RCC时钟配置寄存器RCC_CFGR的位15:14 ADCPRE[1:0]设置， 可以是2/4/6/8分频，注意这里没有1分频。一般我们设置PCLK2=HCLK=72M。

- 采样时间

ADC使用若干个ADC_CLK周期对输入的电压进行采样， 采样的周期数可通过ADC 采样时间寄存器ADC_SMPR1和ADC_SMPR2中的SMP[2:0]位设置， ADC_SMPR2控制的是通道0~9，ADC_SMPR1控制的是通道10~17。每个通道可以分别用不同的时间采样。其中采样周期最小是1.5个， 即如果我们要达到最快的采样，那么应该设置采样周期为1.5个周期，这里说的周期就是1/ADC_CLK。

ADC的转换时间跟ADC的输入时钟和采样时间有关，公式为：Tconv = 采样时间 + 12.5个周期。当ADCLK = 14MHZ （最高）， 采样时间设置为1.5周期（最快），那么总的转换时间（最短）Tconv = 1.5周期 + 12.5周期 = 14周期 = 1us。

一般我们设置PCLK2=72M，经过ADC预分频器能分频到最大的时钟只能是12M，采样周期设置为1.5个周期， 算出最短的转换时间为1.17us，这个才是最常用的。

### 2.6 数据寄存器

一切准备就绪后，ADC转换后的数据根据转换组的不同，规则组的数据放在ADC_DR寄存器，注入组的数据放在JDRx。

- 规则数据寄存器

ADC规则组数据寄存器ADC_DR只有一个，是一个32位的寄存器，低16位在单ADC时使用，高16位是在ADC1中双模式下保存ADC2转换的规则数据， 双模式就是ADC1和ADC2同时使用。在单模式下，ADC1/2/3都不使用高16位。因为ADC的精度是12位，无论ADC_DR的高16或者低16位都放不满， 只能左对齐或者右对齐，具体是以哪一种方式存放，由ADC_CR2的11位ALIGN设置。

规则通道可以有16个这么多，可规则数据寄存器只有一个，如果使用多通道转换，那转换的数据就全部都挤在了DR里面，前一个时间点转换的通道数据， 就会被下一个时间点的另外一个通道转换的数据覆盖掉，所以当通道转换完成后就应该把数据取走，或者开启DMA模式，把数据传输到内存里面， 不然就会造成数据的覆盖。最常用的做法就是开启DMA传输。

- 注入数据寄存器

ADC注入组最多有4个通道，刚好注入数据寄存器也有4个，每个通道对应着自己的寄存器，不会跟规则寄存器那样产生数据覆盖的问题。 ADC_JDRx是32位的，低16位有效，高16位保留，数据同样分为左对齐和右对齐，具体是以哪一种方式存放，由ADC_CR2的11位ALIGN设置。

### 2.7 中断

- 转换结束中断

数据转换结束后，可以产生中断，中断分为三种：规则通道转换结束中断，注入转换通道转换结束中断，模拟看门狗中断。 其中转换结束中断很好理解，跟我们平时接触的中断一样，有相应的中断标志位和中断使能位，我们还可以根据中断类型写相应配套的中断服务程序。

- 模拟看门狗中断

当被ADC转换的模拟电压低于低阈值或者高于高阈值时，就会产生中断，前提是我们开启了模拟看门狗中断， 其中低阈值和高阈值由ADC_LTR和ADC_HTR设置。例如我们设置高阈值是2.5V，那么模拟电压超过2.5V的时候，就会产生模拟看门狗中断，反之低阈值也一样。

- DMA请求

规则和注入通道转换结束后，除了产生中断外，还可以产生DMA请求，把转换好的数据直接存储在内存里面。 要注意的是只有ADC1和ADC3可以产生DMA请求。

### 2.8 电压转换

模拟电压经过ADC转换后，是一个12位的数字值，如果通过串口以16进制打印出来的话，可读性比较差，那么有时候我们就需要把数字电压转换成模拟电压， 也可以跟实际的模拟电压（用万用表测）对比，看看转换是否准确。

我们一般在设计原理图的时候会把ADC的输入电压范围设定在：0~3.3v，因为ADC是12位的，那么12位满量程对应的就是3.3V， 12位满量程对应的数字值是：2^12。数值0对应的就是0V。如果转换后的数值为  X ，X对应的模拟电压为Y， 那么会有这么一个等式成立：  2^12 / 3.3 = X/ Y，=> Y = (3.3 * X ) / 2^12。

## 3. ADC初始化结构体详解

标准库函数对每个外设都建立了一个初始化结构体xxx_InitTypeDef(xxx为外设名称)，结构体成员用于设置外设工作参数， 并由标准库函数xxx_Init()调用这些设定参数进入设置外设相应的寄存器，达到配置外设工作环境的目的。

结构体xxx_InitTypeDef和库函数xxx_Init配合使用是标准库精髓所在，理解了结构体xxx_InitTypeDef每个成员意义基本上就可以对该外设运用自如了。 结构体xxx_InitTypeDef定义在stm32f10x_xxx.h文件中，库函数xxx_Init定义在stm32f10x_xxx.c文件中，编程时我们可以结合这两个文件内注释使用。

- ADC_InitTypeDef结构体

ADC_InitTypeDef结构体定义在stm32f10x_adc.h文件内，具体定义如下：

```c
typedef struct
{
    uint32_t ADC_Mode;                      // ADC 工作模式选择
    FunctionalState ADC_ScanConvMode;       /* ADC 扫描（多通道）
                                            或者单次（单通道）模式选择 */
    FunctionalState ADC_ContinuousConvMode; // ADC 单次转换或者连续转换选择
    uint32_t ADC_ExternalTrigConv;          // ADC 转换触发信号选择
    uint32_t ADC_DataAlign;                 // ADC 数据寄存器对齐格式
    uint8_t ADC_NbrOfChannel;               // ADC 采集通道数
} ADC_InitTypeDef;
```

ADC_Mode：配置ADC的模式，当使用一个ADC时是独立模式，使用两个ADC时是双模式，在双模式下还有很多细分模式可选，我们一般使用一个ADC的独立模式。

ScanConvMode：可选参数为ENABLE和DISABLE，配置是否使用扫描。如果是单通道AD转换使用DISABLE，如果是多通道AD转换使用ENABLE。

ADC_ContinuousConvMode：可选参数为ENABLE和DISABLE，配置是启动自动连续转换还是单次转换。使用ENABLE配置为使能自动连续转换； 使用DISABLE配置为单次转换，转换一次后停止需要手动控制才重新启动转换。一般设置为连续转换。

ADC_ExternalTrigConv：外部触发选择，框图图中列举了很多外部触发条件， 可根据项目需求配置触发来源。实际上，我们一般使用软件自动触发。

ADC_DataAlign：转换结果数据对齐模式，可选右对齐ADC_DataAlign_Right或者左对齐ADC_DataAlign_Left。一般我们选择右对齐模式。

ADC_NbrOfChannel：AD转换通道数目，根据实际设置即可。

---

### 3.1 例子：配置 ADC 进行多通道扫描转换

假设我们需要采集两个通道（例如，PA0 和 PA1），以连续模式运行，并使用定时器作为触发信号。

```c
#include "stm32f4xx.h"

void ADC_Config(void) {
    ADC_InitTypeDef ADC_InitStruct;
    // 使能 ADC 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // 配置 ADC 初始化结构体
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent; // 独立模式
    ADC_InitStruct.ADC_ScanConvMode = ENABLE;      // 启用扫描模式
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; // 启用连续转换模式
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; // 定时器1触发
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right; // 右对齐
    ADC_InitStruct.ADC_NbrOfChannel = 2; // 采集两个通道
    // 初始化 ADC
    ADC_Init(ADC1, &ADC_InitStruct);
    // 配置通道
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_3Cycles);
    // 启动 ADC
    ADC_Cmd(ADC1, ENABLE);
    // 启动定时器（假设已配置好）
    TIM_Cmd(TIM1, ENABLE);
}

int main(void) {
    ADC_Config();

    while (1) {
        // 主循环中可以读取 ADC 数据
        uint16_t value1 = ADC_GetConversionValue(ADC1); // 获取通道0的值
        uint16_t value2 = ADC_GetConversionValue(ADC1); // 获取通道1的值
        // 处理数据...
    }
}
```

### 3.2 说明

- **ADC_Mode** 设置为独立模式，表示该 ADC 独立工作。
- **ADC_ScanConvMode** 和 **ADC_ContinuousConvMode** 启用扫描和连续模式，使 ADC 自动切换并持续读取通道。
- **ADC_ExternalTrigConv** 使用定时器1的比较匹配事件作为转换触发源。
- **ADC_DataAlign** 设置数据右对齐，以便于后续数据处理。
- **ADC_NbrOfChannel** 设置为2，表示我们将采集两个通道。

这样配置后，ADC 将根据定时器的触发信号，自动读取 PA0 和 PA1 的值。

---

2024.9.19 第一次修订，后期不再维护

2025.1.30 简化
