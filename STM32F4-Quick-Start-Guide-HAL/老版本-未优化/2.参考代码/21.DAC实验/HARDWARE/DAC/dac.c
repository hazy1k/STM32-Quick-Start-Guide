#include "dac.h"

DAC_HandleTypeDef DAC1_Handler; // DAC句柄
// 初始化DAC
void DAC1_Init(void)
{
    DAC_ChannelConfTypeDef DACCH1_Config; 
    DAC1_Handler.Instance = DAC;
    HAL_DAC_Init(&DAC1_Handler); // 初始化DAC
    DACCH1_Config.DAC_Trigger = DAC_TRIGGER_NONE;             // 不使用触发功能
    DACCH1_Config.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;// DAC1输出缓冲关闭
    HAL_DAC_ConfigChannel(&DAC1_Handler,&DACCH1_Config,DAC_CHANNEL_1);// DAC通道1配置
    HAL_DAC_Start(&DAC1_Handler,DAC_CHANNEL_1);  // 开启DAC通道1
}
// DAC底层驱动，时钟配置，引脚 配置
// 此函数会被HAL_DAC_Init()调用
// hdac:DAC句柄
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{      
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_DAC_CLK_ENABLE();          // 使能DAC时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();		 // 开启GPIOA时钟	
    GPIO_Initure.Pin = GPIO_PIN_4;       // PA4
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;// 模拟
    GPIO_Initure.Pull = GPIO_NOPULL;     // 不带上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}
// 设置通道1输出电压
// vol:0~3300,代表0~3.3V
void DAC1_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
    HAL_DAC_SetValue(&DAC1_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);//12位右对齐数据格式设置DAC值
}
