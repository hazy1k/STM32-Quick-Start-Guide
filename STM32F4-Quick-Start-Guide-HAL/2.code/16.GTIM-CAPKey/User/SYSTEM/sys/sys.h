#ifndef _SYS_H
#define _SYS_H

#include <stm32f4xx.h>
#include <core_cm4.h>
#include <stm32f4xx_hal.h>


#define SYS_SUPPORT_OS         0

void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);                         
void sys_standby(void);                                                                  
void sys_soft_reset(void);                                                                  
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq);   


void sys_wfi_set(void);           
void sys_intx_disable(void);        
void sys_intx_enable(void);         
void sys_msr_msp(uint32_t addr);

#endif
