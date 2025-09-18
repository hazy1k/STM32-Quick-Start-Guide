# 第十四章 USMART调试组件实验

## 1. 硬件设计

本实验用到的硬件资源有：

- 指示灯 DS0 和 DS1 

- 串口

- TFTLCD 模块

这三个硬件在前面章节均有介绍，本章不再介绍。

## 2. 软件设计

### 2.1 移植USMART调试组件

我们打开工程，并新建 USMART 组，添加 USMART 组件代码，同时把 USMART文件夹添加到头文件包含路径，在主函数里面加入 include“usmart.h”

![屏幕截图 2024 10 08 154557](https://img.picgo.net/2024/10/08/-2024-10-08-154557a737aa177ffd03bb.png)

由于 USMART 默认提供了 STM32F4 的 TIM4 中断初始化设置代码，我们只需要在 usmart.h里面设置 USMART_ENTIMX_SCAN 为 1，即可完成 TIM4 的设置，通过 TIM4 的中断服务函数，调用 usmart_dev.scan()（就是 usmart_scan 函数），实现 usmart 的扫描。此部分代码我们就不列出来了，请参考 usmart.c。

### 2.2 添加要调用的函数

此时，我们就可以使用 USMART 了，不过在主程序里面还得执行 usmart 的初始化，另外还需要针对你自己想要被 USMART 调用的函数在 usmart_config.c 里面进行添加。下面先介绍如何添加自己想要被 USMART 调用的函数，打开 usmart_config.c

![屏幕截图 2024 10 08 154927](https://img.picgo.net/2024/10/08/-2024-10-08-1549275fd908397fcb9646.png)

这里的添加函数很简单，只要把函数所在头文件添加进来，并把函数名按上图所示的方式增加即可，默认我们添加了两个函数： delay_ms 和 delay_us。另外， read_addr 和 write_addr 属于 usmart 自带的函数，用于读写指定地址的数据，通过配置 USMART_USE_WRFUNS，可以使能或者禁止这两个函数。

这里我们根据自己的需要按上图的格式添加其他函数，添加完之后如图：

![屏幕截图 2024 10 08 155227](https://img.picgo.net/2024/10/08/-2024-10-08-1552277d42a3e68c5f317a.png)

上图中，我们添加了 lcd.h，并添加了很多 LCD 函数，最后我们还添加了 led_set 和 test_fun两个函数，这两个函数在 main.c 里面实现，代码如下：

```c
// LED 状态设置函数
void led_set(u8 sta)
{
    LED1 = sta;
}
// 函数参数调用测试函数
void test_fun(void(*ledset)(u8), u8 sta)
{
    ledset(sta);
}
```

led_set 函数，用于设置 LED1 的状态，而第二个函数 test_fun 则是测试 USMART 对函数参数的支持的， test_fun 的第一个参数是函数，在 USMART 里面也是可以被调用的。

### 2.3 主函数

在添加完函数之后，我们修改 main 函数，如下：

```c
// LED状态设置函数
void led_set(u8 sta)
{
    LED1=sta;
} 
//函数参数调用测试函数
void test_fun(void(*ledset)(u8),u8 sta)
{
    ledset(sta);
}     

int main(void)
{
    HAL_Init();                  // 初始化HAL库    
    Stm32_Clock_Init(336,8,2,7); // 设置时钟,168Mhz
    delay_init(168);             // 初始化延时函数
    uart_init(115200);           // 初始化USART
    LED_Init();                     // 初始化LED    
     LCD_Init();                    // 初始化LCD
    usmart_dev.init(84);          // 初始化USMART    
    POINT_COLOR = RED;              //画笔颜色：红色
    LCD_ShowString(30,50,200,16,16,"Explorer STM32F4");    
    LCD_ShowString(30,70,200,16,16,"USMART TEST");    
    LCD_ShowString(30,90,200,16,16,"ATOM@ALIENTEK");
    LCD_ShowString(30,110,200,16,16,"2024/10/08");       
      while(1) 
    {               
        LED0 = !LED0;                     
        delay_ms(500);
    }
}
```

此代码显示简单的信息后，就是在死循环等待串口数据。至此，整个 usmart 的移植就完成了。编译成功后，就可以下载程序到开发板，开始 USMART 的体验。

## 3. 串口测试使用

将程序下载到探索者 STM32F4 开发板后，可以看到 DS0 不停的闪烁，提示程序已经在运行了。 同时，屏幕上显示了一些字符（就是主函数里面要显示的字符）。

### 3.1 系统指令

我们打开串口调试助手 XCOM，选择正确的串口号→多条发送→勾选发送新行（即发送回车键）选项，然后发送 list 指令，即可打印所有 usmart 可调用函数。如下图所示：

![屏幕截图 2024 10 08 160907](https://img.picgo.net/2024/10/08/-2024-10-08-160907870f3b222422fb22.png)

上图中 list、 id、？、 help、 hex、 dec 和 runtime 都属于 usmart 自带的系统命令。下面我们简单介绍下这几个命令：

- list，该命令用于打印所有 usmart 可调用函数。发送该命令后，串口将受到所有能被 usmart调用得到函数

- id，该指令用于获取各个函数的入口地址。比如前面写的 test_fun 函数，就有一个函数参数，我们需要先通过 id 指令，获取 led_set 函数的 id（即入口地址），然后将这个 id 作为函数参数，传递给 test_fun。

- help（或者‘ ？’也可以），发送该指令后，串口将打印 usmart 使用的帮助信息。

- hex 和 dec，这两个指令可以带参数，也可以不带参数。当不带参数的时候， hex 和 dec 分别用于设置串口显示数据格式为 16 进制/10 进制。当带参数的时候， hex 和 dec 就执行进制转换，比如输入： hex 1234，串口将打印： HEX:0X4D2，也就是将 1234 转换为 16 进制打印出来。又比如输入： dec 0X1234，串口将打印： DEC:4660，就是将 0X1234 转换为 10 进制打印出来。

- runtime 指令，用于函数执行时间统计功能的开启和关闭，发送： runtime 1，可以开启函数执行时间统计功能；发送： runtime 0，可以关闭函数执行时间统计功能。函数执行时间统计功能，默认是关闭的。

### 3.2 使用举例

LCD_ShowString 函数，该函数用于显示字符串，我们通过串口输入： LCD_ShowString(20,200,200,100,16,"This is a test for usmart!!")

![屏幕截图 2024 10 08 161108](https://img.picgo.net/2024/10/08/-2024-10-08-1611084bdff55a3f5b5bd8.png)

该函数用于在指定区域，显示指定字符串，发送给开发板后，我们可以看到 LCD 在我们指定的地方显示了： This is a test for usmart!! 这个字符串。

其他函数的调用，也都是一样的方法，这里我们就不多介绍了，最后说一下带有函数参数的函数的调用。我们将 led_set 函数作为 test_fun 的参数，通过在 test_fun 里面调用 led_set 函数，实现对 DS1(LED1)的控制。前面说过，我们要调用带有函数参数的函数，就必须先得到函数参数的入口地址（id），通过输入 id 指令，我们可以得到 led_set 的函数入口地址是： 0X080052C9，所以，我们在串口输入： test_fun(0X080052C9,0)，就可以控制 DS1 亮了。

![屏幕截图 2024 10 08 161141](https://img.picgo.net/2024/10/08/-2024-10-08-1611419bb5de30d3b930cb.png)
