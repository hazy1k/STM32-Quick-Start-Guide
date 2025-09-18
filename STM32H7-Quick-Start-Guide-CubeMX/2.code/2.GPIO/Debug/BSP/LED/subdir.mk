################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/LED/led.c 

OBJS += \
./BSP/LED/led.o 

C_DEPS += \
./BSP/LED/led.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/LED/%.o BSP/LED/%.su BSP/LED/%.cyclo: ../BSP/LED/%.c BSP/LED/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"E:/StudyNote/study_STM32H7-MX/2.code/2.GPIO/BSP" -I"E:/StudyNote/study_STM32H7-MX/2.code/2.GPIO/BSP/LED" -I"E:/StudyNote/study_STM32H7-MX/2.code/2.GPIO/BSP/KEY" -I"E:/StudyNote/study_STM32H7-MX/2.code/2.GPIO/BSP/BEEP" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-LED

clean-BSP-2f-LED:
	-$(RM) ./BSP/LED/led.cyclo ./BSP/LED/led.d ./BSP/LED/led.o ./BSP/LED/led.su

.PHONY: clean-BSP-2f-LED

