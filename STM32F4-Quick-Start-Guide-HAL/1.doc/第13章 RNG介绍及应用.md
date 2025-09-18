# 第十三章 RNG介绍及应用

## 1. RNG简介

STM32F407 自带了硬件随机数发生器（RNG）， RNG 处理器是一个以连续模拟噪声为基础的随机数发生器，在主机读数时提供一个 32 位的随机数。

![屏幕截图 2025-08-04 105418.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/04-10-54-26-屏幕截图%202025-08-04%20105418.png)

STM32F407 的随机数发生器（RNG）采用模拟电路实现。此电路产生馈入线性反馈移位寄存器（RNG_LFSR）的种子，用于生成 32 位随机数。

该模拟电路由几个环形振荡器组成，振荡器的输出进行异或运算以产生种子。RNG_LFSR 由专用时钟(PLL48CLK)按恒定频率提供时钟信息，因此随机数质量与 HCLK频率无关。当将大量种子引入 RNG_LFSR 后， RNG_LFSR 的内容会传入数据寄存器(RNG_DR)。

同时，系统会监视模拟种子和专用时钟 rng_clk，当种子上出现异常序列，或 rng_clk 时钟频率过低时，可以由 RNG_SR 寄存器的对应位读取到，如果设置了中断，则在检测到错误时，还可以产生中断。

## 2. RNG使用示例

### 2.1 RNG初始化

```c
#include "rng.h"
#include "delay.h"

RNG_HandleTypeDef rng_hangle;

// 初始化RNG
uint8_t RNG_Init(void)
{
    uint16_t retry = 0;
    rng_hangle.Instance = RNG;
    HAL_RNG_DeInit(&rng_hangle);
    HAL_RNG_Init(&rng_hangle);
    while(__HAL_RNG_GET_FLAG(&rng_hangle, RNG_FLAG_DRDY) == RESET && retry < 10000)
    {
        retry++;
        delay_us(10);
    }
    if(retry >= 10000)
    {
        return 1;
    }
    return 0;
}

void HAL_RNG_MspInit(RNG_HandleTypeDef *hRNG)
{
    __HAL_RCC_RNG_CLK_ENABLE();
}
```

### 2.2 获取随机数

```c
// 获取随机数
uint32_t rng_get_random(void)
{
    uint32_t random_num = 0;
    HAL_RNG_GenerateRandomNumber(&rng_hangle, &random_num);
    return random_num;
}

// 获取范围内的随机数
uint32_t rng_get_random_range(int min, int max)
{
    uint32_t random_num = 0;
    HAL_RNG_GenerateRandomNumber(&rng_hangle, &random_num);
    return (random_num % (max - min + 1)) + min;
}
```

### 2.3 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "usmart.h"
#include "rng.h"

int main(void)
{
  uint32_t random_number;
  uint8_t i = 0;
  uint8_t key;
  bsp_init();
  usmart_dev.init(84);
  LCD_ShowString(30,50,200,16,16,"RNG Test");
  while(RNG_Init())
  {
    LCD_ShowString(30,110,200,16,16,"RNG Init Error");
    delay_ms(1000);
    LCD_ShowString(30,110,200,16,16,"RNG Trying...");
  }
  LCD_ShowString(30,110,200,16,16,"RNG Init Success");
    while(1)
  {
    key = key_scan(0);
    if(key == KEY0_Press)
    {
      random_number = rng_get_random();
      LCD_ShowNum(128, 150, random_number, 10, 16);
    }
    if((i % 20) == 0)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
      random_number = rng_get_random_range(0,100); // 获取100以内的随机数
      LCD_ShowNum(128, 170, random_number, 10, 16);
    }
    i++;
    delay_ms(10);
  }
}
```

## 3. RNG常见函数（HAL库）

### 3.1 RNG 初始化与配置

#### 3.1.1 `HAL_RNG_Init()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RNG_Init(RNG_HandleTypeDef *hrng)
```

**参数**:

- `hrng`: RNG 句柄指针

**配置结构体**:

```c
typedef struct {
  RNG_TypeDef          *Instance;    // RNG寄存器基地址
  uint32_t             State;        // RNG状态机
  uint32_t             Lock;         // 锁定状态
  __IO uint32_t        ErrorCode;    // 错误代码
} RNG_HandleTypeDef;
```

**功能**: 初始化 RNG 外设，使能时钟和硬件

**示例配置**:

```c
RNG_HandleTypeDef hrng;

hrng.Instance = RNG;
if (HAL_RNG_Init(&hrng) != HAL_OK) {
  // 错误处理
  Error_Handler();
}
```

#### 3.1.2 `HAL_RNG_DeInit()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RNG_DeInit(RNG_HandleTypeDef *hrng)
```

**功能**: 反初始化 RNG 外设，禁用时钟

### 3.2 随机数生成

#### 3.2.1 `HAL_RNG_GenerateRandomNumber()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber(
    RNG_HandleTypeDef *hrng,
    uint32_t *random32bit)
```

**参数**:

- `random32bit`: 指向存储生成的32位随机数的变量

**功能**: 生成一个32位随机数

**示例**:

```c
uint32_t random_value;
if (HAL_RNG_GenerateRandomNumber(&hrng, &random_value) == HAL_OK) {
  // 使用随机数
  printf("Random number: %lu\n", random_value);
}
```

#### 3.2.2 `HAL_RNG_GetRandomNumber()`

**函数原型**:

```c
uint32_t HAL_RNG_GetRandomNumber(RNG_HandleTypeDef *hrng)
```

**功能**: 直接返回32位随机数 (简化版)

**示例**:

```c
uint32_t random_value = HAL_RNG_GetRandomNumber(&hrng);
```

### 3.3 中断模式随机数生成

#### 3.3.1 `HAL_RNG_GenerateRandomNumber_IT()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber_IT(RNG_HandleTypeDef *hrng)
```

**功能**: 启动中断模式下的随机数生成

#### 3.3.2 随机数就绪回调函数

```c
// 用户需重写此函数
void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, uint32_t random32bit)
{
  // 处理生成的随机数
  printf("Interrupt generated random number: %lu\n", random32bit);
}
```

#### 3.3.3 错误回调函数

```c
// 用户需重写此函数
void HAL_RNG_ErrorCallback(RNG_HandleTypeDef *hrng)
{
  // 处理RNG错误
  printf("RNG error detected: 0x%08lX\n", hrng->ErrorCode);
}
```

### 3.4 DMA 模式随机数生成

#### 3.4.1 `HAL_RNG_GenerateRandomNumber_DMA()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber_DMA(
    RNG_HandleTypeDef *hrng,
    uint32_t *pData,
    uint32_t Length)
```

**参数**:

- `pData`: 指向存储随机数的缓冲区

- `Length`: 要生成的随机数数量

**功能**: 使用DMA生成多个随机数

**示例**:

```c
#define RANDOM_NUM_COUNT 100
uint32_t random_buffer[RANDOM_NUM_COUNT];

// 启动DMA传输
if (HAL_RNG_GenerateRandomNumber_DMA(&hrng, random_buffer, RANDOM_NUM_COUNT) != HAL_OK) {
  // 错误处理
}
```

#### 3.4.2 DMA 传输完成回调

```c
// 用户需重写此函数
void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, uint32_t random32bit)
{
  // 当使用DMA时，此回调在每次生成随机数时被调用
}
```

#### 3.4.3 DMA 传输完整回调

```c
// 用户需重写此函数
void HAL_RNGEx_RandomNumberGeneratedCallback(RNG_HandleTypeDef *hrng)
{
  // 当所有随机数生成完成时调用
  printf("All random numbers generated via DMA\n");
}
```

### 3.5 RNG 状态与错误处理

#### 3.5.1 状态检查函数

```c
// 获取RNG状态
HAL_RNG_StateTypeDef HAL_RNG_GetState(RNG_HandleTypeDef *hrng);

// 获取错误代码
uint32_t HAL_RNG_GetError(RNG_HandleTypeDef *hrng);
```

#### 3.5.2 常见错误代码

```c
#define HAL_RNG_ERROR_NONE     0x00000000U // 无错误
#define HAL_RNG_ERROR_SEED     0x00000001U // 种子错误
#define HAL_RNG_ERROR_CLOCK    0x00000002U // 时钟错误
#define HAL_RNG_ERROR_BUSY     0x00000004U // RNG忙
#define HAL_RNG_ERROR_TIMEOUT  0x00000008U // 超时错误
#define HAL_RNG_ERROR_DMA      0x00000010U // DMA错误
```

#### 3.5.3 错误处理示例

```c
uint32_t random_value;
HAL_StatusTypeDef status = HAL_RNG_GenerateRandomNumber(&hrng, &random_value);

if (status != HAL_OK) {
  uint32_t error_code = HAL_RNG_GetError(&hrng);

  if (error_code & HAL_RNG_ERROR_SEED) {
    // 种子错误处理
    printf("RNG seed error detected\n");
  }

  if (error_code & HAL_RNG_ERROR_CLOCK) {
    // 时钟错误处理
    printf("RNG clock error detected\n");
  }
}
```

### 3.6 RNG 健康检查

#### 3.6.1 `HAL_RNGEx_RecoverSeedError()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_RNGEx_RecoverSeedError(RNG_HandleTypeDef *hrng)
```

**功能**: 尝试从种子错误中恢复

**示例**:

```c
if (HAL_RNG_GenerateRandomNumber(&hrng, &random_value) != HAL_OK) {
  if (HAL_RNG_GetError(&hrng) & HAL_RNG_ERROR_SEED) {
    // 尝试恢复种子错误
    if (HAL_RNGEx_RecoverSeedError(&hrng) == HAL_OK) {
      // 重新尝试生成随机数
      HAL_RNG_GenerateRandomNumber(&hrng, &random_value);
    }
  }
}
```

---
