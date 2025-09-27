#ifndef __BSP_ADC_H
#define	__BSP_ADC_H

#include "stm32f1xx.h"

// ADC 序号宏定义
#define CURR_ADC                        ADC1
#define CURR_ADC_CLK_ENABLE()           __HAL_RCC_ADC1_CLK_ENABLE()

#define ADC_VBUS_IRQ                    ADC1_IRQn
#define ADC_VBUS_IRQHandler             ADC1_IRQHandler

#define VREF                            3.3f     // 参考电压，理论上是3.3，可通过实际测量得3.258
#define ADC_NUM_MAX                     1024       // ADC 转换结果缓冲区最大值

#define GET_ADC_VDC_VAL(val)            ((float)val/(float)4096.0*VREF)          // 得到电压值
  
/*********************** 电流采集 ******************/
// ADC GPIO 宏定义
#define TEMP_ADC_GPIO_PORT              GPIOC
#define TEMP_ADC_GPIO_PIN               GPIO_PIN_0
#define TEMP_ADC_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()

#define TEMP_ADC_CHANNEL                ADC_CHANNEL_10

// ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
#define TEMP_ADC_DR_ADDR                ((uint32_t)ADC1+0x4c)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define TEMP_ADC_DMA_CLK_ENABLE()       __HAL_RCC_DMA1_CLK_ENABLE()
#define TEMP_ADC_DMA_STREAM             DMA1_Channel1

#define ADC_DMA_IRQ                     DMA1_Channel1_IRQn
#define ADC_DMA_IRQ_Handler             DMA1_Channel1_IRQHandler

/*********************** 电源电压采集 ******************/

#define VBUS_GPIO_PORT                  GPIOC
#define VBUS_GPIO_PIN                   GPIO_PIN_1
#define VBUS_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOC_CLK_ENABLE()

#define VBUS_ADC_CHANNEL                ADC_CHANNEL_11

#define GET_VBUS_VAL(val)               (((float)val-(float)1.24f) * (float)37.0f)      // 电压最大值（测量电压是电源电压的1/500）

#define VBUS_MAX                        30    // 电压最大值
#define VBUS_MIN                        15    // 电压最小值

#define VBUS_HEX_MAX                    ((VBUS_MAX/37.0+1.24)/VREF*4096)    // 电压最大值（测量电压是电源电压的1/37）
#define VBUS_HEX_MIN                    ((VBUS_MIN/37.0+1.24)/VREF*4096)    // 电压最小值（测量电压是电源电压的1/37）

extern DMA_HandleTypeDef DMA_Init_Handle;
extern ADC_HandleTypeDef ADC_Handle;

void ADC_Init(void);
float get_ntc_v_val(void);
float get_ntc_r_val(void);
float get_ntc_t_val(void);
float get_vbus_val(void);
 
#endif /* __BSP_ADC_H */
