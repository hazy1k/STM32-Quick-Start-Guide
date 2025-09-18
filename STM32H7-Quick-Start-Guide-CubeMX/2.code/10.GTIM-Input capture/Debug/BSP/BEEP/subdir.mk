################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/BEEP/beep.c 

OBJS += \
./BSP/BEEP/beep.o 

C_DEPS += \
./BSP/BEEP/beep.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/BEEP/%.o BSP/BEEP/%.su BSP/BEEP/%.cyclo: ../BSP/BEEP/%.c BSP/BEEP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"E:/StudyNote/study_STM32H7-MX/2.code/10.GTIM-Input capture/BSP" -I"E:/StudyNote/study_STM32H7-MX/2.code/10.GTIM-Input capture/BSP/LED" -I"E:/StudyNote/study_STM32H7-MX/2.code/10.GTIM-Input capture/BSP/KEY" -I"E:/StudyNote/study_STM32H7-MX/2.code/10.GTIM-Input capture/BSP/BEEP" -I"E:/StudyNote/study_STM32H7-MX/2.code/10.GTIM-Input capture/BSP/EXTI" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-BEEP

clean-BSP-2f-BEEP:
	-$(RM) ./BSP/BEEP/beep.cyclo ./BSP/BEEP/beep.d ./BSP/BEEP/beep.o ./BSP/BEEP/beep.su

.PHONY: clean-BSP-2f-BEEP

