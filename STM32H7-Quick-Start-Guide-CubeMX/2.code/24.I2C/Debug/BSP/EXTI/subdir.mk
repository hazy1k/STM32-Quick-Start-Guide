################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/EXTI/exti.c 

OBJS += \
./BSP/EXTI/exti.o 

C_DEPS += \
./BSP/EXTI/exti.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/EXTI/%.o BSP/EXTI/%.su BSP/EXTI/%.cyclo: ../BSP/EXTI/%.c BSP/EXTI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"E:/StudyNote/study_STM32H7-MX/2.code/24.I2C/BSP" -I"E:/StudyNote/study_STM32H7-MX/2.code/24.I2C/BSP/LED" -I"E:/StudyNote/study_STM32H7-MX/2.code/24.I2C/BSP/KEY" -I"E:/StudyNote/study_STM32H7-MX/2.code/24.I2C/BSP/BEEP" -I"E:/StudyNote/study_STM32H7-MX/2.code/24.I2C/BSP/EXTI" -I"E:/StudyNote/study_STM32H7-MX/2.code/24.I2C/BSP/I2C" -I"E:/StudyNote/study_STM32H7-MX/2.code/24.I2C/BSP/Delay" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-EXTI

clean-BSP-2f-EXTI:
	-$(RM) ./BSP/EXTI/exti.cyclo ./BSP/EXTI/exti.d ./BSP/EXTI/exti.o ./BSP/EXTI/exti.su

.PHONY: clean-BSP-2f-EXTI

