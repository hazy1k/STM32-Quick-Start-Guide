# 第十六章 USMART调试组件

## 1. USMART调试组件

USMART 是由 ALIENTEK 开发的一个灵巧的串口调试互交组件，通过它你可以通过串口助手调用程序里面的任何函数，并执行。因此，你可以随意更改函数的输入参数（支持数字（10/16进制， 支持负数)、字符串、函数入口地址等作为参数），单个函数最多支持 10 个输入参数，并支持函数返回值显示

USMART 的特点如下：

- 可以调用绝大部分用户直接编写的函数

- 资源占用极少（最少情况： FLASH:4K； SRAM:72B）

- 支持参数类型多（数字（包含 10/16 进制，支持负数）、字符串、函数指针等）

- 支持函数返回值显示

- 支持参数及返回值格式设置

- 支持函数执行时间计算

有了 USMART，你可以轻易的修改函数参数、查看函数运行结果，从而快速解决问题。比如你调试一个摄像头模块，需要修改其中的几个参数来得到最佳的效果，普通的做法：写函数→修改参数→下载→看结果→不满意→修改参数→下载→看结果→不满意….不停的循环，直到满意为止。这样做很麻烦不说，单片机也是有寿命的啊，老这样不停的刷，很折寿的。而利用USMART，则只需要在串口调试助手里面输入函数及参数，然后直接串口发送给单片机，就执行了一次参数调整，不满意的话，你在串口调试助手修改参数在发送就可以了，直到你满意为止。这样，修改参数十分方便，不需要编译、不需要下载、不会让单片机折寿。

USMART 支持的参数类型基本满足任何调试了，支持的类型有： 10 或者 16 进制数字、字符串指针（如果该参数是用作参数返回的话，可能会有问题！）、函数指针等。因此绝大部分函数，可以直接被 USMART 调用，对于不能直接调用的，你只需要重写一个函数，把影响调用的参数去掉即可，这个重写后的函数，即可以被 USMART 调用了。

USMART 的实现流程简单概括就是：第一步，添加需要调用的函数（在 usmart_config.c 里面的 usmart_nametab 数组里面添加）；第二步，初始化串口；第三步，初始化 USMART（通过usmart_init 函数实现）；第四步，轮询 usmart_scan 函数，处理串口数据。

经过以上简单介绍，我们对 USMART 有了个大概了解，接下来我们来简单介绍下USMART 组件的移植。

## 2. USMART文件详解

USMART 组件总共包含 6 文件如图所示：

![屏幕截图 2024 09 23 182704](https://img.picgo.net/2024/09/23/-2024-09-23-1827048cd518c56697267c.png)

其中 redeme.txt 是一个说明文件，不参与编译。其他五个文件， usmart.c 负责与外部互交等。usmat_str.c 主要负责命令和参数解析。 usmart_config.c 主要由用户添加需要由 usmart 管理的函数

usmart.h 和 usmart_str.h 是两个头文件，其中 usmart.h 里面含有几个用户配置宏定义，可以用来配置 usmart 的功能及总参数长度(直接和 SRAM 占用挂钩)、是否使能定时器扫描、是否使用读写函数等。

USMART 的移植，只需要实现 5 个函数。其中 4 个函数都在 usmart.c 里面，另外一个是串口接收函数，必须由用户自己实现，用于接收串口发送过来的数据。

### 2.1 串口接收函数

第一个函数，串口接收函数。该函数，我们是通过 SYSTEM 文件夹默认的串口接收来实现的，SYSTEM 文件夹里面的串口接收函数，最大可以一次接收 200 字节，用于从串口接收函数名和参数等。大家如果在其他平台移植，请参考 SYSTEM 文件夹串口接收的实现方式进行移植。

### 2.2 usmart_init()函数

第二个是 void usmart_init(void)函数，该函数的实现代码如下：

```c
// 初始化串口控制器
// sysclk:系统时钟（Mhz）
void usmart_init(u8 sysclk)
{
#if USMART_ENTIMX_SCAN == 1
    Timer4_Init(1000, (u32)sysclk*100-1); // 分频,时钟为 10K ,100ms 中断一次,注意,计数频率必须为 10Khz,以和 runtime 单位(0.1ms)同步.
#endif
    usmart_dev.sptype=1; // 十六进制显示参数
}
```

该函数有一个参数 sysclk，就是用于定时器初始化。另外 USMART_ENTIMX_SCAN 是在usmart.h 里面定义的一个是否使能定时器中断扫描的宏定义。如果为 1，就初始化定时器中断，并在中断里面调用 usmart_scan 函数。如果为 0，那么需要用户需要自行间隔一定时间（100ms左右为宜）调用一次 usmart_scan 函数，以实现串口数据处理。 注意：如果要使用函数执行时间统计功能（runtime 1）， 则必须设置 USMART_ENTIMX_SCAN 为 1。 另外，为了让统计时间精确到 0.1ms，定时器的计数时钟频率必须设置为 10Khz，否则时间就不是 0.1ms 了。

### 2.3 时间统计函数

第三和第四个函数仅用于服务 USMART 的函数执行时间统计功能（串口指令： runtime 1），分别是： usmart_reset_runtime 和 usmart_get_runtime，这两个函数代码如下：

```c
// 复位 runtime
// 需要根据所移植到的 MCU 的定时器参数进行修改
void usmart_reset_runtime(void)
{
    __HAL_TIM_CLEAR_FLAG(&TIM4_Handler, TIM_FLAG_UPDATE); // 清除中断标志位
    __HAL_TIM_SET_AUTORELOAD(&TIM4_Handler, 0XFFFF); // 将重装载值设置到最大
    __HAL_TIM_SET_COUNTER(&TIM4_Handler, 0); // 清空定时器的 CNT
    usmart_dev.runtime = 0;
} 
// 获得 runtime 时间
// 返回值:执行时间,单位:0.1ms,最大延时时间为定时器 CNT 值的 2 倍*0.1ms
// 需要根据所移植到的 MCU 的定时器参数进行修改
u32 usmart_get_runtime(void)
{
    if(__HAL_TIM_GET_FLAG(&TIM4_Handler,TIM_FLAG_UPDATE) == SET) // 在运行期间,产生了定时器溢出
    {
        usmart_dev.runtime += 0XFFFF;
    }
    usmart_dev.runtime += __HAL_TIM_GET_COUNTER(&TIM4_Handler);
    return usmart_dev.runtime; // 返回计数值
}
```

这里我们利用定时器 4 来做执行时间计算， usmart_reset_runtime 函数在每次 USMART 调用函数之前执行，清除计数器，然后在函数执行完之后，调用 usmart_get_runtime 获取整个函数的运行时间。由于 usmart 调用的函数，都是在中断里面执行的，所以我们不太方便再用定时器的中断功能来实现定时器溢出统计，因此， USMART 的函数执行时间统计功能，最多可以统计定时器溢出 1 次的时间，对 STM32F4 的定时器 4，该定时器是 16 位的，最大计数是 65535，而由于我们定时器设置的是 0.1ms 一个计时周期（ 10Khz），所以最长计时时间是： 65535*2*0.1ms=13.1 秒。也就是说，如果函数执行时间超过 13.1 秒，那么计时将不准确。

### 2.4 扫描函数

最后一个是 usmart_scan 函数，该函数用于执行 usmart 扫描，该函数需要得到两个参量，第一个是从串口接收到的数组（USART_RX_BUF），第二个是串口接收状态（USART_RX_STA）。接收状态包括接收到的数组大小，以及接收是否完成。该函数代码如下：

```c
// usmart 扫描函数
// 通过调用该函数,实现 usmart 的各个控制.该函数需要每隔一定时间被调用一次
// 以及时执行从串口发过来的各个函数.
// 本函数可以在中断里面调用,从而实现自动管理.
// 非 ALIENTEK 开发板用户,则 USART_RX_STA 和 USART_RX_BUF[]需要用户自己实现
void usmart_scan(void)
{
    u8 sta, len;
    if(USART_RX_STA&0x8000) // 串口接收完成？
    {
        len = USART_RX_STA&0x3fff; // 得到此次接收到的数据长度
        USART_RX_BUF[len]='\0';    // 在末尾加入结束符.
        sta = usmart_dev.cmd_rec(USART_RX_BUF); // 得到函数各个信息
        if(sta == 0)
            usmart_dev.exe(); //执行函数
        else
        {
            len = usmart_sys_cmd_exe(USART_RX_BUF);
            if(len != USMART_FUNCERR)
                sta = len;
            if(sta)
            {
                switch(sta)
                {
                    case USMART_FUNCERR:
                        printf("函数错误!\r\n");
                        break;
                    case USMART_PARMERR:
                        printf("参数错误!\r\n");
                        break;
                    case USMART_PARMOVER:
                        printf("参数太多!\r\n");
                        break;
                    case USMART_NOFUNCFIND:
                        printf("未找到匹配的函数!\r\n");
                        break;
                }
            }
        }
        USART_RX_STA = 0; // 状态寄存器清空
    }
}
```

该函数的执行过程：先判断串口接收是否完成（USART_RX_STA 的最高位是否为 1），如果完成，则取得串口接收到的数据长度（USART_RX_STA 的低 14 位），并在末尾增加结束符，再执行解析，解析完之后清空接收标记（USART_RX_STA 置零）。如果没执行完成，则直接跳过，不进行任何处理。

完成这几个函数的移植，你就可以使用 USMART 了。不过，需要注意的是， usmart 同外部的互交，一般是通过 usmart_dev 结构体实现，所以 usmart_init 和 usmart_scan 的调用分别是通过： usmart_dev.init 和 usmart_dev.scan 实现的。

---

2024.10.8 第一次修订
