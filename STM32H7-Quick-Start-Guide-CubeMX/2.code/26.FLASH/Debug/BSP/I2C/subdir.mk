################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/I2C/eeprom.c \
../BSP/I2C/i2c.c 

OBJS += \
./BSP/I2C/eeprom.o \
./BSP/I2C/i2c.o 

C_DEPS += \
./BSP/I2C/eeprom.d \
./BSP/I2C/i2c.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/I2C/%.o BSP/I2C/%.su BSP/I2C/%.cyclo: ../BSP/I2C/%.c BSP/I2C/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"E:/StudyNote/study_STM32H7-MX/2.code/26.FLASH/BSP" -I"E:/StudyNote/study_STM32H7-MX/2.code/26.FLASH/BSP/LED" -I"E:/StudyNote/study_STM32H7-MX/2.code/26.FLASH/BSP/KEY" -I"E:/StudyNote/study_STM32H7-MX/2.code/26.FLASH/BSP/BEEP" -I"E:/StudyNote/study_STM32H7-MX/2.code/26.FLASH/BSP/EXTI" -I"E:/StudyNote/study_STM32H7-MX/2.code/26.FLASH/BSP/I2C" -I"E:/StudyNote/study_STM32H7-MX/2.code/26.FLASH/BSP/Delay" -I"E:/StudyNote/study_STM32H7-MX/2.code/26.FLASH/BSP/QSPI" -I"E:/StudyNote/study_STM32H7-MX/2.code/26.FLASH/BSP/FLASH" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-I2C

clean-BSP-2f-I2C:
	-$(RM) ./BSP/I2C/eeprom.cyclo ./BSP/I2C/eeprom.d ./BSP/I2C/eeprom.o ./BSP/I2C/eeprom.su ./BSP/I2C/i2c.cyclo ./BSP/I2C/i2c.d ./BSP/I2C/i2c.o ./BSP/I2C/i2c.su

.PHONY: clean-BSP-2f-I2C

