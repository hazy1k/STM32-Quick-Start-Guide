# 第0章 新建HAL库工程模板

## 1. 新建工程模板

在建立工程之前，我们建议用户在电脑的某个目录下面建立一个文件夹，后面所建立的工程都可以放在这个文件夹下面，新建好的目录结构如下图：

![屏幕截图 2024-12-17 200137.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/12/17-20-01-52-屏幕截图%202024-12-17%20200137.png)

接下来，打开 MDK，点击菜单 Project –>New Uvision Project ，然后将目录定位到刚才建立的文件夹 Template 之下的 USER 子目录， 工程取名为 Template 之后点击保存， 工程文件就都保存到 USER 文件夹下面。

![屏幕截图 2024-12-17 200217.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/12/17-20-02-23-屏幕截图%202024-12-17%20200217.png)

接下来会出现一个选择 Device 的界面，就是选择我们的芯片型号，这里我们定位到STMicroelectronics下面的STM32F407ZGT6 (针对我们的正点原子探索者STM32F4板子是这个型号)。

![屏幕截图 2024-12-17 200315.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/12/17-20-03-20-屏幕截图%202024-12-17%20200315.png)

现在我们看看 USER 目录下面内容：

![](C:\Users\qiu\AppData\Roaming\marktext\images\2024-08-28-16-41-50-image.png)

这里我们说明一下， Template.uvprojx 是工程文件，非常关键，不能轻易删除， MDK5.20生成的工程文件是以.uvprojx 为后缀。 DebugConfig， Listings 和 Objects 三个文件夹是 MDK 自动生成的文件夹。其中 DebugConfig 文件夹用于存储一些调试配置文件， Listings 和 Objects 文件夹用来存储 MDK 编译过程的一些中间文件。这里，我们把 Listings 和 Objects 文件夹删除，我们会在下一步骤中新建一个 OBJ 文件夹，用来存放编译中间文件。当然，我们不删除这两个文件夹也没有关系，只是我们不用它而已。

接下来我们将从官方 stm32cubeF4 包里面复制一些我们新建工程需要的关键文件到我们的工程目录中。这个内容可以参考下面的链接，因为前人写得很详细了，我就不在这凑字数了：

[10. 新建工程—库函数版 — [野火]STM32 HAL库开发实战指南——基于野火F4系列开发板 文档](https://doc.embedfire.com/mcu/stm32/f4/hal_general/zh/latest/doc/chapter11/chapter11.html)

接下来，我们还需要复制 ALIENTEK 编写的 SYSTEM 文件夹内容到工程目录中。首先，我们需要解释一下，这个SYSTEM 文件夹内容是 ALIENTEK 为开发板用户编写的一套非常实用的函数库，比如系统时钟初始化，串口打印，延时函数等，大家也可以根据自己需求决定是否需要 SYSTEM 文件夹，对于 STM32F407 的工程模板，如果没有加入 SYSTEM 文件夹，那么大家需要自己定义系统时钟初始化。

![屏幕截图 2024-12-17 200824.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/12/17-20-08-35-屏幕截图%202024-12-17%20200824.png)

到这里，工程模板所需要的所有文件都已经复制进去。接下来，我们将在 MDK 中将这些文件添加到工程。

## 2. 使用工具新建模板

### 2.1. 安装工具链

确保你已经安装了STM32CubeIDE或STM32CubeMX和相应的编译工具链。

### 2.2 创建新工程

1. **打开STM32CubeIDE**。
2. 选择 **File -> New -> STM32 Project**。
3. 在弹出的窗口中，选择你的STM32芯片或开发板，点击 **Next**。

### 2.3 配置项目

1. 输入项目名称和选择保存位置。
2. 选择工程类型为 **Generate under root** 或 **Generate in workspace**，根据需要选择。
3. 点击 **Finish**。

### 2.4 启用HAL库

1. 在打开的项目中，进入 **.ioc** 文件（STM32CubeMX配置界面）。
2. 在 **Pinout & Configuration** 视图中，选择需要使用的外设（如GPIO、USART、ADC等）。
3. 在 **Configuration** 面板中，确保选中 **HAL Drivers**。
4. 点击 **Project -> Settings**，确保选择 **Toolchain / IDE** 为 **STM32CubeIDE**。
5. 点击 **Project -> Generate Code**，生成HAL库的代码。

### 2.5 编写代码

1. 进入生成的 `Src` 和 `Inc` 文件夹，查看 `main.c` 和其他生成的文件。
2. 在 `main.c` 中编写主程序逻辑，使用HAL库函数进行外设操作。

### 2.6 编译和调试

1. 选择项目，点击 **Build** 按钮编译代码。
2. 将开发板连接到电脑，点击 **Run** 按钮下载并调试程序。

### 2.7 示例代码

以下是一个简单的示例代码，用于点亮LED：

```c
#include "main.h"

int main(void) {
    HAL_Init(); // 初始化HAL库
    SystemClock_Config(); // 配置系统时钟
    __HAL_RCC_GPIOA_CLK_ENABLE(); // 使能GPIOA时钟

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5; // 假设使用PA5引脚
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // 设置为推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 不使用上拉下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // 设置速度
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // 初始化GPIO

    while (1) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // 切换LED状态
        HAL_Delay(500); // 延时500毫秒
    }
}
```

更多关于STM32MX的使用教程：[STM32CubeMX系列教程 - STM32CubeMX系列教程 微雪课堂](https://www.waveshare.net/study/article-629-1.html?_dsign=8f9f177a)

---

2024.9.23 第一次修订，后期不再维护

2024.12.17 修补内容
