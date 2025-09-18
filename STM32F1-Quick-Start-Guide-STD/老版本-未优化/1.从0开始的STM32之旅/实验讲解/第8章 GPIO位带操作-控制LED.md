# 第八章 GPIO-位带操作

## 1. 位带简介

    位操作就是单独的对一个比特位读和写，在51单片机里我们见到过很多次了（sbit实现位定义，比如sbit LED1 = P2^0啦）但是STM32没有这样的关键字，所以通过访问位带别名区来实现。

    在STM32中，有两个地方实现了位带，一个是SRAM区的最低1MB空间，另一个是外设区最低1MB空间。 这两个1MB的空间除了可以像正常的RAM一样操作外，他们还有自己的位带别名区，位带别名区把这1MB的空间的每一个位膨胀成一个32位的字， 当访问位带别名区的这些字时，就可以达到访问位带区某个比特位的目的。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/GPIObi002.png)

### 1.1 外设位带区

    外设外带区的地址为：0X40000000~0X40100000，大小为1MB，这1MB的大小在103系列大/中/小容量型号的单片机中包含了片上外设的全部寄存器， 这些寄存器的地址为：0X40000000~0X40029FFF。外设位带区经过膨胀后的位带别名区地址为：0X42000000~0X43FFFFFF， 这个地址仍然在CM3 片上外设的地址空间中。在103系列单片机里面，0X40030000~0X4FFFFFFF属于保留地址， 膨胀后的32MB位带别名区刚好就落到这个地址范围内，不会跟片上外设的其他寄存器地址重合。

    STM32的全部寄存器都可以通过访问位带别名区的方式来达到访问原始寄存器比特位的效果，这比51单片机强大很多。 因为51单片机里面并不是所有的寄存器都是可以比特位操作，有些寄存器还是得字节操作，比如SBUF。

    更多关于外设位带区：[STM32学习——位带区的理解-CSDN博客](https://blog.csdn.net/weixin_44586750/article/details/97370963)

    [STM32位带操作-详解-计算过程_位带别名区地址计算0x20000100-CSDN博客](https://blog.csdn.net/weixin_46897073/article/details/110727064#:~:text=%E9%A6%96%E5%85%88%E6%88%91%E4%BB%AC%E8%A6%81%E4%BA%86%E8%A7%A3%E5%9C%A8STM32%E4%B8%AD%E6%9C%89%E4%B8%A4%E4%B8%AA%E5%8C%BA%E5%9F%9F%E5%8F%AF%E4%BB%A5%E8%BF%9B%E8%A1%8C%E4%BD%8D%E5%B8%A6%E6%93%8D%E4%BD%9C%EF%BC%8C%E6%94%AF%E6%8C%81%E4%BD%8D%E5%B8%A6%E6%93%8D%E4%BD%9C%E7%9A%84%E4%B8%A4%E4%B8%AA%E5%86%85%E5%AD%98%E5%8C%BA%E7%9A%84%E8%8C%83%E5%9B%B4%E6%98%AF%3A%200x2000%200000-0x200F%20FFFF,%28SRAM%E5%8C%BA%E4%B8%AD%E7%9A%84%E6%9C%80%E4%BD%8E1MB%29%200x4000%200000-0x400F%20FFFF%20%28%E7%89%87%E4%B8%8A%E5%A4%96%E8%AE%BE%E5%8C%BA%E4%B8%AD%E7%9A%84%E6%9C%80%E4%BD%8E1MB%29)

### 1.2 SRAM位带区

    SRAM的位带区的地址为：0X2000 0000~X2010 0000，大小为1MB，经过膨胀后的位带别名区地址为：0X2200 0000~0X23FF FFFF， 大小为32MB。操作SRAM的比特位这个用得很少。

    更多可以参考：[stm32位段操作SRAM_#define bitband(addr, bitnum) ((addr & 0xf0000000)-CSDN博客](https://blog.csdn.net/Andemund/article/details/95202579)

### 1.3 位带区和位带别名区地址转换

    位带区的一个比特位经过膨胀之后，虽然变大到4个字节，但是还是LSB才有效。有人会问这不是浪费空间吗， 要知道STM32的系统总线是32位的，按照4个字节访问的时候是最快的，所以膨胀成4个字节来访问是最高效的。

    我们可以通过指针的形式访问位带别名区地址从而达到操作位带区比特位的效果。那这两个地址直接如何转换，我们简单介绍一下。

#### 1.3.1 外设位带别名区地址

    对于片上外设位带区的某个比特，记它所在字节的地址为 A,位序号为 n(0<=n<=31)(n的范围根据具体寄存器能控制的位决定)，则该比特在别名区的地址为：

```c
AliasAddr= =0x42000000+ (A-0x40000000)*8*4 +n*4
```

    0X42000000是外设位带别名区的起始地址，0x40000000是外设位带区的起始地址，（A-0x40000000）表示该比特前面有多少个字节， 一个字节有8位，所以*8，一个位膨胀后是4个字节，所以*4，n表示该比特在A地址的序号，因为一个位经过膨胀后是四个字节，所以也*4。

#### 1.3.2 SRAM位带别名区地址

    对于SRAM位带区的某个比特，记它所在字节的地址为 A,位序号为 n(0<=n<=31)(n的范围根据具体寄存器能控制的位决定)，则该比特在别名区的地址为：

```c
AliasAddr= =0x22000000+ (A-0x20000000)*8*4 +n*4
```

    公式分析同上。

#### 1.3.3 统一公式

    为了方便操作，我们可以把这两个公式合并成一个公式，把“位带地址+位序号”转换成别名区地址统一成一个宏。

```c
// 把“位带地址+位序号”转换成别名地址的宏
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x02000000+((addr & 0x00FFFFFF)<<5)+(bitnum<<2))
```

    addr & 0xF0000000是为了区别SRAM还是外设，实际效果就是取出4或者2，如果是外设， 则取出的是4，+0X02000000之后就等于0X42000000，0X42000000是外设别名区的起始地址。 如果是SRAM，则取出的是2，+0X02000000之后就等于0X22000000，0X22000000是SRAM别名区的起始地址。

    addr & 0x00FFFFFF 屏蔽了高三位，相当于减去0X20000000或者0X40000000，但是为什么是屏蔽高三位？ 因为外设的最高地址是：0X20100000，跟起始地址0X20000000相减的时候，总是低5位才有效， 所以干脆就把高三位屏蔽掉来达到减去起始地址的效果，具体屏蔽掉多少位跟最高地址有关。SRAM同理分析即可。 <<5相当于8x4，<<2相当于x4，这两个我们在上面分析过。

    最后我们就可以通过指针的形式操作这些位带别名区地址，最终实现位带区的比特位操作。

```c
// 把一个地址转换成一个指针
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))

// 把位带别名区地址转换成指针
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
```

## 2. 软件设计

    外设的位带区，覆盖了全部的片上外设的寄存器，我们可以通过宏为每个寄存器的位都定义一个位带别名地址，从而实现位操作。 但这个在实际项目中不是很现实，也很少人会这么做，我们在这里仅仅演示下GPIO中ODR和IDR这两个寄存器的位操作。

    从手册中我们可以知道ODR和IDR这两个寄存器对应GPIO基址的偏移是12和8，我们先实现这两个寄存器的地址映射，其中GPIOx_BASE在库函数里面有定义。

### 2.1 GPIO寄存器映射

```c
// 位带操作-1 GPIO ODR 和 IDR 寄存器映射

// GPIO ODR 和 IDR 寄存器地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) // 0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) // 0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) // 0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) // 0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) // 0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) // 0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) // 0x40011E0C

#define GPIOA_IDR_Addr    (GPIOA_BASE+8)  // 0x40010808
#define GPIOB_IDR_Addr    (GPIOB_BASE+8)  // 0x40010C08
#define GPIOC_IDR_Addr    (GPIOC_BASE+8)  // 0x40011008
#define GPIOD_IDR_Addr    (GPIOD_BASE+8)  // 0x40011408
#define GPIOE_IDR_Addr    (GPIOE_BASE+8)  // 0x40011808
#define GPIOF_IDR_Addr    (GPIOF_BASE+8)  // 0x40011A08
#define GPIOG_IDR_Addr    (GPIOG_BASE+8)  // 0x40011E08
```

    现在我们就可以用位操作的方法来控制GPIO的输入和输出了，其中宏参数n表示具体是哪一个IO口，n(0,1,2…15)。 这里面包含了端口A~G ，并不是每个单片机型号都有这么多端口，使用这部分代码时，要查看你的单片机型号，如果是64pin的则最多只能使用到C端口。

### 2.2 GPIO位操作

```c
// 位带操作-2 GPIO 输入输出位操作

// 单独操作 GPIO的某一个IO口，n(0,1,2...15),n表示具体是哪一个IO口
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr, n)  // 输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr, n)  // 输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr, n)  // 输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr, n)  // 输入

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr, n)  // 输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr, n)  // 输入

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr, n)  // 输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr, n)  // 输入

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr, n)  // 输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr, n)  // 输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr, n)  // 输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr, n)  // 输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr, n)  // 输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr, n)  // 输入
```

### 2.3 主函数

    该工程我们直接从LED-库函数 操作移植过来，有关LED GPIO 初始化和软件延时等函数我们直接用， 修改的是控制GPIO输出的部分改成了位操作。该实验我们让IO口输出高低电平来控制LED的亮灭，负逻辑点亮。具体使用哪一个IO和点亮方式由硬件平台决定。

```c
int main(void)
{
    LED_GPIO_Config(); // LED初始化

    while(1) 
    {
        // PB0 = 0,点亮LED
        PBout(0)= 0;
        SOFT_Delay(0x0FFFFF); // 延时

        // PB1 = 1,熄灭LED
        PBout(0)= 1;
        SOFT_Delay(0x0FFFFF); // 延时
    }
}
```

## 3. 小结

    学习过51单片机的应改了解，在控制51单片机IO引脚时，只需要向某一个IO口赋值就可以实现，对应IO口的输出高或低。举例：

```c
#include <reg52.h>

sbit LED = P1^3; // 定义LED连接到P1.3端口

void main() 
{
    LED = 1; // 设置P1.3引脚为高电平，点亮LED
    while (1); // 无限循环，使程序不退出
}
```

    可以看到在51中使用位操作还是比较简单的，类似宏定义。那么STM32可以不可以像51单片机那样，直接对引脚进行操作呢。答案是肯定的可以。这就用到了位带操作。

    位带操作简单的说，就是把每个比特膨胀为一个 32 位的字，当访问这些字的时候就达到了访问比特的目的，比如说 BSRR 寄存器有 32 个位，那么可以映射到 32 个地址上，我们去访问这 32 个地址就达到访问 32 个比特的目的。这样我们往某个地址写 1 就达到往对应比特位写 1 的目的，同样往某个地址写 0 就达到往对应的比特位写 0 的目的。

    那么到底怎么实现位带操作呢：

- 首先实现GPIO ODR 和 IDR 寄存器地址映射（根据基地址和偏移来写就行了）举例：

```c
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) // ODR这个寄存器的偏移量
```

- 实现位带操作，也就是单独操作GPIOx的某一个IO口，（n)代表的是GPIO的引脚举例：

```c
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr, n)  // 输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr, n)  // 输入

// 这里我们就可以操作GPIOA的IO口输入输出
```

- 实现位带操作后，我们知道GPIO就是输出高低电平实现功能，于是我们可以主函数内实现控制LED(1-高-灭，0-低-亮)

```c
// 点亮GPIOA控制的LED
PAout(0) = 0;
```

但是我们在stm32中一般不使用位带操作，因为我们有强大标准库和HAL库，而位带操作在51中就比较常见了

---

2024.7.23 第一次修订

2024.8.20 第二次修订，后期不再维护

2024.12.18 修补内容
