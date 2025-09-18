# 第二十章 TIM-高级定时器

## 1. 高级控制定时器

高级控制定时器(TIM1和TIM8)和通用定时器在基本定时器的基础上引入了外部引脚，可以实现输入捕获和输出比较功能。 高级控制定时器比通用定时器增加了可编程死区互补输出、重复计数器、带刹车(断路)功能，这些功能都是针对工业电机控制方面。

高级控制定时器时基单元包含一个16位自动重装载寄存器ARR，一个16位的计数器CNT，可向上/下计数，一个16位可编程预分频器PSC， 预分频器时钟源有多种可选，有内部的时钟、外部时钟。还有一个8位的重复计数器RCR，这样最高可实现40位的可编程定时。

STM32F103ZET6的高级/通用定时器的IO分配具体见表 [高级控制和通用定时器通道引脚分布，配套开发板因为IO资源紧缺， 定时器的IO很多已经复用它途，故下表中的IO只有部分可用于定时器的实验。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen01.png" title="" alt="" width="753">

## 2. 高级控制定时器功能框图

高级控制定时器功能框图包含了高级控制定时器最核心内容，掌握了功能框图，对高级控制定时器就有一个整体的把握，在编程时思路就非常清晰，图中有些寄存器是带影子的，表示其有影子寄存器。

<img title="" src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen002.png" alt="" width="864">

### 2.1 时钟源

高级控制定时器有四个时钟源可选：

- 内部时钟源CK_INT

- 外部时钟模式1：外部输入引脚TIx（x=1,2,3,4）

- 外部时钟模式2：外部触发输入ETR

- 内部触发输入(ITRx)

#### 2.1.1 内部时钟源（CK_INT）

内部时钟CK_INT即来自于芯片内部，等于72M，一般情况下，我们都是使用内部时钟。当从模式控制寄存器TIMx_SMCR的SMS位等于000时，则使用内部时钟。

#### 2.1.2 外部时钟模式1

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen003.png)

1. 时钟输入引脚

当使用外部时钟模式1的时候，时钟信号来自于定时器的输入通道，总共有4个，分别为TI1/2/3/4，即TIMx_CH1/2/3/4。具体使用哪一路信号， 由TIM_CCMRx的位CCxS[1:0]配置，其中CCMR1控制TI1/2，CCMR2控制TI3/4。

2. 滤波器

如果来自外部的时钟信号的频率过高或者混杂有高频干扰信号的话，我们就需要使用滤波器对信号重新采样，来达到降频或者去除高频干扰的目的，具体的由TIMx_CCMRx的位ICxF[3:0]配置。

3. 边沿检测

边沿检测的信号来自于滤波器的输出，在成为触发信号之前，需要进行边沿检测，决定是上升沿有效还是下降沿有效，具体的由TIMx_CCER的位CCxP和CCxNP配置。

4. 触发选择

当使用外部时钟模式1时，触发源有两个，一个是滤波后的定时器输入1（TI1FP1）和滤波后的定时器输入2（TI2FP2），具体的由TIMxSMCR的位TS[2:0]配置。

5. 从模式选择

选定了触发源信号后，最后我们需把信号连接到TRGI引脚，让触发信号成为外部时钟模式1的输入，最终等于CK_PSC，然后驱动计数器CNT计数。 具体的配置TIMx_SMCR的位SMS[2:0]为000即可选择外部时钟模式1。

6. 使能计数器

经过上面的5个步骤之后，最后我们只需使能计数器开始计数，外部时钟模式1的配置就算完成。使能计数器由TIMx_CR1的位CEN配置。

#### 2.1.3 外部时钟模式2

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen004.png)

1. 时钟信号输入引脚

当使用外部时钟模式2的时候，时钟信号来自于定时器的特定输入通道TIMx_ETR，只有1个。

2. 外部触发极性

来自ETR引脚输入的信号可以选择为上升沿或者下降沿有效，具体的由TIMx_SMCR的位ETP配置。

3. 外部触发预分频器

由于ETRP的信号的频率不能超过TIMx_CLK（72M）的1/4，当触发信号的频率很高的情况下， 就必须使用分频器来降频，具体的由 TIMx_SMCR的位ETPS[1:0]配置。

4. 滤波器

如果ETRP的信号的频率过高或者混杂有高频干扰信号的话，我们就需要使用滤波器对ETRP信号重新采样， 来达到降频或者去除高频干扰的目的。具体的由TIMx_SMCR的位ETF[3:0]配置， 其中的fDTS是由内部时钟CK_INT分频得到，具体的由TIMx_CR1的位CKD[1:0]配置。

5. 从模式选择

经过滤波器滤波的信号连接到ETRF引脚后，触发信号成为外部时钟模式2的输入，最终等于CK_PSC，然后驱动计数器CNT计数。具体的配置TIMx_SMCR的位ECE为1即可选择外部时钟模式2。

6. 使能计数器

经过上面的5个步骤之后，最后我们只需使能计数器开始计数，外部时钟模式2的配置就算完成。使能计数器由TIMx_CR1的位CEN配置。

#### 2.1.4 内部触发输入

内部触发输入是使用一个定时器作为另一个定时器的预分频器。硬件上高级控制定时器和通用定时器在内部连接在一起， 可以实现定时器同步或级联。主模式的定时器可以对从模式定时器执行复位、启动、停止或提供时钟。

### 2.2 控制器

高级控制定时器控制器部分包括触发控制器、从模式控制器以及编码器接口。触发控制器用来针对片内外设输出触发信号，比如为其它定时器提供时钟和触发DAC/ADC转换。

编码器接口专门针对编码器计数而设计。从模式控制器可以控制计数器复位、启动、递增/递减、计数。有关控制器部分只需熟练阅读寄存器描述即可。

### 2.3 时基单元

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen005.png)

高级控制定时器时基单元功能包括四个寄存器，分别是计数器寄存器(CNT)、预分频器寄存器(PSC)、自动重载寄存器(ARR)和重复计数器寄存器(RCR)。 其中重复计数器RCR是高级定时器独有，通用和基本定时器没有。

前面三个寄存器都是16位有效，TIMx_RCR寄存器是8位有效。

#### 2.3.1 预分频器PSC

预分频器PSC，有一个输入时钟CK_PSC和一个输出时钟CK_CNT。输入时钟CK_PSC就是上面时钟源的输出，输出CK_CNT则用来驱动计数器CNT计数。 通过设置预分频器PSC的值可以得到不同的CK_CNT， 实际计算为：f<sub>CK_CNT等于f<sub>CK_PSC/(PSC[15:0]+1)，可以实现1至65536分频。

#### 2.3.2 计数器CNT

高级控制定时器的计数器有三种计数模式，分别为递增计数模式、递减计数模式和递增/递减(中心对齐)计数模式。

1. 递增计数模式下，计数器从0开始计数，每来一个CK_CNT脉冲计数器就增加1，直到计数器的值与自动重载寄存器ARR值相等， 然后计数器又从0开始计数并生成计数器上溢事件，计数器总是如此循环计数。如果禁用重复计数器，在计数器生成上溢事件就马上生成更新事件(UEV)； 如果使能重复计数器，每生成一次上溢事件重复计数器内容就减1，直到重复计数器内容为0时才会生成更新事件。

2.  递减计数模式下，计数器从自动重载寄存器ARR值开始计数，每来一个CK_CNT脉冲计数器就减1，直到计数器值为0， 然后计数器又从自动重载寄存器ARR值开始递减计数并生成计数器下溢事件，计数器总是如此循环计数。如果禁用重复计数器， 在计数器生成下溢事件就马上生成更新事件；如果使能重复计数器，每生成一次下溢事件重复计数器内容就减1，直到重复计数器内容为0时才会生成更新事件。

3. 中心对齐模式下，计数器从0开始递增计数，直到计数值等于(ARR-1)值生成计数器上溢事件， 然后从ARR值开始递减计数直到1生成计数器下溢事件。然后又从0开始计数，如此循环。每次发生计数器上溢和下溢事件都会生成更新事件。

#### 2.3.3 自动重载寄存器ARR

自动重载寄存器ARR用来存放与计数器CNT比较的值，如果两个值相等就递减重复计数器。可以通过TIMx_CR1寄存器的ARPE位控制自动重载影子寄存器功能， 如果ARPE位置1，自动重载影子寄存器有效，只有在事件更新时才把TIMx_ARR值赋给影子寄存器。如果ARPE位为0，则修改TIMx_ARR值马上有效。

#### 2.3.4 重复计数器RCR

在基本/通用定时器发生上/下溢事件时直接就生成更新事件，但对于高级控制定时器却不是这样，高级控制定时器在硬件结构上多出了重复计数器， 在定时器发生上溢或下溢事件是递减重复计数器的值，只有当重复计数器为0时才会生成更新事件。在发生N+1个上溢或下溢事件(N为RCR的值)时产生更新事件。

### 2.4 输入捕获

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen006.png)

输入捕获可以对输入的信号的上升沿、下降沿或者双边沿进行捕获，常用的有测量输入信号的脉宽，和测量PWM输入信号的频率和占空比这两种。

输入捕获的大概的原理就是，当捕获到信号的跳变沿的时候，把计数器CNT的值锁存到捕获寄存器CCR中，把前后两次捕获到的CCR寄存器中的值相减， 就可以算出脉宽或者频率。如果捕获的脉宽的时间长度超过你的捕获定时器的周期，就会发生溢出，这个我们需要做额外的处理。

#### 2.4.1 输入通道

需要被测量的信号从定时器的外部引脚TIMx_CH1/2/3/4进入，通常叫TI1/2/3/4，在后面的捕获讲解中对于要被测量的信号我们都以TIx为标准叫法。

#### 2.4.2 输入滤波器和边沿检测器

当输入的信号存在高频干扰的时候，我们需要对输入信号进行滤波，即进行重新采样，根据采样定律，采样的频率必须大于等于两倍的输入信号。 比如输入的信号为1M，又存在高频的信号干扰，那么此时就很有必要进行滤波，我们可以设置采样频率为2M， 这样可以在保证采样到有效信号的基础上把高于2M的高频干扰信号过滤掉。

滤波器的配置由CR1寄存器的位CKD[1:0]和CCMR1/2的位ICxF[3:0]控制。从ICxF位的描述可知， 采样频率f<sub>SAMPLE可以由f<sub>CK_INT和f<sub>DTS分频后的时钟提供， 其中是f<sub>CK_INT内部时钟，f<sub>DTS是f<sub>CK_INT经过分频后得到的频率， 分频因子由CKD[1:0]决定，可以是不分频，2分频或者是4分频。

边沿检测器用来设置信号在捕获的时候是什么边沿有效，可以是上升沿，下降沿，或者是双边沿，具体的由CCER寄存器的位CCxP和CCxNP决定。

#### 2.4.3 捕获通道

捕获通道就是图中的IC1/2/3/4，每个捕获通道都有相对应的捕获寄存器CCR1/2/3/4，当发生捕获的时候，计数器CNT的值就会被锁存到捕获寄存器中。

这里我们要搞清楚输入通道和捕获通道的区别，输入通道是用来输入信号的，捕获通道是用来捕获输入信号的通道， 一个输入通道的信号可以同时输入给两个捕获通道。比如输入通道TI1的信号经过滤波边沿检测器之后的TI1FP1和TI1FP2可以进入到捕获通道IC1和IC2， 其实这就是我们后面要讲的PWM输入捕获，只有一路输入信号（TI1）却占用了两个捕获通道（IC1和IC2）。当只需要测量输入信号的脉宽时候， 用一个捕获通道即可。输入通道和捕获通道的映射关系具体由寄存器CCMRx的位CCxS[1:0]配置。

#### 2.4.4 预分频器

ICx的输出信号会经过一个预分频器，用于决定发生多少个事件时进行一次捕获。具体的由寄存器CCMRx的位ICxPSC配置，如果希望捕获信号的每一个边沿，则不分频。

#### 2.4.5 捕获寄存器

经过预分频器的信号ICxPS是最终被捕获的信号，当发生捕获时（第一次），计数器CNT的值会被锁存到捕获寄存器CCR中， 还会产生CCxI中断，相应的中断位CCxIF（在SR寄存器中）会被置位，通过软件或者读取CCR中的值可以将CCxIF清0。 如果发生第二次捕获（即重复捕获：CCR寄存器中已捕获到计数器值且 CCxIF 标志已置 1）， 则捕获溢出标志位CCxOF（在SR寄存器中）会被置位，CCxOF只能通过软件清零。

### 2.5 输出比较

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen007.png)

输出比较就是通过定时器的外部引脚对外输出控制信号，有冻结、将通道X（x=1,2,3,4）设置为匹配时输出有效电平、 将通道X设置为匹配时输出无效电平、翻转、强制变为无效电平、强制变为有效电平、PWM1和PWM2这八种模式， 具体使用哪种模式由寄存器CCMRx的位OCxM[2:0]配置。其中PWM模式是输出比较中的特例，使用的也最多。

#### 2.5.1 比较寄存器

当计数器CNT的值跟比较寄存器CCR的值相等的时候，输出参考信号OCxREF的信号的极性就会改变，其中OCxREF=1（高电平）称之为有效电平， OCxREF=0（低电平）称之为无效电平，并且会产生比较中断CCxI，相应的标志位CCxIF（SR寄存器中）会置位。 然后OCxREF再经过一系列的控制之后就成为真正的输出信号OCx/OCxN。

#### 2.5.2 死区发生器

在生成的参考波形OCxREF的基础上，可以插入死区时间，用于生成两路互补的输出信号OCx和OCxN， 死区时间的大小具体由BDTR寄存器的位DTG[7:0]配置。死区时间的大小必须根据与输出信号相连接的器件及其特性来调整。 下面我们简单举例说明下带死区的PWM信号的应用，我们以一个板桥驱动电路为例。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen008.png)

在这个半桥驱动电路中，Q1导通，Q2截止，此时我想让Q1截止Q2导通，肯定是要先让Q1截止一段时间之后，再等一段时间才让Q2导通， 那么这段等待的时间就称为死区时间，因为Q1关闭需要时间（由MOS管的工艺决定）。如果Q1关闭之后，马上打开Q2， 那么此时一段时间内相当于Q1和Q2都导通了，这样电路会短路。

图是针对上面的半桥驱动电路而画的带死区插入的PWM信号，图中的死区时间要根据MOS管的工艺来调节。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen009.png)

#### 2.5.3 输出控制

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen010.png)

在输出比较的输出控制中，参考信号OCxREF在经过死区发生器之后会产生两路带死区的互补信号OCx_DT和OCxN_DT（通道1~3才有互补信号，通道4没有， 其余跟通道1~3一样），这两路带死区的互补信号然后就进入输出控制电路，如果没有加入死区控制，那么进入输出控制电路的信号就直接是OCxREF。

进入输出控制电路的信号会被分成两路，一路是原始信号，一路是被反向的信号，具体的由寄存器CCER的位CCxP和CCxNP控制。 经过极性选择的信号是否由OCx引脚输出到外部引脚CHx/CHxN则由寄存器CCER的位CxE/CxNE配置。

如果加入了断路（刹车）功能，则断路和死区寄存器BDTR的MOE、OSSI和OSSR这三个位会共同影响输出的信号。

#### 2.5.4 输出引脚

输出比较的输出信号最终是通过定时器的外部IO来输出的，分别为CH1/2/3/4， 其中前面三个通道还有互补的输出通道CH1/2/3N。

### 2.6 断路功能

断路功能就是电机控制的刹车功能，使能断路功能时，根据相关控制位状态修改输出信号电平。在任何情况下， OCx和OCxN输出都不能同时为有效电平，这关系到电机控制常用的H桥电路结构原因。

断路源可以是时钟故障事件，由内部复位时钟控制器中的时钟安全系统(CSS)生成，也可以是外部断路输入IO，两者是或运算关系。

系统复位启动都默认关闭断路功能，将断路和死区寄存器(TIMx_BDTR)的BKE为置1，使能断路功能。 可通过TIMx_BDTR 寄存器的BKP位设置，设置断路输入引脚的有效电平，设置为1时输入BRK为高电平有效，否则低电平有效。

发送断路时，将产生以下效果：

- TIMx_BDTR 寄存器中主输出模式使能(MOE)位被清零，输出处于无效、空闲或复位状态；

- 根据相关控制位状态控制输出通道引脚电平；当使能通道互补输出时，会根据情况自动控制输出通道电平；

- 将TIMx_SR 寄存器中的 BIF位置 1，并可产生中断和DMA传输请求。

- 如果 TIMx_BDTR 寄存器中的 自动输出使能(AOE)位置 1，则MOE位会在发生下一个UEV事件时自动再次置 1。

## 3. 输入捕获应用

输入捕获一般应用在两个方面，一个方面是脉冲跳变沿时间测量，另一方面是PWM输入测量。

### 3.1 测量脉宽或者频率

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen011.png)

#### 3.1.1 测量频率

当捕获通道TIx上出现上升沿时，发生第一次捕获，计数器CNT的值会被锁存到捕获寄存器CCR中，而且还会进入捕获中断， 在中断服务程序中记录一次捕获（可以用一个标志变量来记录），并把捕获寄存器中的值读取到value1中。当出现第二次上升沿时， 发生第二次捕获，计数器CNT的值会再次被锁存到捕获寄存器CCR中，并再次进入捕获中断，在捕获中断中，把捕获寄存器的值读取到value3中， 并清除捕获记录标志。利用value3和value1的差值我们就可以算出信号的周期（频率）。

#### 3.1.2 测量脉宽

当捕获通道TIx上出现上升沿时，发生第一次捕获，计数器CNT的值会被锁存到捕获寄存器CCR中，而且还会进入捕获中断， 在中断服务程序中记录一次捕获（可以用一个标志变量来记录），并把捕获寄存器中的值读取到value1中。然后把捕获边沿改变为下降沿捕获， 目的是捕获后面的下降沿。当下降沿到来的时候，发生第二次捕获，计数器CNT的值会再次被锁存到捕获寄存器CCR中，并再次进入捕获中断，在捕获中断中， 把捕获寄存器的值读取到value3中，并清除捕获记录标志。然后把捕获边沿设置为上升沿捕获。

在测量脉宽过程中需要来回的切换捕获边沿的极性，如果测量的脉宽时间比较长，定时器就会发生溢出， 溢出的时候会产生更新中断，我们可以在中断里面对溢出进行记录处理。

### 3.2 PWM输入模式

测量脉宽和频率还有一个更简便的方法就是使用PWM输入模式，该模式是输入捕获的特例，只能使用通道1和通道2，通道3和通道4使用不了。 与上面那种只使用一个捕获寄存器测量脉宽和频率的方法相比，PWM输入模式需要占用两个捕获寄存器。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen012.png)

当使用PWM输入模式的时候，因为一个输入通道(TIx)会占用两个捕获通道(ICx)，所以一个定时器在使用PWM输入的时候最多只能使用两个输入通道(TIx)。

我们以输入通道TI1工作在PWM输入模式为例来讲解下具体的工作原理，其他通道以此类推即可。

PWM信号由输入通道TI1进入，因为是PWM输入模式的缘故，信号会被分为两路，一路是TI1FP1，另外一路是TI1FP2。其中一路是周期， 另一路是占空比，具体哪一路信号对应周期还是占空比，得从程序上设置哪一路信号作为触发输入，作为触发输入的哪一路信号对应的就是周期， 另一路就是对应占空比。作为触发输入的那一路信号还需要设置极性，是上升沿还是下降沿捕获，一旦设置好触发输入的极性， 另外一路硬件就会自动配置为相反的极性捕获，无需软件配置。一句话概括就是：选定输入通道，确定触发信号，然后设置触发信号的极性即可， 因为是PWM输入的缘故，另一路信号则由硬件配置，无需软件配置。

当使用PWM输入模式的时候必须将从模式控制器配置为复位模式（配置寄存器SMCR的位SMS[2:0]来实现）， 即当我们启动触发信号开始进行捕获的时候，同时把计数器CNT复位清零。

下面我们以一个更加具体的时序图来分析下PWM输入模式。

<img src="https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen013.png" title="" alt="" width="1097">

PWM信号由输入通道TI1进入，配置TI1FP1为触发信号，上升沿捕获。当上升沿的时候IC1和IC2同时捕获，计数器CNT清零， 到了下降沿的时候，IC2捕获，此时计数器CNT的值被锁存到捕获寄存器CCR2中，到了下一个上升沿的时候，IC1捕获， 计数器CNT的值被锁存到捕获寄存器CCR1中。其中CCR2+1测量的是脉宽，CCR1+1测量的是周期。 这里要注意的是CCR2和CCR1的值在计算占空比和频率的时候都必须加1，因为计数器是从0开始计数的。

从软件上来说，用PWM输入模式测量脉宽和周期更容易，付出的代价是需要占用两个捕获寄存器。

## 4. 输出比较应用

输出比较模式总共有8种，具体的由寄存器CCMRx的位OCxM[2:0]配置。我们这里只讲解最常用的PWM模式，其他几种模式具体的看数据手册即可。

### 4.1 PWM输出模式

PWM输出就是对外输出脉宽（即占空比）可调的方波信号，信号频率由自动重装寄存器ARR的值决定，占空比由比较寄存器CCR的值决定。

PWM模式分为两种，PWM1和PWM2，总得来说是差不多，就看你怎么用而已， 具体的区别见表格：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen02.png)

下面我们以PWM1模式来讲解，以计数器CNT计数的方向不同还分为边沿对齐模式和中心对齐模式。PWM信号主要都是用来控制电机， 一般的电机控制用的都是边沿对齐模式，FOC电机一般用中心对齐模式。我们这里只分析这两种模式在信号感官上（即信号波形）的区别， 具体在电机控制中的区别不做讨论，到了你真正需要使用的时候就会知道了。

#### 4.1.1 PWM边沿对齐模式

在递增计数模式下，计数器从 0 计数到自动重载值（ TIMx_ARR寄存器的内容），然后重新从 0 开始计数并生成计数器上溢事件

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen014.png)

在边沿对齐模式下，计数器CNT只工作在一种模式，递增或者递减模式。这里我们以CNT工作在递增模式为例， 在中，ARR=8，CCR=4，CNT从0开始计数，当CNT<CCR的值时，OCxREF为有效的高电平，于此同时，比较中断寄存器CCxIF置位。 当CCR=<CNT<=ARR时，OCxREF为无效的低电平。然后CNT又从0开始计数并生成计数器上溢事件，以此循环往复。

#### 4.1.2 PWM中心对齐模式

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen015.png)

在中心对齐模式下，计数器CNT是工作做递增/递减模式下。开始的时候，计数器CNT从 0 开始计数到自动重载值减1(ARR-1)， 生成计数器上溢事件；然后从自动重载值开始向下计数到 1 并生成计数器下溢事件。之后从0 开始重新计数。

图是PWM1模式的中心对齐波形，ARR=8，CCR=4。 第一阶段计数器CNT工作在递增模式下，从0开始计数，当CNT<CCR的值时，OCxREF为有效的高电平， 当CCR=<CNT<<ARR时，OCxREF为无效的低电平。

第二阶段计数器CNT工作在递减模式，从ARR的值开始递减， 当CNT>CCR时，OCxREF为无效的低电平，当CCR=>CNT>=1时，OCxREF为有效的高电平。

在波形图上我们把波形分为两个阶段，第一个阶段是计数器CNT工作在递增模式的波形，这个阶段我们又分为①和②两个阶段， 第二个阶段是计数器CNT工作在递减模式的波形，这个阶段我们又分为③和④两个阶段。要说中心对齐模式下的波形有什么特征的话， 那就是①和③阶段的时间相等，②和④阶段的时间相等。

中心对齐模式又分为中心对齐模式1/2/3 三种，具体由寄存器CR1位CMS[1:0]配置。 具体的区别就是比较中断中断标志位CCxIF在何时置1：中心模式1在CNT递减计数的时候置1， 中心对齐模式2在CNT递增计数时置1，中心模式3在CNT递增和递减计数时都置1。

## 5. 定时器初始化结构体详解

在标准库函数头文件stm32f10x_tim.h中对定时器外设建立了四个初始化结构体，分别为时基初始化结构体TIM_TimeBaseInitTypeDef、 输出比较初始化结构体TIM_OCInitTypeDef、输入捕获初始化结构体TIM_ICInitTypeDef和断路和死区初始化结构体TIM_BDTRInitTypeDef， 高级控制定时器可以用到所有初始化结构体，通用定时器不能使用TIM_BDTRInitTypeDef结构体， 基本定时器只能使用时基结构体。接下来我们具体讲解下这四个结构体。

### 5.1 TIM_TimeBaseInitTypeDef

时基结构体TIM_TimeBaseInitTypeDef用于定时器基础参数设置，与TIM_TimeBaseInit函数配合使用完成配置。

```c
typedef struct {
    uint16_t TIM_Prescaler;       // 预分频器
    uint16_t TIM_CounterMode;     // 计数模式
    uint32_t TIM_Period;          // 定时器周期
    uint16_t TIM_ClockDivision;   // 时钟分频
    uint8_t TIM_RepetitionCounter;// 重复计算器
} TIM_TimeBaseInitTypeDef;
```

- TIM_Prescaler： 定时器预分频器设置，时钟源经该预分频器才是定时器计数时钟CK_CNT，它设定PSC寄存器的值。 计算公式为：计数器时钟频率 (f<sub>CK_CNT) 等于 f<sub>CK_PSC / (PSC[15:0] + 1)，可实现1至65536分频。

- TIM_CounterMode： 定时器计数方式，可设置为向上计数、向下计数以及中心对齐。高级控制定时器允许选择任意一种。

- TIM_Period： 定时器周期，实际就是设定自动重载寄存器ARR的值，ARR 为要装载到实际自动重载寄存器（即影子寄存器）的值，可设置范围为0至65535。

- TIM_ClockDivision： 时钟分频，设置定时器时钟CK_INT频率与死区发生器以及数字滤波器采样时钟频率分频比。可以选择1、2、4分频。

- TIM_RepetitionCounter： 重复计数器，只有8位，只存在于高级定时器。

### 5.2 TIM_OCInitTypeDef

输出比较结构体TIM_OCInitTypeDef用于输出比较模式，与TIM_OCxInit函数配合使用完成指定定时器输出通道初始化配置。高级控制定时器有四个定时器通道，使用时都必须单独设置。

```c
typedef struct {
    uint16_t TIM_OCMode;       // 比较输出模式
    uint16_t TIM_OutputState;  // 比较输出使能
    uint16_t TIM_OutputNState; // 比较互补输出使能
    uint32_t TIM_Pulse;        // 脉冲宽度
    uint16_t TIM_OCPolarity;   // 输出极性
    uint16_t TIM_OCNPolarity;  // 互补输出极性
    uint16_t TIM_OCIdleState;  // 空闲状态下比较输出状态
    uint16_t TIM_OCNIdleState; // 空闲状态下比较互补输出状态
} TIM_OCInitTypeDef;
```

- TIM_OCMode： 比较输出模式选择，总共有八种，常用的为PWM1/PWM2。它设定CCMRx寄存器OCxM[2:0]位的值。

- TIM_OutputState： 比较输出使能，决定最终的输出比较信号OCx是否通过外部引脚输出。它设定TIMx_CCER寄存器CCxE/CCxNE位的值。

- TIM_OutputNState: 比较互补输出使能，决定OCx的互补信号OCxN是否通过外部引脚输出。它设定CCER寄存器CCxNE位的值。

- TIM_Pulse： 比较输出脉冲宽度，实际设定比较寄存器CCR的值，决定脉冲宽度。可设置范围为0至65535。

- TIM_OCPolarity： 比较输出极性，可选OCx为高电平有效或低电平有效。它决定着定时器通道有效电平。它设定CCER寄存器的CCxP位的值。

- TIM_OCNPolarity： 比较互补输出极性，可选OCxN为高电平有效或低电平有效。它设定TIMx_CCER寄存器的CCxNP位的值。

- TIM_OCIdleState： 空闲状态时通道输出电平设置，可选输出1或输出0，即在空闲状态(BDTR_MOE位为0)时，经过死区时间后定时器通道输出高电平或低电平。它设定CR2寄存器的OISx位的值。

- TIM_OCNIdleState： 空闲状态时互补通道输出电平设置，可选输出1或输出0，即在空闲状态(BDTR_MOE位为0)时，经过死区时间后定时器互补通道输出高电平或低电平， 设定值必须与TIM_OCIdleState相反。它设定是CR2寄存器的OISxN位的值。

### 5.3 TIM_ICInitTypeDef

输入捕获结构体TIM_ICInitTypeDef用于输入捕获模式，与TIM_ICInit函数配合使用完成定时器输入通道初始化配置。 如果使用PWM输入模式需要与TIM_PWMIConfig函数配合使用完成定时器输入通道初始化配置。

```c
typedef struct {
    uint16_t TIM_Channel;     // 输入通道选择
    uint16_t TIM_ICPolarity;  // 输入捕获触发选择
    uint16_t TIM_ICSelection; // 输入捕获选择
    uint16_t TIM_ICPrescaler; // 输入捕获预分频器
    uint16_t TIM_ICFilter;    // 输入捕获滤波器
} TIM_ICInitTypeDef;
```

- TIM_Channel： 捕获通道ICx选择，可选TIM_Channel_1、 TIM_Channel_2、TIM_Channel_3或TIM_Channel_4四个通道。它设定CCMRx寄存器CCxS位 的值。

- TIM_ICPolarity： 输入捕获边沿触发选择，可选上升沿触发、 下降沿触发或边沿跳变触发。它设定CCER寄存器CCxP位和CCxNP位的值。

- TIM_ICSelection： 输入通道选择，捕获通道ICx的信号可来自三个输入通道，分别为TIM_ICSelection_DirectTI、 TIM_ICSelection_IndirectTI或TIM_ICSelection_TRC，具体的区别见图，如果是普通的输入捕获，4个通道都可以使用，如果是PWM输入则只能使用通道1和通道2。它设定CCRMx寄存器的CCxS[1:0]位的值。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/TIMsen016.png)

- TIM_ICPrescaler： 输入捕获通道预分频器，可设置1、2、4、8分频，它设定CCMRx寄存器的ICxPSC[1:0]位的值。如果需要捕获输入信号的每个有效边沿，则设置1分频即可。

- TIM_ICFilter： 输入捕获滤波器设置，可选设置0x0至0x0F。它设定CCMRx寄存器ICxF[3:0]位的值。一般我们不使用滤波器，即设置为0。

### 5.4 TIM_BDTRInitTypeDef

断路和死区结构体TIM_BDTRInitTypeDef用于断路和死区参数的设置，属于高级定时器专用，用于配置断路时通道输出状态，以及死区时间。 它与TIM_BDTRConfig函数配置使用完成参数配置。这个结构体的成员只对应BDTR这个寄存器，有关成员的具体使用配置请参考手册BDTR寄存器的详细描述。

```c
typedef struct {
    uint16_t TIM_OSSRState;       // 运行模式下的关闭状态选择
    uint16_t TIM_OSSIState;       // 空闲模式下的关闭状态选择
    uint16_t TIM_LOCKLevel;       // 锁定配置
    uint16_t TIM_DeadTime;        // 死区时间
    uint16_t TIM_Break;           // 断路输入使能控制
    uint16_t TIM_BreakPolarity;   // 断路输入极性
    uint16_t TIM_AutomaticOutput; // 自动输出使能
} TIM_BDTRInitTypeDef;
```

- TIM_OSSRState： 运行模式下的关闭状态选择，它设定BDTR寄存器OSSR位的值。

- TIM_OSSIState： 空闲模式下的关闭状态选择，它设定BDTR寄存器OSSI位的值。

- TIM_LOCKLevel： 锁定级别配置， BDTR寄存器LOCK[1:0]位的值。

- TIM_DeadTime： 配置死区发生器，定义死区持续时间，可选设置范围为0x0至0xFF。它设定BDTR寄存器DTG[7:0]位的值。

- TIM_Break： 断路输入功能选择，可选使能或禁止。它设定BDTR寄存器BKE位的值。

- TIM_BreakPolarity： 断路输入通道BRK极性选择，可选高电平有效或低电平有效。它设定BDTR寄存器BKP位的值。

- TIM_AutomaticOutput： 自动输出使能，可选使能或禁止，它设定BDTR寄存器AOE位的值。

## 6. 三种定时器之区别（高级、通用、基本）

### 1. **高级定时器 (Advanced-control Timers)**

**示例**：TIM1、TIM8（在STM32F103中）

**功能特点**：

- **复杂PWM生成**：支持中心对齐PWM模式和边沿对齐PWM模式，并且可以生成死区时间（Dead-time），这是驱动电机或H桥电路时保护功率开关的重要功能。
- **高级控制功能**：支持复杂的输出比较配置、单脉冲模式、重复计数模式等，适用于需要复杂控制信号的场合。
- **输入捕获和输出比较**：提供多通道输入捕获和输出比较功能，适用于高精度的时间测量和复杂的信号调节。
- **同步和触发功能**：支持多个定时器之间的同步操作，能够实现复杂的触发和同步功能。
- **死区时间生成**：能够生成死区时间，用于防止功率转换器的两个开关同时导通，避免短路。

**应用场景**：

- 电机控制（如三相电机的PWM驱动）
- 高精度的时间测量和事件捕获
- 高级的波形生成和控制

### 2. **通用定时器 (General-purpose Timers)**

**示例**：TIM2、TIM3、TIM4（在STM32F103中）

**功能特点**：

- **PWM生成**：支持边沿对齐PWM模式，用于简单的频率和占空比调节。
- **计数功能**：可以作为基本的计数器，支持计数、定时和延迟功能。
- **输入捕获和输出比较**：提供基本的输入捕获和输出比较功能，适用于标准的时间测量和简单的事件处理。
- **同步功能**：支持与其他定时器进行基本的同步操作，但不如高级定时器复杂。

**应用场景**：

- 基本的PWM信号生成（如LED调光）
- 基本的计时和延迟
- 简单的时间测量和事件捕获

### 3. **基本定时器 (Basic Timers)**

**示例**：TIM6、TIM7（在STM32F103中）

**功能特点**：

- **计数功能**：主要用于计数和生成定时中断，不具备PWM生成和复杂的输出比较功能。
- **简单的操作**：配置和操作相对简单，适用于基本的时间延迟和定时任务。
- **不支持输入捕获和输出比较**：不具备输入捕获和输出比较功能，因此不适用于需要复杂时间处理的场合。

**应用场景**：

- 简单的定时器任务（如定时中断）
- 基本的延迟生成
- 简单的定时事件

### 总结

- **高级定时器**：功能强大，适用于需要复杂PWM生成、精确同步和高级控制功能的应用。适合电机控制、高级波形生成等。
- **通用定时器**：提供标准的PWM生成和计数功能，适用于较为简单的时间控制和信号生成任务。
- **基本定时器**：用于简单的计时和定时任务，功能有限，适合基础的延迟和计时操作。

## 7. 理解死区

死区时间（Dead-Time）是指在一个PWM信号的高电平和低电平之间插入的一段时间间隔。在驱动电机或其他功率转换器（如H桥电路）时，死区时间是非常重要的，因为它可以防止两个开关同时导通，从而避免短路和潜在的设备损坏。

### **为什么需要死区时间？**

在很多功率转换器设计中，特别是在H桥驱动电机的场合，通常会用到四个开关（晶体管或MOSFET）来控制电流的流动方向。如果这四个开关同时打开或关闭，会产生短路，损坏电路或电源。因此，通常在两个开关切换状态之间需要插入一段时间间隔，即死区时间，以确保两个开关不会同时导通。

### **如何实现死区时间？**

在STM32系列微控制器中，死区时间通常由高级定时器（如TIM1、TIM8）提供的功能来实现。以下是一个如何在STM32中配置死区时间的示例代码：

#### **示例代码：STM32高级定时器配置死区时间**

下面的代码片段是使用STM32 HAL库配置TIM1定时器以生成PWM信号并设置死区时间。假设你已经配置了HAL库和相关的STM32 HAL驱动。

```c
#include "stm32f4xx_hal.h"

// 定时器句柄
TIM_HandleTypeDef htim1;

void MX_TIM1_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    // 定时器基本配置
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 1999;  // PWM周期
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(&htim1);

    // 配置PWM通道
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 999;  // PWM占空比
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);

    // 配置死区时间
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 50;  // 死区时间设置（单位：时钟周期）
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
    HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);

    // 配置定时器主模式
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);

    // 启动PWM
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
}
```

### **代码解析**

1. **定时器初始化**：`MX_TIM1_Init` 函数初始化了TIM1定时器，设置了基本的PWM配置，包括计数器模式、周期、占空比等。

2. **PWM配置**：使用 `HAL_TIM_PWM_ConfigChannel` 函数配置PWM信号的模式和占空比。

3. **死区时间配置**：`TIM_BreakDeadTimeConfigTypeDef` 结构体用于配置死区时间。`DeadTime` 字段指定了死区时间的长度（单位是时钟周期）。

4. **主模式配置**：使用 `HAL_TIMEx_MasterConfigSynchronization` 函数配置定时器的主模式。

5. **启动PWM**：通过 `HAL_TIM_PWM_Start` 函数启动PWM信号。

---

2024.9.13 第一次修订，后期不再维护
