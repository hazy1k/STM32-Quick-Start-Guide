# 第十七章 ADC应用补充-内部温度与光敏

## 1. 内部温度传感器

### 1.1 内部温度传感器简介

STM32F407 有一个内部的温度传感器，可以用来测量 CPU 及周围的温度(TA)。 对于STM32F407 系列来说， 该温度传感器在内部和 ADC1_INP16（ STM32F40xx/F41xx 系列）或ADC_IN18（STM32F42xx/F43xx） 输入通道相连接，此通道把传感器输出的电压转换成数字值。STM32F4 的内部温度传感器支持的温度范围为： -40~125 度。精度为±1.5℃左右。

STM32F407 内部温度传感器的使用很简单，只要设置一下内部 ADC，并激活其内部温度传感器通道就差不多了。关于 ADC 的设置，我们在上一章已经进行了详细的介绍，这里就不再多说。接下来我们介绍一下和温度传感器设置相关的两个地方。

第一个地方，我们要使用 STM32F407 的内部温度传感器，必须先激活 ADC 的内部通道，这里通过 ADC_CCR 的 VSENSEEN 位（bit23）设置。设置该位为 1 则启用内部温度传感器。

第二个地方， STM32F407ZGT6 的内部温度传感器固定的连接在 ADC1 的通道 16 上，所以，我们在设置好 ADC1 之后只要读取通道 16 的值，就是温度传感器返回来的电压值了。根据这个值，我们就可以计算出当前温度。计算公式如下：

```c
𝑇(℃) ={(Vsense - V25) / Avg_Slope}+25
```

式子中：

V25 = Vsense 在 25 度时的数值（典型值为： 0.76）

Avg_Slope = 温度与 Vsense 曲线的平均斜率（单位： mv/℃或 uv/℃）（典型值： 2.5mv/℃）。利用以上公式，我们就可以方便的计算出当前温度传感器的温度了。

### 1.2 软件设计

#### 1.2.1 内部温度传感器初始化

```c
// 内部温度传感器初始化
void adc_temp_init(void)
{
    adc_init();
    ADC->CCR |= 1<<23;
}
```

#### 1.2.2 获取内部温度传感器温度值

```c
// 获取内部温度传感器温度值
short adc_get_temp(void)
{
    uint32_t adc_val = 0;
    short result = 0;
    double temp;
    adc_val = adc_get_result_average(ADC_CHANNEL_TEMPSENSOR, 10);
    temp = (float)adc_val*(3.3/4095);
    temp = (temp - 0.76)/0.0025+25;
    temp *= 100;
    result = temp;
    return result;
}
```

#### 1.2.3 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"

int main(void)
{
  short temp;
  bsp_init();
  adc_temp_init();
  LCD_ShowString(30,50,200,16,16,"STM32F4 ADC Temperature");
  LCD_ShowString(30,120,200,16,16, "TEMPERATE: 00.00C");
    while(1)
  {
    temp = adc_get_temp();
    if(temp < 0)
    {
      temp = -temp;
      LCD_ShowString(110,120,16,16,16,"-");
    }
    else
    {
      LCD_ShowString(110,120,16,16,16," ");
    }
    LCD_ShowxNum(118,120,temp/100,2,16,0);
    LCD_ShowxNum(142,120,temp%100,2,16,0x80);
    LED_TOGGLE(LED0_GPIO_Pin);
    delay_ms(100);
  }
}
```

## 2. 光敏传感器

### 2.1 光敏传感器简介

光敏传感器是最常见的传感器之一，它的种类繁多，主要有：光电管、光电倍增管、光敏电阻、光敏三极管、太阳能电池、红外线传感器、紫外线传感器、光纤式光电传感器、色彩传感器、 CCD 和 CMOS 图像传感器等。光传感器是目前产量最多、应用最广的传感器之一，它在自动控制和非电量电测技术中占有非常重要的地位。

光敏传感器是利用光敏元件将光信号转换为电信号的传感器，它的敏感波长在可见光波长附近，包括红外线波长和紫外线波长。光传感器不只局限于对光的探测，它还可以作为探测元件组成其他传感器，对许多非电量进行检测，只要将这些非电量转换为光信号的变化即可。

探索者 STM32F407 开发板板载了一个光敏二极管（光敏电阻），作为光敏传感器， 它对光的变化非常敏感。 光敏二极管也叫光电二极管。光敏二极管与半导体二极管在结构上是类似的，其管芯是一个具有光敏特征的 PN 结，具有单向导电性，因此工作时需加上反向电压。无光照时，有很小的饱和反向漏电流，即暗电流，此时光敏二极管截止。当受到光照时， 饱和反向漏电流大大增加，形成光电流,它随入射光强度的变化而变化。当光线照射 PN 结时，可以使 PN结中产生电子一空穴对，使少数载流子的密度增加。这些载流子在反向电压下漂移，使反向电流增加。因此可以利用光照强弱来改变电路中的电流。

利用这个电流变化，我们串接一个电阻，就可以转换成电压的变化，从而通过 ADC 读取电压值，判断外部光线的强弱。

本章，我们利用 ADC3 的通道 5（PF7）来读取光敏二极管电压的变化，从而得到环境光线的变化，并将得到的光线强度，显示在 TFTLCD 上面。关于 ADC 的介绍，前面已经有详细介绍了，这里我们就不再细说了。

![屏幕截图 2025-08-10 124549.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/10-12-48-17-屏幕截图%202025-08-10%20124549.png)

### 2.2 软件设计

#### 2.2.1 光敏传感器初始化

```c
// 初始化光敏传感器-PF7
void light_sensor_init(void)
{
    GPIO_InitTypeDef gpio_init_structure;
    __HAL_RCC_GPIOF_CLK_ENABLE();
    gpio_init_structure.Pin = GPIO_PIN_7;
    gpio_init_structure.Mode = GPIO_MODE_ANALOG;
    gpio_init_structure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &gpio_init_structure);
    adc_init();
}
```

#### 2.2.2 读取光敏传感器值

```c
// 读取光敏传感器值
uint8_t light_sensor_read(void)
{
    uint32_t temp_val = 0;
    temp_val = adc_get_result_average(ADC_CHANNEL_5, 10);
    temp_val /= 40;
    if(temp_val > 100)
    {
        temp_val = 100;
    }
    return (uint8_t)(100-temp_val);
}

```

#### 2.2.3 主函数测试

```c
#include "bsp_init.h"
#include "stdio.h"
#include "adc.h"

int main(void)
{
  uint16_t temp;
  bsp_init();
  light_sensor_init();
  LCD_ShowString(30,50,200,16,16,"LIGHT SENSOR TEST");
  LCD_ShowString(30,110,200,16,16, "LIGHT_VAL:");
	while(1)
  {
    temp = light_sensor_read();
    LCD_ShowxNum(110,110,temp,3,16,0);
    LED_TOGGLE(LED0_GPIO_Pin);
    delay_ms(100);
  }
}

```

---
