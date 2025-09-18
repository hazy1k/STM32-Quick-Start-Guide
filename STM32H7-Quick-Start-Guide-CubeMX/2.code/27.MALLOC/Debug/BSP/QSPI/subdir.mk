################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/QSPI/flash.c \
../BSP/QSPI/flash_ex.c 

OBJS += \
./BSP/QSPI/flash.o \
./BSP/QSPI/flash_ex.o 

C_DEPS += \
./BSP/QSPI/flash.d \
./BSP/QSPI/flash_ex.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/QSPI/%.o BSP/QSPI/%.su BSP/QSPI/%.cyclo: ../BSP/QSPI/%.c BSP/QSPI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"E:/StudyNote/study_STM32H7-MX/2.code/27.MALLOC/BSP" -I"E:/StudyNote/study_STM32H7-MX/2.code/27.MALLOC/BSP/LED" -I"E:/StudyNote/study_STM32H7-MX/2.code/27.MALLOC/BSP/KEY" -I"E:/StudyNote/study_STM32H7-MX/2.code/27.MALLOC/BSP/BEEP" -I"E:/StudyNote/study_STM32H7-MX/2.code/27.MALLOC/BSP/EXTI" -I"E:/StudyNote/study_STM32H7-MX/2.code/27.MALLOC/BSP/I2C" -I"E:/StudyNote/study_STM32H7-MX/2.code/27.MALLOC/BSP/Delay" -I"E:/StudyNote/study_STM32H7-MX/2.code/27.MALLOC/BSP/QSPI" -I"E:/StudyNote/study_STM32H7-MX/2.code/27.MALLOC/BSP/FLASH" -I"E:/StudyNote/study_STM32H7-MX/2.code/27.MALLOC/BSP/MALLOC" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-QSPI

clean-BSP-2f-QSPI:
	-$(RM) ./BSP/QSPI/flash.cyclo ./BSP/QSPI/flash.d ./BSP/QSPI/flash.o ./BSP/QSPI/flash.su ./BSP/QSPI/flash_ex.cyclo ./BSP/QSPI/flash_ex.d ./BSP/QSPI/flash_ex.o ./BSP/QSPI/flash_ex.su

.PHONY: clean-BSP-2f-QSPI

