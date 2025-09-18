################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/bsp_init.c 

OBJS += \
./BSP/bsp_init.o 

C_DEPS += \
./BSP/bsp_init.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/%.o BSP/%.su BSP/%.cyclo: ../BSP/%.c BSP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"E:/StudyNote/study_STM32H7-MX/2.code/4.USART/BSP" -I"E:/StudyNote/study_STM32H7-MX/2.code/4.USART/BSP/LED" -I"E:/StudyNote/study_STM32H7-MX/2.code/4.USART/BSP/KEY" -I"E:/StudyNote/study_STM32H7-MX/2.code/4.USART/BSP/BEEP" -I"E:/StudyNote/study_STM32H7-MX/2.code/4.USART/BSP/EXTI" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP

clean-BSP:
	-$(RM) ./BSP/bsp_init.cyclo ./BSP/bsp_init.d ./BSP/bsp_init.o ./BSP/bsp_init.su

.PHONY: clean-BSP

