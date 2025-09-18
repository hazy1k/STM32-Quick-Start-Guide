# 第二十七章 FPU介绍及应用

## 1. FPU简介

FPU 即浮点运算单元（Float Point Unit）。浮点运算，对于定点 CPU（没有 FPU 的 CPU）来说必须要按照 IEEE-754 标准的算法来完成运算，是相当耗费时间的。而对于有 FPU 的 CPU来说，浮点运算则只是几条指令的事情，速度相当快。

STM32F407 属于 Cortex M4 架构，带有 32 位单精度硬件 FPU，支持浮点指令集，相对于Cortex M0 和 Cortex M3 等，高出数十倍甚至上百倍的运算性能。

STM32F407 硬件上要开启 FPU 是很简单的，通过一个叫：协处理器控制寄存器（CPACR）的寄存器设置即可开启 STM32F407 的硬件 FPU，该寄存器各位描述如图

![屏幕截图 2025-08-16 092944.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/16-09-29-51-屏幕截图%202025-08-16%20092944.png)

这里我们就是要设置 CP11 和 CP10 这 4 个位，复位后，这 4 个位的值都为 0，此时禁止访问协处理器（禁止了硬件 FPU），我们将这 4 个位都设置为 1，即可完全访问协处理器（开启硬件 FPU），此时便可以使用 STM32F407 内置的硬件 FPU 了。 CPACR 寄存器这 4 个位的设置，我们在 system_stm32f4xx.c 文件里面开启，代码如下：

```c
void SystemInit (void)
{    
    /* 省略部分代码 */
    /* FPU settings ------------------------------------------------------------*/
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
    SCB->CPACR |= ((3UL << (10*2))|(3UL << (11*2)));
    /* set CP10 and CP11 Full Access */
    #endif
    /* 省略部分代码 */
}
```

此部分代码是系统初始化函数的部分内容，功能就是设置 CPACR 寄存器的 20~23 位为 1，以开启 STM32F407 的硬件 FPU 功能。从程序可以看出，只要我们定义了全局宏定义标识符__FPU_PRESENT 以及__FPU_USED 为 1，那么就可以开启硬件 FPU。其中宏定义标识符__FPU_PRESENT 用来确定处理器是否带 FPU 功能，标识符__FPU_USED 用来确定是否开启FPU 功能。

实际上，因为 STM32F407 是带 FPU 功能的，所以在我们的 stm32f407xx.h 头文件里面，我们默认是定义了__FPU_PRESENT 为 1。大家可以打开文件搜索即可找到下面一行代码：

```c
#define __FPU_PRESENT 1U /* FPU present */
```

但是，仅仅只是说明处理器有 FPU 功能是不够的，我们还需要开启 FPU 功能。开启 FPU有两种方法，第一种是直接在头文件 stm32f407xx.h 中定义宏定义标识符__FPU_USED 的值为1。也可以直接在 MDK 编译器上面设置，我们在 MDK5 编译器里面，点击魔术棒按钮，然后在 Target选项卡里面，设置 Floating Point Hardware 为 Use Single Precision，

![屏幕截图 2025-08-16 093733.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/16-09-37-36-屏幕截图%202025-08-16%20093733.png)

经过这个设置，编译器会自动加入标识符__FPU_USED 为 1。这样遇到浮点运算就会使用硬件 FPU 相关指令，执行浮点运算，从而大大减少计算时间。

## 2. Julia 分形简介

Julia 分形即 Julia 集，它最早由法国数学家 Gaston Julia 发现，因此命名为 Julia（朱利亚）集。 Julia 集合的生成算法非常简单：对于复平面的每个点，我们计算一个定义序列的发散速度。该序列的 Julia 集计算公式为：

```c
zn+1 = zn2 + c
```

针对复平面的每个 x + i.y 点，我们用 c = cx + i.cy 计算该序列：

```c
xn+1 + i.yn+1 = xn2 - yn2 + 2.i.xn.yn + cx + i.cy
xn+1 = xn2 - yn2 + cx 且 yn+1 = 2.xn.yn + cy
```

一旦计算出的复值超出给定圆的范围（数值大小大于圆半径），序列便会发散，达到此限值时完成的迭代次数与该点相关。随后将该值转换为颜色，以图形方式显示复平面上各个点的分散速度。

经过给定的迭代次数后，若产生的复值保持在圆范围内，则计算过程停止，并且序列也不发散，本例程生成 Julia 分形图片的代码如下：

```c
#define ITERATION 128 /* 迭代次数 */
#define REAL_CONSTANT 0.285f /* 实部常量 */
#define IMG_CONSTANT 0.01f /* 虚部常量 */
/**
* @brief 产生 Julia 分形图形
* @param size_x : 屏幕 x 方向的尺寸
* @param size_y : 屏幕 y 方向的尺寸
* @param offset_x : 屏幕 x 方向的偏移
* @param offset_y : 屏幕 y 方向的偏移
* @param zoom : 缩放因子
* @retval 无
*/
void julia_generate_fpu(uint16_t size_x, uint16_t size_y, uint16_t offset_x,
uint16_t offset_y, uint16_t zoom)
{
    uint8_t i;
    uint16_t x, y;
    float tmp1, tmp2;
    float num_real, num_img;
    float radius;
    for (y = 0; y < size_y; y++)
    {
        for (x = 0; x < size_x; x++)
        {
            num_real = y - offset_y;
            num_real = num_real / zoom;
            num_img = x - offset_x;
            num_img = num_img / zoom;
            i = 0;
            radius = 0;
            while ((i < ITERATION - 1) && (radius < 4))
            {
                tmp1 = num_real * num_real;
                tmp2 = num_img * num_img;
                num_img = 2 * num_real * num_img + IMG_CONSTANT;
                num_real = tmp1 - tmp2 + REAL_CONSTANT;
                radius = tmp1 + tmp2;
                i++;
            }
            LCD->LCD_RAM = g_color_map[i]; /* 绘制到屏幕 */
        }
    }
}
```

这种算法非常有效地展示了 FPU 的优势：无需修改代码，只需在编译阶段激活或禁止FPU（在 MDK Code Generation 的 Float Point Hardware 选项里面设置： Single Precision /Not Used），即可测试使用硬件 FPU 和不使用硬件 FPU 的差距。

## 3. FPU应用示例

```c
#include "bsp_init.h"

uint8_t timeout;

/* FPU模式提示 */
#if __FPU_USED==1
#define SCORE_FPU_MODE                  "FPU On"
#else
#define SCORE_FPU_MODE                  "FPU Off"
#endif

#define     ITERATION           128     /* 迭代次数 */
#define     REAL_CONSTANT       0.285f  /* 实部常量 */
#define     IMG_CONSTANT        0.01f   /* 虚部常量 */

/* 颜色表 */
uint16_t g_color_map[ITERATION];

/* 缩放因子列表 */
const uint16_t zoom_ratio[] =
{
    120, 110, 100, 150, 200, 275, 350, 450,
    600, 800, 1000, 1200, 1500, 2000, 1500,
    1200, 1000, 800, 600, 450, 350, 275, 200,
    150, 100, 110,
};

/**
 * @brief       初始化颜色表
 * @param       clut     : 颜色表指针
 * @retval      无
 */
void julia_clut_init(uint16_t *clut)
{
    uint32_t i = 0x00;
    uint16_t  red = 0, green = 0, blue = 0;

    for (i = 0; i < ITERATION; i++) /* 产生颜色表 */
    {
        /* 产生RGB颜色值 */
        red = (i * 8 * 256 / ITERATION) % 256;
        green = (i * 6 * 256 / ITERATION) % 256;
        blue = (i * 4 * 256 / ITERATION) % 256;

        /* 将RGB888,转换为RGB565 */
        red = red >> 3;
        red = red << 11;
        green = green >> 2;
        green = green << 5;
        blue = blue >> 3;
        clut[i] = red + green + blue;
    }
}

/* RGB LCD 缓存*/
uint16_t g_lcdbuf[800];

/**
 * @brief        产生Julia分形图形
 * @param       size_x   : 屏幕x方向的尺寸
 * @param       size_y   : 屏幕y方向的尺寸
 * @param       offset_x : 屏幕x方向的偏移
 * @param       offset_y : 屏幕y方向的偏移
 * @param       zoom     : 缩放因子
 * @retval      无
 */
void julia_generate_fpu(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom)
{
    uint8_t i;
    uint16_t x, y;
    float tmp1, tmp2;
    float num_real, num_img;
    float radius;

    for (y = 0; y < size_y; y++)
    {
        for (x = 0; x < size_x; x++)
        {
            num_real = y - offset_y;
            num_real = num_real / zoom;
            num_img = x - offset_x;
            num_img = num_img / zoom;
            i = 0;
            radius = 0;

            while ((i < ITERATION - 1) && (radius < 4))
            {
                tmp1 = num_real * num_real;
                tmp2 = num_img * num_img;
                num_img = 2 * num_real * num_img + IMG_CONSTANT;
                num_real = tmp1 - tmp2 + REAL_CONSTANT;
                radius = tmp1 + tmp2;
                i++;
            }
            LCD->LCD_RAM = g_color_map[i]; /* 绘制到屏幕 */
        }
    }
}

int main(void)
{
    uint8_t key_value = 0;
    uint8_t i = 0;
    uint8_t autorun = 0;
    float time;
    char buf[50];
    bsp_init();
    LCD_ShowString(30,70,200,16,16,"FPU TEST");
    LCD_ShowString(30,110,200,16,16,"KEY0:+    KEY1:-");
    LCD_ShowString(30,130,200,16,16,"KEY_UP:AUTO/MANUL");
    delay_ms(500);
    julia_clut_init(g_color_map);
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_Press:
                i++;
                if(i > sizeof(zoom_ratio)/2-1)
                    i = 0;
                break;
            case KEY1_Press:
                if(i)
                    i--;
                else
                    i = sizeof(zoom_ratio)/2-1;
                break;
            case WKUP_Press:
                autorun = !autorun;
                break;
            default:break;    
        }
        if(autorun)
        {
            i++;
            LED_ON(LED1_GPIO_Pin);
            if(i > sizeof(zoom_ratio)/2-1)
            {
                i = 0;
            }
        }
        else
        {
            LED_OFF(LED1_GPIO_Pin);
        }
        LCD_Set_Window(0,0,lcddev.width, lcddev.height);
        LCD_WriteRAM_Prepare();
        TIM6->CNT = 0;
        timeout = 0;
        julia_generate_fpu(lcddev.width, lcddev.height, lcddev.width/2, lcddev.height/2, zoom_ratio[i]);
        time = TIM6->CNT + (uint32_t)timeout * 65536;
        sprintf(buf, "%s: Zoom:%d  RunTime:%0.1fms", SCORE_FPU_MODE,zoom_ratio[i], time/10);
        LCD_ShowString(5,lcddev.height-17,lcddev.width-5,12,12,buf);
        printf("%s\n", buf);
        LED_TOGGLE(LED0_GPIO_Pin);
    }
}
```

## 4. FPU常见函数（HAL库）

### 4.1 FPU基础配置

#### 4.1.1. 启用FPU支持

在STM32CubeIDE/IAR/Keil中启用FPU：

- **Keil MDK**：
  
  ```c
  // Target -> Floating Point Hardware 选择 "Single Precision"
  ```

- **IAR EWARM**：
  
  ```c
  // Project -> Options -> General Options -> Floating Point
  // 选择 "FPv4-SP" (Single Precision)
  ```

- **STM32CubeIDE**：
  
  ```c
  // Project Properties -> C/C++ Build -> Settings
  // -> MCU Settings -> Floating-point unit = Hardware (-mfpu=fpv4-sp-d16)
  ```

#### 4.1.2 启动文件配置

```asm6502
; startup_stm32f407xx.s
Reset_Handler:
 ; 启用FPU
 LDR.W R0, =0xE000ED88 ; 加载CPACR地址
 LDR R1, [R0] ; 读取CPACR
 ORR R1, R1, #(0xF << 20) ; 设置CP10和CP11为全访问
 STR R1, [R0] ; 写回CPACR
 DSB ; 数据同步屏障
 ISB ; 指令同步屏障
```

### 4.2 FPU相关寄存器

#### 4.2.1 CPACR (协处理器访问控制寄存器)

地址: `0xE000ED88`

```c
// 启用FPU
SCB->CPACR |= (0xF << 20); // 设置CP10和CP11为全访问

// 检查FPU是否启用
if ((SCB->CPACR & (0xF << 20)) == (0xF << 20)) {
 // FPU已启用
}
```

#### 4.2.2 FPU状态寄存器

```c
// 获取FPU状态
uint32_t fpscr = __get_FPSCR();

// 设置舍入模式
__set_FPSCR(fpscr & ~(0x3 << 22)); // 清除模式位
__set_FPSCR(fpscr | (0x1 << 22)); // 设置为向零舍入

// 启用/禁用异常
__set_FPSCR(fpscr | (1 << 9)); // 启用无效操作异常
```

### 4.3 FPU使用示例

#### 4.3.1 基本浮点运算

```c
float vector_add(float a, float b) {
 return a + b; // 编译器自动生成FPU指令VADD.F32
}

float matrix_multiply(const float *matA, const float *matB, int size) {
 float sum = 0.0f;
 for (int i = 0; i < size; i++) {
 // 使用FPU指令加速计算
 sum += matA[i] * matB[i]; // VMLA.F32
 }
 return sum;
}
```

#### 4.3.2 向量运算 (使用CMSIS-DSP)

```c
#include "arm_math.h"

void fpu_vector_operations(void) {
    float32_t vecA[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float32_t vecB[4] = {0.5f, 1.5f, 2.5f, 3.5f};
    float32_t result[4];
    
    // 向量加法 (FPU优化)
    arm_add_f32(vecA, vecB, result, 4);
    
    // 点积运算
    float32_t dotProduct;
    arm_dot_prod_f32(vecA, vecB, 4, &dotProduct);
    
    // 矩阵乘法
    float32_t matA[9], matB[9], matC[9];
    arm_mat_mult_f32(&matA, &matB, &matC);
}
```

### 4.4 性能优化技巧

#### 4.4.1 循环向量化

```c
// 启用自动向量化 (编译器选项)
// -O3 -ffast-math (GCC)
// --vectorize (IAR)

// 手动向量化示例
void vector_scale(float *data, float scale, int len) {
    // 确保4字节对齐
    if ((uint32_t)data & 0x3) {
        // 处理非对齐数据
    }
    
    int i;
    // 主循环 (每次处理4个元素)
    for (i = 0; i < len - 3; i += 4) {
        data[i]   *= scale;
        data[i+1] *= scale;
        data[i+2] *= scale;
        data[i+3] *= scale;
    }
    
    // 处理剩余元素
    for (; i < len; i++) {
        data[i] *= scale;
    }
}
```

#### 4.4.2 避免浮点转换

```c
// 错误做法 - 导致整数到浮点的转换
for (int i = 0; i < 1000; i++) {
 float x = i; // 隐式转换 (VSCVTF)
 // ...
}

// 正确做法 - 使用浮点计数器
for (float f = 0.0f; f < 1000.0f; f += 1.0f) {
 // 直接使用浮点值
}
```

#### 4.4.3 使用FPU友好算法

```c
// 使用FMA指令 (乘加融合)
float fma_example(float a, float b, float c) {
 // 编译器可能生成 VFMA.F32
 return a * b + c;
}

// 牛顿迭代法求平方根 (FPU优化)
float sqrt_newton(float x) {
 float y = x * 0.5f; // 初始估计值
 for (int i = 0; i < 4; i++) {
 y = 0.5f * (y + x / y); // VSQRT.F32替代
 }
 return y;
}
```

### 4.5 FPU异常处理

#### 4.5.1 异常类型

| 异常位 | 描述    |
| --- | ----- |
| IOC | 无效操作  |
| DZC | 除零    |
| OFC | 溢出    |
| UFC | 下溢    |
| IXC | 不精确结果 |

#### 4.5.2 异常处理配置

```c
void configure_fpu_exceptions(void) {
    // 启用FPU异常
    NVIC_SetPriorityGrouping(3);
    NVIC_SetPriority(MemoryManagement_IRQn, 0);
    NVIC_SetPriority(BusFault_IRQn, 0);
    NVIC_SetPriority(UsageFault_IRQn, 0);
    NVIC_EnableIRQ(UsageFault_IRQn);
    
    // 启用所有FPU异常
    __set_FPSCR(__get_FPSCR() | 0x9F);
}

// 异常处理函数
void UsageFault_Handler(void) {
    uint32_t cfsr = SCB->CFSR;
    uint32_t fpscr = __get_FPSCR();
    
    if (cfsr & (1 << 9)) {  // FPU异常标志
        printf("FPU Exception: ");
        
        if (fpscr & (1 << 0)) printf("IOC ");  // 无效操作
        if (fpscr & (1 << 1)) printf("DZC ");  // 除零
        if (fpscr & (1 << 2)) printf("OFC ");  // 溢出
        if (fpscr & (1 << 3)) printf("UFC ");  // 下溢
        if (fpscr & (1 << 4)) printf("IXC ");  // 不精确
        
        // 清除异常标志
        __set_FPSCR(fpscr & ~0x1F);
    }
    
    while(1);
}
```

### 4.6 FPU性能测试

#### 4.6.1 性能测试函数

```c
#include "arm_math.h"

void fpu_performance_test(void) {
    const int ITERATIONS = 10000;
    uint32_t start, end;
    float result = 0.0f;
    
    // 浮点加法测试
    start = DWT->CYCCNT;
    for (int i = 0; i < ITERATIONS; i++) {
        result += 1.234f;
    }
    end = DWT->CYCCNT;
    printf("Float Add: %lu cycles/op\n", (end - start) / ITERATIONS);
    
    // 浮点乘法测试
    start = DWT->CYCCNT;
    for (int i = 0; i < ITERATIONS; i++) {
        result *= 1.001f;
    }
    end = DWT->CYCCNT;
    printf("Float Mul: %lu cycles/op\n", (end - start) / ITERATIONS);
    
    // 浮点除法测试
    start = DWT->CYCCNT;
    for (int i = 0; i < ITERATIONS; i++) {
        result /= 1.001f;
    }
    end = DWT->CYCCNT;
    printf("Float Div: %lu cycles/op\n", (end - start) / ITERATIONS);
    
    // 浮点平方根测试
    start = DWT->CYCCNT;
    for (int i = 0; i < ITERATIONS; i++) {
        result = sqrtf(result + 1.0f);
    }
    end = DWT->CYCCNT;
    printf("Float Sqrt: %lu cycles/op\n", (end - start) / ITERATIONS);
}
```

---


