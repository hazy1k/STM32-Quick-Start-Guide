# 第一章 GPIO介绍及使用

## 1. STM32H7 GPIO简介

GPIO 是控制或者采集外部器件的信息的外设，既负责输入输出。它按组分配，每组 16 个IO 口，组数视芯片而定。 STM32H750VBT6 芯片是 100 脚的芯片，它的 IO 口有 82 个， IO 分为 5 组，分别是 GPIOA-GPIOE。 这里 GPIOA-GPIOE， 16*5=80 个 IO 口，少了两个，还有两个就是 PH0 和 PH1。 PH0 和 PH1 用于连接外部高速晶振。

### 1.1 GPIO的八种工作模式

#### 1.1.1 浮空输入（Input Floating）

- **MODER**: `00`
- **PUPDR**: `00`（无上下拉）
- **OTYPER**: 无效（输入模式不关心输出类型）

🔹 **特点**：

- 输入引脚不连接内部上拉或下拉电阻。
- 引脚电平“浮动”，完全由外部电路决定。
- 功耗低，但容易受干扰。

🔹 **应用场景**：

- 连接外部数字信号源（如传感器输出、其他 MCU 引脚）。
- 按键检测（需外部上拉/下拉电阻）。
- 高速信号输入（减少 RC 延迟）。

⚠️ **注意**：若引脚悬空（未连接），可能读取不稳定值，建议避免。

---

#### 1.1.2 上拉输入（Input Pull-up）

- **MODER**: `00`
- **PUPDR**: `01`

🔹 **特点**：

- 内部弱上拉电阻（约 30–50kΩ）连接到 VDD。
- 当外部无信号驱动时，引脚默认为高电平。

🔹 **应用场景**：

- 按键输入（按键按下接地，读低电平）。
- 总线空闲时保持高电平（如 I2C 数据线，但 I2C 通常用开漏）。

✅ **优点**：无需外部上拉电阻，简化硬件设计。

---

#### 1.1.3 下拉输入（Input Pull-down）

- **MODER**: `00`
- **PUPDR**: `10`

🔹 **特点**：

- 内部弱下拉电阻连接到 GND。
- 外部无驱动时，默认为低电平。

🔹 **应用场景**：

- 按键输入（按键接 VDD，按下读高电平）。
- 检测使能信号或标志位。

✅ 与上拉类似，避免引脚悬空。

---

#### 1.1.4 模拟输入（Analog Mode）

- **MODER**: `11`
- **PUPDR**: `00`（通常不启用上下拉）

🔹 **特点**：

- 数字输入缓冲器关闭，引脚直接连接模拟外设（如 ADC、DAC、比较器）。
- 引脚呈现高阻抗状态。
- 不参与数字逻辑操作。

🔹 **应用场景**：

- ADC 采集（如温度传感器、电位器）。
- DAC 输出通道。
- 低功耗休眠模式下的引脚配置。

✅ **重要**：使用 ADC 时必须配置为模拟输入，否则可能引入噪声或功耗。

---

#### 1.1.5 推挽输出（Output Push-Pull）

- **MODER**: `01`
- **OTYPER**: `0`（推挽）

🔹 **特点**：

- 可主动输出高电平（VDD）或低电平（GND）。
- 驱动能力强，高低电平均有低阻抗路径。
- 不需要外部上拉电阻。

🔹 **应用场景**：

- 驱动 LED（需限流电阻）。
- 控制继电器、MOSFET 栅极。
- 数字信号输出（如使能信号、状态指示）。

📌 **优点**：响应快、驱动能力强，是常用的输出模式。

---

#### 1.1.6 开漏输出（Output Open-Drain）

- **MODER**: `01`
- **OTYPER**: `1`（开漏）

🔹 **特点**：

- 只能主动拉低电平（NMOS 导通接地）。
- 输出高电平时“断开”，需外部上拉电阻实现高电平。
- 多个开漏输出可“线与”连接。

🔹 **应用场景**：

- I2C 总线（SDA/SCL 必须为开漏）。
- 多设备共享信号线（如中断请求线）。
- 电平转换（连接不同电压系统）。

✅ **必须外接上拉电阻**（如 4.7kΩ 到目标电压）。

---

#### 1.1.7 推挽式复用功能（Alternate Function Push-Pull）

- **MODER**: `10`
- **OTYPER**: `0`
- **AFRL/AFRH**: 选择复用功能编号（AF0–AF15）

🔹 **特点**：

- 引脚由某个外设（如 USART、SPI、TIM）控制输出。
- 输出方式为推挽，适合高速、强驱动场合。

🔹 **应用场景**：

- USART Tx 引脚（高速串行通信）。
- SPI SCK、MOSI（高速时钟和数据输出）。
- PWM 输出（TIMx_CHx）。

📌 常用于需要高性能输出的外设功能。

---

#### 1.1.8 开漏式复用功能（Alternate Function Open-Drain）

- **MODER**: `10`
- **OTYPER**: `1`
- **AFRL/AFRH**: 选择复用功能编号

🔹 **特点**：

- 外设控制引脚，但输出为开漏方式。
- 需外部上拉电阻实现高电平。

🔹 **应用场景**：

- I2C 总线（标准模式）。
- 与其他设备共享的外设信号线。
- 与 5V 系统兼容的通信接口（若引脚支持 5V tolerant）。

📌 适用于需要“线与”逻辑或电平转换的场景。

---

### 1.2 模式对比表

| 模式   | MODER | 输入/输出 | 上下拉  | 输出类型 | 典型用途             |
| ---- | ----- | ----- | ---- | ---- | ---------------- |
| 浮空输入 | 00    | 输入    | 无    | -    | 传感器、高速输入         |
| 上拉输入 | 00    | 输入    | 上拉   | -    | 按键（低有效）          |
| 下拉输入 | 00    | 输入    | 下拉   | -    | 按键（高有效）          |
| 模拟输入 | 11    | 输入    | 无    | -    | ADC、DAC、低功耗      |
| 推挽输出 | 01    | 输出    | 可选   | 推挽   | LED、继电器、数字输出     |
| 开漏输出 | 01    | 输出    | 外加上拉 | 开漏   | I2C、线与、电平转换      |
| 推挽复用 | 10    | 外设控制  | 可选   | 推挽   | USART Tx、SPI、PWM |
| 开漏复用 | 10    | 外设控制  | 外加上拉 | 开漏   | I2C、共享总线         |

## 2. GPIO使用示例-STM32IDE

使用STM32IDE集成的STM32Cube配置GPIO，初始化Beep，LED，按键外设，并编写测试代码

### 2.1 STM32Cube配置

#### 2.1.1 RCC配置

只在第一章中展示，因为后续内容一样（温馨提示：如果图片加载不出，请使用代理，因为图床搭建使用的是GitHub）

![屏幕截图 2025-08-04 214455.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/04-21-59-47-屏幕截图%202025-08-04%20214455.png)

![屏幕截图 2025-08-04 214429.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/04-22-00-00-屏幕截图%202025-08-04%20214429.png)

#### 2.1.2 LED GPIO配置

![屏幕截图 2025-08-04 220242.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/04-22-03-00-屏幕截图%202025-08-04%20220242.png)

![屏幕截图 2025-08-04 220248.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/04-22-03-04-屏幕截图%202025-08-04%20220248.png)

![屏幕截图 2025-08-04 220253.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/04-22-03-19-屏幕截图%202025-08-04%20220253.png)

#### 2.1.3 Beep GPIO配置

![屏幕截图 2025-08-04 220444.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/04-22-04-52-屏幕截图%202025-08-04%20220444.png)

#### 2.1.4 按键 GPIO配置

![屏幕截图 2025-08-04 220528.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/04-22-06-22-屏幕截图%202025-08-04%20220528.png)

![屏幕截图 2025-08-04 220534.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/04-22-06-35-屏幕截图%202025-08-04%20220534.png)

![屏幕截图 2025-08-04 220545.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/04-22-06-51-屏幕截图%202025-08-04%20220545.png)

### 2.2 用户代码

#### 2.2.1 LED控制宏

```c
// LED低电平点亮
#define LED_ON(port, pin)      HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)
#define LED_OFF(port, pin)     HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)
#define LED_TOGGLE(port, pin)  HAL_GPIO_TogglePin(port, pin)
```

#### 2.2.2 按键扫描函数

```c
// 按键状态定义
typedef enum {
    KEY_NONE = 0,
    KEY_WK_UP,
    KEY0,
    KEY1
} KEY_TypeDef;

// 按键扫描函数，返回按下键值（一次只响应一次）
KEY_TypeDef Key_Scan(void)
{
    static uint8_t key_up_wkup = 1; // 松手标志
    static uint8_t key_up_key0 = 1;
    static uint8_t key_up_key1 = 1;

    // WKUP为高电平按下，其余为低电平按下
    if (HAL_GPIO_ReadPin(WK_UP_GPIO_Port, WK_UP_Pin) == GPIO_PIN_SET)
    {
        if (key_up_wkup)  // 上一个循环没有按下，当前为按下
        {
            key_up_wkup = 0;
            return KEY_WK_UP;
        }
    }
    else key_up_wkup = 1;

    if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET)
    {
        if (key_up_key0)
        {
            key_up_key0 = 0;
            return KEY0;
        }
    }
    else key_up_key0 = 1;

    if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
    {
        if (key_up_key1)
        {
            key_up_key1 = 0;
            return KEY1;
        }
    }
    else key_up_key1 = 1;

    return KEY_NONE;
}
```

#### 2.2.3 主函数测试

```c
/* 主函数 */
int main(void)
{
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  while (1)
  {
      KEY_TypeDef key = Key_Scan(); // 扫描按键
      switch(key)
      {
            case KEY_WK_UP:
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET); // 打开
              beep_status = 1;
              break;
          case KEY0:
              HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); // 关闭
              beep_status = 0;
              break;
          case KEY1:
              beep_status = !beep_status;
              HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, beep_status ? GPIO_PIN_SET : GPIO_PIN_RESET); // 翻转
              break;
          default:
              break;
      }
      /* 流水灯 */
      LED_OFF(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
      LED_ON(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
      HAL_Delay(100);
      LED_OFF(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
      LED_ON(LED_RED_GPIO_Port, LED_RED_Pin);
      HAL_Delay(100);
      LED_OFF(LED_RED_GPIO_Port, LED_RED_Pin);
      LED_ON(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
      HAL_Delay(100);
  }
} 
```

## 3. GPIO相关函数总结（HAL库）

### 3.1 初始化与配置

- **`HAL_GPIO_Init(GPIO_TypeDef *GPIOx, const GPIO_InitTypeDef *GPIO_Init)`**  
  初始化一个或多个 GPIO 引脚。  
  **使用前必须使能时钟**：

```c
__HAL_RCC_GPIOA_CLK_ENABLE();
```

- **`GPIO_InitTypeDef` 结构体成员说明**：
  
  - `Pin`：指定引脚，如 `GPIO_PIN_5`，支持组合 `GPIO_PIN_5 | GPIO_PIN_6`
  - `Mode`：工作模式（见下表）
  - `Pull`：上下拉配置
  - `Speed`：输出速度（仅输出/复用模式有效）
  - `Alternate`：复用功能编号（如 `GPIO_AF5_SPI1`）

| 模式宏                           | 说明            |
| ----------------------------- | ------------- |
| `GPIO_MODE_INPUT`             | 浮空输入          |
| `GPIO_MODE_OUTPUT_PP`         | 推挽输出          |
| `GPIO_MODE_OUTPUT_OD`         | 开漏输出          |
| `GPIO_MODE_AF_PP`             | 复用推挽输出        |
| `GPIO_MODE_AF_OD`             | 复用开漏输出        |
| `GPIO_MODE_ANALOG`            | 模拟输入（ADC/DAC） |
| `GPIO_MODE_IT_RISING`         | 上升沿外部中断       |
| `GPIO_MODE_IT_FALLING`        | 下降沿外部中断       |
| `GPIO_MODE_IT_RISING_FALLING` | 双边沿外部中断       |

- **输出速度选项**：

```c
GPIO_SPEED_FREQ_LOW       // 低速
GPIO_SPEED_FREQ_MEDIUM    // 中速
GPIO_SPEED_FREQ_HIGH      // 高速
GPIO_SPEED_FREQ_VERY_HIGH // 超高速
```

- **上下拉配置**：

```c
GPIO_NOPULL     // 无
GPIO_PULLUP     // 上拉
GPIO_PULLDOWN   // 下拉
```

### 3.2 IO 读写操作

- **`HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)`**  
  读取引脚电平，返回：
  
  - `GPIO_PIN_SET`（高电平）
  - `GPIO_PIN_RESET`（低电平）  
    ✅ 常用于按键检测。

- **`HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)`**  
  设置引脚输出：

```c
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // 输出高
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // 输出低
```

- **`HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)`**  
  翻转引脚电平，常用于 LED 闪烁：

```c
HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
```

### 3.3 外部中断处理

- **`HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)`**  
  中断服务函数，需在中断向量中调用：

```c
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
```

- **`HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)`**  
  用户可重写的回调函数（弱函数），添加业务逻辑：

```c
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // 翻转 LED
    }
}
```

### 3.4 高级功能

- **`HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)`**  
  锁定引脚配置（防止误改），锁定后需复位才能修改。  
  返回 `HAL_OK` 表示成功。

- **中断标志操作宏**：
  
  - `__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5)`：检查中断是否触发
  - `__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5)`：清除中断标志
  - `__HAL_GPIO_EXTI_GENERATE_SWIT(GPIO_PIN_5)`：软件触发中断（测试用）

### 3.5 使用示例（完整流程）

```c
// 1. 使能时钟
__HAL_RCC_GPIOA_CLK_ENABLE();

// 2. 配置 PA5 为推挽输出
GPIO_InitTypeDef GPIO_InitStruct = {0};
GPIO_InitStruct.Pin   = GPIO_PIN_5;
GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
GPIO_InitStruct.Pull  = GPIO_NOPULL;
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

// 3. 控制 LED
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // 点亮
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); // 熄灭
HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);                // 翻转

// 4. 读取按键（PB0，下拉输入）
__HAL_RCC_GPIOB_CLK_ENABLE();
GPIO_InitStruct.Pin   = GPIO_PIN_0;
GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET)
{
    // 按键按下
}
```

- 未使用引脚配置为 `GPIO_MODE_ANALOG`，降低功耗
- I2C 引脚必须使用 `GPIO_MODE_AF_OD` 并外接 4.7kΩ 上拉电阻
- 中断引脚需配置 NVIC 优先级并使能
- 多引脚配置支持位或操作
- 调试时用 `TogglePin` 实现心跳灯，简洁可靠

---
