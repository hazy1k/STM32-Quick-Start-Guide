################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/KEY/key.c 

OBJS += \
./BSP/KEY/key.o 

C_DEPS += \
./BSP/KEY/key.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/KEY/%.o BSP/KEY/%.su BSP/KEY/%.cyclo: ../BSP/KEY/%.c BSP/KEY/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"E:/StudyNote/study_STM32H7-MX/2.code/18.RTC/BSP" -I"E:/StudyNote/study_STM32H7-MX/2.code/18.RTC/BSP/LED" -I"E:/StudyNote/study_STM32H7-MX/2.code/18.RTC/BSP/KEY" -I"E:/StudyNote/study_STM32H7-MX/2.code/18.RTC/BSP/BEEP" -I"E:/StudyNote/study_STM32H7-MX/2.code/18.RTC/BSP/EXTI" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-KEY

clean-BSP-2f-KEY:
	-$(RM) ./BSP/KEY/key.cyclo ./BSP/KEY/key.d ./BSP/KEY/key.o ./BSP/KEY/key.su

.PHONY: clean-BSP-2f-KEY

