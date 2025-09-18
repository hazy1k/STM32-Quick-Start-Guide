################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/PWR/pwr.c 

OBJS += \
./BSP/PWR/pwr.o 

C_DEPS += \
./BSP/PWR/pwr.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/PWR/%.o BSP/PWR/%.su BSP/PWR/%.cyclo: ../BSP/PWR/%.c BSP/PWR/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"E:/StudyNote/study_STM32H7-MX/2.code/21.DMA/BSP" -I"E:/StudyNote/study_STM32H7-MX/2.code/21.DMA/BSP/LED" -I"E:/StudyNote/study_STM32H7-MX/2.code/21.DMA/BSP/KEY" -I"E:/StudyNote/study_STM32H7-MX/2.code/21.DMA/BSP/BEEP" -I"E:/StudyNote/study_STM32H7-MX/2.code/21.DMA/BSP/EXTI" -I"E:/StudyNote/study_STM32H7-MX/2.code/21.DMA/BSP/PWR" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-PWR

clean-BSP-2f-PWR:
	-$(RM) ./BSP/PWR/pwr.cyclo ./BSP/PWR/pwr.d ./BSP/PWR/pwr.o ./BSP/PWR/pwr.su

.PHONY: clean-BSP-2f-PWR

