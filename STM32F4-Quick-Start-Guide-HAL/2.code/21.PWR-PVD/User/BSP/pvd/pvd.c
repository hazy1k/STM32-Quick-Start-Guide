#include "pvd.h"
#include "led.h"
#include "lcd.h"

void pwr_pvd_init(uint32_t pls)
{
    PWR_PVDTypeDef pvd_handle = {0};
    __HAL_RCC_PWR_CLK_ENABLE();
    pvd_handle.PVDLevel = pls; // 设置PVD电压监视器等级
    HAL_PWR_ConfigPVD(&pvd_handle); // 配置PVD
    HAL_NVIC_SetPriority(PVD_IRQn,3,3);
    HAL_NVIC_EnableIRQ(PVD_IRQn); // 使能PVD中断
    HAL_PWR_EnablePVD();
}

void PVD_IRQHandler(void)
{
    HAL_PWR_PVD_IRQHandler();
}

void HAL_PWR_PVDCallback(void)
{
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_PVDO)) // 电压比PLS所选电压更低，触发中断
    {
        LCD_ShowString(30,130,200,16,16,"PVD Low Voltage Detected!");
        LED_ON(LED1_GPIO_Pin);
    }
    else
    {
        LCD_ShowString(30,130,200,16,16,"PVD Normal Voltage!");
        LED_OFF(LED1_GPIO_Pin);
    }
}
