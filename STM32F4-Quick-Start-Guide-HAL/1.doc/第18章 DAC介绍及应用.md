# 第十八章 DAC介绍及应用

## 1. DAC简介

TM32F407 的 DAC 模块（数字/模拟转换模块） 是 12 位数字输入，电压输出型的 DAC。DAC 可以配置为 8 位或 12 位模式，也可以与 DMA 控制器配合使用。 DAC 工作在 12 位模式时，数据可以设置成左对齐或右对齐。 DAC 模块有 2 个输出通道，每个通道都有单独的转换器。在双 DAC 模式下， 2 个通道可以独立地进行转换，也可以同时进行转换并同步地更新 2 个通道的输出。 DAC 可以通过引脚输入参考电压 Vref+（通 ADC 共用） 以获得更精确的转换结果。

![屏幕截图 2025-08-11 113521.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/11-11-35-30-屏幕截图%202025-08-11%20113521.png)

图中 VDDA 和 VSSA 为 DAC 模块模拟部分的供电，而 VREF+则是 DAC 模块的参考电压。DAC_OUT1/2 就是 DAC 的两个输出通道了（对应 PA4 或者 PA5 引脚）。 ADC 的这些输入/输出引脚信息如下表所示：

![屏幕截图 2025-08-11 113603.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/11-11-36-08-屏幕截图%202025-08-11%20113603.png)

## 2. DAC应用示例

### 2.1 DAC输出指定电压

#### 2.1.1 DAC初始化

```c
void dac_init(uint8_t outx)
{
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    dac_handle.Instance = DAC;
    HAL_DAC_Init(&dac_handle);
    DAC_ChannelConfTypeDef dac_ch_handle;
    dac_ch_handle.DAC_Trigger = DAC_TRIGGER_NONE;
    dac_ch_handle.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

    switch (outx)
    {
    case 1:
        HAL_DAC_ConfigChannel(&dac_handle, &dac_ch_handle, DAC_CHANNEL_1);
        HAL_DAC_Start(&dac_handle, DAC_CHANNEL_1);
        break;

    case 2:
        HAL_DAC_ConfigChannel(&dac_handle, &dac_ch_handle, DAC_CHANNEL_2);
        HAL_DAC_Start(&dac_handle, DAC_CHANNEL_2);
        break;
    default:
        break;
    }
}
```

#### 2.1.2 DAC设置电压

```c
// 设置输出通道电压
void dac_set_voltage(uint8_t outx, uint16_t voltage)
{
    double temp = voltage;
    temp /= 1000;
    temp = temp*4095/3.3;
    if(temp > 4095)
        temp = 4095;
    if(outx)
    {
        HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp); // 设置DAC输出值
    }   
    else
    {
        HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_2, DAC_ALIGN_12B_R, temp);
    }
}
```

#### 2.1.3 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"
#include "dac.h"

extern DAC_HandleTypeDef dac_handle;

int main(void)
{
  uint16_t adc_value;
  float temp;
  uint8_t i =0;
  uint16_t dac_value = 0;
  uint8_t key_value = 0;

  bsp_init();
  adc_init();
  dac_init(1); // 初始化DAC1_OUT1通道 1:DAC_OUT_1-PA4  2:DAC_OUT_2-PA5 
  LCD_ShowString(30,50,200,16,16,"STM32F4 DAC Test");
  LCD_ShowString(30,110,200,16,16,"WK_UP:+  KEY1:-");
  LCD_ShowString(30,130,200,16,16,"DAC VAL:");
  LCD_ShowString(30,150,200,16,16,"DAC VOL:0.000V");
  LCD_ShowString(30,170,200,16,16,"ADC VOL:0.000V");
    while(1)
  {
    i++;
    key_value = key_scan(0);
    if(key_value == WKUP_Press)
    {
      if(dac_value < 4095)
      {
        dac_value += 200;
      }
      HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    }
    else if(key_value == KEY1_Press)
    {
      if(dac_value > 0)
      {
        dac_value -= 200;
      }
      else
      {
        dac_value = 0;
      }
      HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
    }
    if(i == 0 || key_value == KEY1_Press || key_value == WKUP_Press) // WKUP/KEY1按下了,或者定时时间到了
    {
      adc_value = HAL_DAC_GetValue(&dac_handle, DAC_CHANNEL_1); // 获取DAC输出值
      LCD_ShowxNum(94,130,adc_value,4,16,0);
      temp = (float)adc_value*(3.3/4095); // 计算ADC电压值
      adc_value = temp;
      LCD_ShowxNum(94,150,temp,1,16,0); // 显示电压整数值
      temp -= adc_value;
      temp *= 1000;
      LCD_ShowxNum(110,150,temp,3,16,0x80);
      adc_value = adc_get_result_average(ADC_CHANNEL_5, 20); // 获取ADC电压值
      temp = (float)adc_value*(3.3/4095); // 计算ADC电压值
      adc_value = temp;
      LCD_ShowxNum(94,170,temp,1,16,0); // 显示电压整数值
      temp -= adc_value;
      temp *= 1000;
      LCD_ShowxNum(110,170,temp,3,16,0x80);
      LED_TOGGLE(LED0_GPIO_Pin);
      i = 0;
    }
    delay_ms(10);
  }
}
```

### 2.2 PWM模拟DAC

#### 2.2.1 定时器配置

```c
#include "dac.h"

// 定时器PWM模拟DAC
TIM_HandleTypeDef tim_handle;
TIM_OC_InitTypeDef tim_oc_init;

void pwmdac_init(uint16_t arr, uint16_t psc)
{
    tim_handle.Instance = TIM9;
    tim_handle.Init.Prescaler = psc;
    tim_handle.Init.Period = arr;
    tim_handle.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_PWM_Init(&tim_handle);

    tim_oc_init.OCMode = TIM_OCMODE_PWM1;
    tim_oc_init.Pulse = arr/2;
    tim_oc_init.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(&tim_handle, &tim_oc_init, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&tim_handle, TIM_CHANNEL_2);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init;
    if(htim->Instance == TIM9)
    {
        __HAL_RCC_TIM9_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        gpio_init.Pin = GPIO_PIN_3;
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Pull = GPIO_PULLUP;
        gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init.Alternate = GPIO_AF3_TIM9;
        HAL_GPIO_Init(GPIOA, &gpio_init);
    }
}
```

#### 2.2.2 设置输出电压

```c
void pwmdac_set_value(uint16_t value)
{
    float temp = value;
    temp /= 100;
    temp = temp*256/3.3f;
    __HAL_TIM_SET_COMPARE(&tim_handle, TIM_CHANNEL_4, temp);
}
```

#### 2.2.3 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"
#include "dac.h"

extern TIM_HandleTypeDef tim_handle;

int main(void)
{
  uint16_t adc_value = 0;
  float temp;
  uint8_t i = 0;
  uint8_t key_value = 0;
  uint16_t dac_value;
  bsp_init();
  adc_init();
  pwmdac_init(256-1,0); // PWM DAC初始化,Fpwm = 84 / 256 =328.125Khz

  LCD_ShowString(30,130,200,16,16,"PWM VAL:");
  LCD_ShowString(30,150,200,16,16,"DAC VOL:0.000V");
  LCD_ShowString(30,170,200,16,16,"ADC VAL:0.000V");

  __HAL_TIM_SET_COMPARE(&tim_handle, TIM_CHANNEL_2, dac_value);
  while(1)
  {
    i++;
    key_value = key_scan(0);
    if(key_value == WKUP_Press)
    {
      if(dac_value < 250)
      {
        dac_value += 10;
      }
      __HAL_TIM_SET_COMPARE(&tim_handle, TIM_CHANNEL_2, dac_value);
    }
    else if(key_value == KEY1_Press)
    {
      if(dac_value > 10)
      {
        dac_value -= 10;
      }
      else
      {
        dac_value = 0;
      }
      __HAL_TIM_SET_COMPARE(&tim_handle, TIM_CHANNEL_2, dac_value);
    }

    if(i == 10||key_value == WKUP_Press||key_value == KEY1_Press)
    {
      /* DAC值读取计算 */
      adc_value = __HAL_TIM_GET_COMPARE(&tim_handle, TIM_CHANNEL_2);
      LCD_ShowxNum(94,130,adc_value,3,16,0);
      temp = (float)adc_value*(3.3/256);
      adc_value = temp;
      LCD_ShowxNum(94,150,temp,1,16,0);
      temp -= adc_value;
      temp *= 1000;
      LCD_ShowxNum(110,150,temp,3,16,0x80);
      /* ADC值读取计算 */
      adc_value = adc_get_result_average(ADC_CHANNEL_5, 10);
      temp = (float)adc_value*(3.3/4096);
      adc_value = temp;
      LCD_ShowxNum(94,170,temp,1,16,0);
      temp -= adc_value;
      temp *= 1000;
      LCD_ShowxNum(110,170,temp,3,16,0x80);

      LED_TOGGLE(LED0_GPIO_Pin);
      i = 0;
    }
    delay_ms(5);
  }
}
```

### 2.3 DAC输出三角波

#### 2.3.1 DAC初始化

```c
#include "dac.h"
#include "delay.h"

DAC_HandleTypeDef dac_handle;

void dac_init(uint8_t outx)
{
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    dac_handle.Instance = DAC;
    HAL_DAC_Init(&dac_handle);
    DAC_ChannelConfTypeDef dac_ch_handle;
    dac_ch_handle.DAC_Trigger = DAC_TRIGGER_NONE;
    dac_ch_handle.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

    switch (outx)
    {
    case 1:
        HAL_DAC_ConfigChannel(&dac_handle, &dac_ch_handle, DAC_CHANNEL_1);
        HAL_DAC_Start(&dac_handle, DAC_CHANNEL_1);
        break;

    case 2:
        HAL_DAC_ConfigChannel(&dac_handle, &dac_ch_handle, DAC_CHANNEL_2);
        HAL_DAC_Start(&dac_handle, DAC_CHANNEL_2);
        break;
    default:
        break;
    }
}

// 设置输出通道电压
void dac_set_voltage(uint8_t outx, uint16_t voltage)
{
    double temp = voltage;
    temp /= 1000;
    temp = temp*4095/3.3;
    if(temp > 4095)
        temp = 4095;
    if(outx)
    {
        HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, temp); // 设置DAC输出值
    }   
    else
    {
        HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_2, DAC_ALIGN_12B_R, temp);
    }
}
```

#### 2.3.2 输出三角波

```c
/**
 * @brief       设置DAC_OUT1输出三角波
 *   @note      输出频率 ≈ 1000 / (dt * samples) Khz, 不过在dt较小的时候,比如小于5us时, 由于delay_us
 *              本身就不准了(调用函数,计算等都需要时间,延时很小的时候,这些时间会影响到延时), 频率会偏小.
 * 
 * @param       maxval : 最大值(0 < maxval < 4096), (maxval + 1)必须大于等于samples/2
 * @param       dt     : 每个采样点的延时时间(单位: us)
 * @param       samples: 采样点的个数, samples必须小于等于(maxval + 1) * 2 , 且maxval不能等于0
 * @param       n      : 输出波形个数,0~65535
 *
 * @retval      无
 */
void dac_triangular_wave(uint16_t maxval, uint16_t dt, uint16_t samples, uint16_t n)
{
    uint16_t i, j;
    float incval; // 递增量 
    float Curval; // 当前值
    if((maxval + 1) <= samples) return ;   // 数据不合法
    incval = (maxval + 1) / (samples / 2); // 计算递增量
    for(j = 0; j < n; j++)
    { 
        Curval = 0;
        HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Curval); // 先输出0
        for(i = 0; i < (samples / 2); i++)  // 输出上升沿
        {
            Curval  +=  incval;             // 新的输出值
            HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Curval);
            delay_us(dt);
        } 
        for(i = 0; i < (samples / 2); i++)  // 输出下降沿
        {
            Curval  -=  incval;             // 新的输出值
            HAL_DAC_SetValue(&dac_handle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Curval);
            delay_us(dt);
        }
    }
}
```

#### 2.3.3 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"
#include "dac.h"

extern DAC_HandleTypeDef dac_handle;

int main(void)
{
  uint8_t i = 0;
  uint8_t key_value = 0;
  bsp_init();
  adc_init();
  dac_init(1); // 初始化DAC1_OUT1通道 1:DAC_OUT_1-PA4  2:DAC_OUT_2-PA5 
  LCD_ShowString(30,50,200,16,16,"STM32F4 DAC Triangular Test");
  LCD_ShowString(30,110,200,16,16,"KEY0:Wave1  WKUP:Wave2");
  LCD_ShowString(30,130,200,16,16,"DAC None");
    while(1)
  {
    i++;
    key_value = key_scan(0);
    if(key_value == KEY0_Press)
    {
      LCD_ShowString(30,130,200,16,16,"DAC Wave1");
      dac_triangular_wave(4095,5,2000,100); /* 幅值4095, 采样点间隔5us, 2000个采样点, 100个波形 */
      LCD_ShowString(30,130,200,16,16,"DAC None ");
    }
    else if(key_value == WKUP_Press)
    {
      LCD_ShowString(30,130,200,16,16,"DAC Wave2");
      dac_triangular_wave(4095,500,20,100); /* 幅值4095, 采样点间隔500us, 20个采样点, 100个波形 */
      LCD_ShowString(30,130,200,16,16,"DAC None ");
    }
    if(i == 10)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
      i = 0;
    }
    delay_ms(10);
  }
}
```

### 2.4 DAC输出正弦波

#### 2.4.1 DAC-DMA初始化

```c
void dac_dma_init(uint8_t outx)
{
    DAC_ChannelConfTypeDef dac_ch_handle;
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DAC_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    GPIO_InitStruct.Pin = (outx==1)? GPIO_PIN_4 : GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    dac_handle.Instance = DAC;
    HAL_DAC_Init(&dac_handle);
    dac_dma_handle.Instance = (outx==1)?DMA1_Stream5:DMA1_Stream6; // DMA1_Stream5/6
    dac_dma_handle.Init.Channel = DMA_CHANNEL_7;
    dac_dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH; // 存储器到外设
    dac_dma_handle.Init.PeriphInc = DMA_PINC_DISABLE; // 外设地址不增长
    dac_dma_handle.Init.MemInc = DMA_MINC_ENABLE; // 存储器地址增长
    dac_dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    dac_dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    dac_dma_handle.Init.Mode = DMA_CIRCULAR; // 循环模式
    dac_dma_handle.Init.Priority = DMA_PRIORITY_MEDIUM;
    dac_dma_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用FIFO
    HAL_DMA_Init(&dac_dma_handle);
    __HAL_LINKDMA(&dac_handle, DMA_Handle1, dac_dma_handle); // 链接DMA
    dac_ch_handle.DAC_Trigger = DAC_TRIGGER_T7_TRGO; // 定时器7触发
    dac_ch_handle.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; // 使能输出缓冲
    HAL_DAC_ConfigChannel(&dac_handle, &dac_ch_handle, DAC_CHANNEL_1); // 配置通道1
}
```

#### 2.4.2 使能波形输出

```c
/**
 * @brief       DAC DMA使能波形输出
 *   @note      TIM7的输入时钟频率(f)来自APB1, f = 42 * 2 = 84Mhz.
 *              DAC触发频率 ftrgo = f / ((psc + 1) * (arr + 1))
 *              波形频率 = ftrgo / ndtr;
 * @param       outx        : DAC通道1/2
 * @param       ndtr        : DMA通道单次传输数据量
 * @param       arr         : TIM7的自动重装载值
 * @param       psc         : TIM7的分频系数
 * @retval      无
 */
void dac_dma_wave_enable(uint8_t outx, uint16_t ndtr, uint16_t arr, uint16_t psc)
{
    TIM_HandleTypeDef tim7_handle = {0};
    TIM_MasterConfigTypeDef master_config = {0};

    __HAL_RCC_TIM7_CLK_ENABLE();                                       

    tim7_handle.Instance = TIM7;                                        
    tim7_handle.Init.Prescaler = psc;                                  
    tim7_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                 
    tim7_handle.Init.Period = arr;                                     
    tim7_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&tim7_handle); // TIM7初始化                                    

    master_config.MasterOutputTrigger = TIM_TRGO_UPDATE; // 定时器7更新触发
    master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE; // 禁止同步模式
    HAL_TIMEx_MasterConfigSynchronization(&tim7_handle, &master_config); // 配置TIM7 TRGO
    HAL_TIM_Base_Start(&tim7_handle); 
    HAL_DAC_Stop_DMA(&dac_handle, (outx == 1)  ? DAC_CHANNEL_1 : DAC_CHANNEL_2); // 先停止之前的传输
    HAL_DAC_Start_DMA(&dac_handle, (outx == 1) ? DAC_CHANNEL_1 : DAC_CHANNEL_2, (uint32_t*)dac_sin_buf, ndtr, DAC_ALIGN_12B_R); // 启动DMA传输
}
```

#### 2.4.3 产生正弦波函序列

```c
/**
 * @brief       产生正弦波函序列
 *   @note      需保证 : maxval > samples/2
 * @param       maxval : 最大值(0 < maxval < 2048)
 * @param       samples: 采样点的个数
 * @retval      无
 */
void dac_creat_sin_buf(uint16_t maxval, uint16_t samples)
{
    uint8_t i;
    float inc = (2 * 3.1415962) / samples; // 计算增量（一个周期DAC_SIN_BUF个点）
    float outdata = 0;
    for (i = 0; i < samples; i++)
    {
        outdata = maxval * (1 + sin(inc * i)); // 计算以dots个点为周期的每个点的值，放大maxval倍，并偏移到正数区域
        if (outdata > 4095)
            outdata = 4095;                    
        //printf("%f\r\n", outdata);
        dac_sin_buf[i] = outdata;
    }
}

// 设置正弦波输出参数
void dac_dma_sin_set(uint16_t arr, uint16_t psc)
{
  dac_dma_wave_enable(1,100,arr,psc);
}
```

#### 2.4.4 主函数测试

```c
int main(void)
{
  uint8_t i = 0;
  uint8_t key_value = 0;
  float temp = 0;
  uint16_t adc_value = 0;
  bsp_init();
  adc_init();
  dac_dma_init(1);
  LCD_ShowString(30,50,200,16,16,"STM32F4 DAC SineWave");
  LCD_ShowString(30,110,200,16,16,"KEY0:3KHZ  KEY1:30KHZ");
  LCD_ShowString(30,130,200,16,16,"DAC VAL:");
  LCD_ShowString(30,150,200,16,16,"DAC VOL:0.000V");
  LCD_ShowString(30,170,200,16,16,"ADC VAL:0.000V");
  dac_creat_sin_buf(2048,100);
  dac_dma_wave_enable(1,100,10-1,84-1);
    while(1)
  {
    i++;
    key_value = key_scan(0);
    if(key_value == KEY0_Press)
    {
      dac_creat_sin_buf(2048,100);
      dac_dma_wave_enable(1,100,10-1,28-1);/* 300Khz触发频率, 100个点, 可以得到最高3KHz的正弦波. */
    }
    else if(key_value == KEY1_Press)
    {
      dac_creat_sin_buf(2048,10);
      dac_dma_wave_enable(1,10,10-1,28-1);/* 300Khz触发频率, 10个点, 可以得到最高30KHz的正弦波. */
    }
    // 显示DAC输出值
    adc_value = DAC1->DHR12R1;  
    LCD_ShowxNum(94,130,adc_value,4,16,0);
    temp = (float)adc_value*(3.3/4095);
    adc_value = temp;
    LCD_ShowxNum(94,150,temp,1,16,0);
    temp -= adc_value;
    temp *= 1000;
    LCD_ShowxNum(110,150,temp,3,16,0x80);
    // 显示ADC输入值
    adc_value = adc_get_result_average(ADC_CHANNEL_5 ,20);
    temp = (float)adc_value*(3.3/4095);
    adc_value = temp;
    LCD_ShowxNum(94,170,temp,1,16,0);
    temp -= adc_value;
    temp *= 1000;
    LCD_ShowxNum(110,170,temp,3,16,0x80);
    if(i == 10)
    {
      LED_TOGGLE(LED0_GPIO_Pin);
      i = 0;
    }
    delay_ms(5);
  }
}
```

## 3. DAC常见函数（HAL库）

### 3.1 DAC 初始化与配置

#### 3.1.1 `HAL_DAC_Init()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_DAC_Init(DAC_HandleTypeDef *hdac)
```

**参数**:

- `hdac`: DAC 句柄指针

**配置结构体**:

```c
typedef struct {
 DAC_TypeDef *Instance; // DAC实例 (DAC)
 __IO HAL_DAC_StateTypeDef State; // DAC状态
 HAL_LockTypeDef Lock; // 锁定对象
} DAC_HandleTypeDef;
```

**功能**: 初始化 DAC 外设  
**示例配置**:

```c
DAC_HandleTypeDef hdac1;

void DAC_Init(void) {
    __HAL_RCC_DAC_CLK_ENABLE();
    
    hdac1.Instance = DAC;
    if (HAL_DAC_Init(&hdac1) != HAL_OK) {
        Error_Handler();
    }
}
```

### 3.2 DAC 通道配置

#### 3.2.1 `HAL_DAC_ConfigChannel()`

**函数原型**:

```c
HAL_StatusTypeDef HAL_DAC_ConfigChannel(
 DAC_HandleTypeDef *hdac,
 DAC_ChannelConfTypeDef *sConfig,
 uint32_t Channel)
```

**参数**:

- `Channel`: 通道选择: `DAC_CHANNEL_1` 或 `DAC_CHANNEL_2`

- `sConfig`: 通道配置结构体指针

**通道配置结构体**:

```c
typedef struct {
 uint32_t DAC_Trigger; // 触发源:
 // DAC_TRIGGER_NONE (软件触发)
 // DAC_TRIGGER_T2_TRGO
 // DAC_TRIGGER_T4_TRGO
 // DAC_TRIGGER_T5_TRGO
 // DAC_TRIGGER_T6_TRGO
 // DAC_TRIGGER_T7_TRGO
 // DAC_TRIGGER_T8_TRGO
 // DAC_TRIGGER_EXT_IT9
 // DAC_TRIGGER_SOFTWARE
 uint32_t DAC_OutputBuffer; // 输出缓冲: 
// DAC_OUTPUTBUFFER_ENABLE/DISABLE
 uint32_t DAC_ConnectOnChipPeripheral; // 保留参数 (设为0)
 uint32_t DAC_UserTrimming; // 修整模式:
 // DAC_TRIMMING_FACTORY (出厂修整)
 // DAC_TRIMMING_USER (用户修整)
} DAC_ChannelConfTypeDef;
```

**示例配置**:

```c
DAC_ChannelConfTypeDef sConfig = {0};

// 配置DAC通道1
sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO; // TIM6触发
sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE; // 禁用输出缓冲
sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY; // 使用出厂修整值

if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK) {
 Error_Handler();
}
```

### 3.3 DAC 输出控制

#### 3.3.1 设置 DAC 值

```c
// 设置DAC通道值
HAL_StatusTypeDef HAL_DAC_SetValue(
 DAC_HandleTypeDef *hdac,
 uint32_t Channel,
 uint32_t Alignment,
 uint32_t Data)
```

**参数**:

- `Alignment`: 数据对齐方式
  
  - `DAC_ALIGN_12B_R`: 12位右对齐
  
  - `DAC_ALIGN_12B_L`: 12位左对齐
  
  - `DAC_ALIGN_8B_R`: 8位右对齐

- `Data`: 要转换的数字值 (0-4095 或 0-255)

#### 3.3.2 启动/停止 DAC 转换

```c
// 启动DAC转换
HAL_StatusTypeDef HAL_DAC_Start(
 DAC_HandleTypeDef *hdac,
 uint32_t Channel)

// 停止DAC转换
HAL_StatusTypeDef HAL_DAC_Stop(
 DAC_HandleTypeDef *hdac,
 uint32_t Channel)
```

**示例** (输出1.65V):

```c
// 设置DAC值 (3.3V参考电压下1.65V对应2048)
HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);

// 启动DAC转换 (软件触发)
HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
```

### 3.4 DAC 波形生成

#### 3.4.1 三角波生成

```c
HAL_StatusTypeDef HAL_DACEx_TriangleWaveGenerate(
 DAC_HandleTypeDef *hdac,
 uint32_t Channel,
 uint32_t Amplitude)
```

**参数**:

- `Amplitude`: 三角波幅度
  
  - `DAC_TRIANGLEAMPLITUDE_1`: 1
  
  - `DAC_TRIANGLEAMPLITUDE_3`: 3
  
  - ...
  
  - `DAC_TRIANGLEAMPLITUDE_4095`: 4095

#### 3.4.2 噪声波生成

```c
HAL_StatusTypeDef HAL_DACEx_NoiseWaveGenerate(
 DAC_HandleTypeDef *hdac,
 uint32_t Channel,
 uint32_t Amplitude)
```

**参数**: 同三角波

#### 3.4.3 双通道同步

```c
// 同时启动两个通道
HAL_DAC_Start(&hdac1, DAC_CHANNEL_1 | DAC_CHANNEL_2);

// 同时设置两个通道的值
HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value1);
HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, value2);
```

### 3.5 DAC DMA 传输

#### 3.5.1 启动 DMA 传输

```c
HAL_StatusTypeDef HAL_DAC_Start_DMA(
 DAC_HandleTypeDef *hdac,
 uint32_t Channel,
 uint32_t *pData,
 uint32_t Length,
 uint32_t Alignment)
```

**功能**: 通过 DMA 连续输出数据流  
**示例** (正弦波生成):

```c
#define SINE_WAVE_SAMPLES 128
uint32_t sine_wave[SINE_WAVE_SAMPLES];

// 生成正弦波表
void Generate_Sine_Wave(void) {
 for (int i = 0; i < SINE_WAVE_SAMPLES; i++) {
 sine_wave[i] = (uint32_t)(2047.5 * (1 + sin(2 * M_PI * i / SINE_WAVE_SAMPLES)));
 }
}

// 启动DAC DMA传输
HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, sine_wave, SINE_WAVE_SAMPLES, DAC_ALIGN_12B_R);
```

#### 3.5.2 停止 DMA 传输

```c
HAL_StatusTypeDef HAL_DAC_Stop_DMA(
 DAC_HandleTypeDef *hdac,
 uint32_t Channel)
```

### 3.6 DAC 校准

#### 3.6.1 修整值设置

```c
HAL_StatusTypeDef HAL_DACEx_SetUserTrimming(
 DAC_HandleTypeDef *hdac,
 uint32_t Channel,
 uint32_t NewTrimming)
```

#### 3.6.2 获取修整值

```c
uint32_t HAL_DACEx_GetTrimOffset(
 DAC_HandleTypeDef *hdac,
 uint32_t Channel,
 uint32_t Trimming)
```

#### 3.6.3 自校准

```c
HAL_StatusTypeDef HAL_DACEx_SelfCalibrate(
 DAC_HandleTypeDef *hdac,
 DAC_ChannelConfTypeDef *sConfig,
 uint32_t Channel)
```

**示例**:

```c
// 使用出厂修整值
DAC_ChannelConfTypeDef sConfig = {0};
sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1);
HAL_DACEx_SelfCalibrate(&hdac1, &sConfig, DAC_CHANNEL_1);
```

### 3.7 DAC 中断处理

#### 3.7.1 中断服务函数

```c
// DAC中断服务函数
void DAC_IRQHandler(void) {
 HAL_DAC_IRQHandler(&hdac1);
}
```

#### 3.7.2 回调函数

```c
// DMA传输完成回调
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
 // 处理DMA传输完成事件
}

// DMA半传输完成回调
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
 // 处理半传输完成事件
}

// 错误回调
void HAL_DAC_ErrorCallbackCh1(DAC_HandleTypeDef *hdac) {
 // 处理DAC错误
}
```

### 3.8 DAC 状态管理

#### 3.8.1 状态获取函数

```c
// 获取DAC状态
HAL_DAC_StateTypeDef HAL_DAC_GetState(DAC_HandleTypeDef *hdac)

// 获取错误代码
uint32_t HAL_DAC_GetError(DAC_HandleTypeDef *hdac)
```

#### 3.8.2 状态枚举

```c
HAL_DAC_STATE_RESET // 复位状态
HAL_DAC_STATE_READY // 就绪状态
HAL_DAC_STATE_BUSY // 忙状态
HAL_DAC_STATE_TIMEOUT // 超时状态
HAL_DAC_STATE_ERROR // 错误状态
```

---


