#include "delay.h"

static uint32_t fac_us = 0;
static uint32_t fac_ms = 0;

void Delay_Init(void) {
    // 使用SysTick实现精确延时
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    fac_us = SystemCoreClock / 8000000;
    fac_ms = fac_us * 1000;
}

void Delay_us(uint32_t us) {
    uint32_t temp;
    SysTick->LOAD = us * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    do {
        temp = SysTick->CTRL;
    } while((temp & 0x01) && !(temp & (1 << 16)));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

void Delay_ms(uint32_t ms) {
    uint32_t temp;
    while(ms--) {
        SysTick->LOAD = 1000 * fac_us;
        SysTick->VAL = 0x00;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        do {
            temp = SysTick->CTRL;
        } while((temp & 0x01) && !(temp & (1 << 16)));
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        SysTick->VAL = 0x00;
    }
}

// 在delay.c中实现
static uint32_t sysTick = 0;

void SysTick_Handler(void) {
    sysTick++;
}

uint32_t GetTick(void) {
    return sysTick;
}
