################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Users/Admin/Documents/STM32/LIB/SERIAL_LIB_STM32_F4/Serial.cpp 

OBJS += \
./SERIAL_LIB_STM32_F4/Serial.o 

CPP_DEPS += \
./SERIAL_LIB_STM32_F4/Serial.d 


# Each subdirectory must supply rules for building sources it contributes
SERIAL_LIB_STM32_F4/Serial.o: C:/Users/Admin/Documents/STM32/LIB/SERIAL_LIB_STM32_F4/Serial.cpp SERIAL_LIB_STM32_F4/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DSTM32F103xB -DDEBUG -c -I../Core/Inc -I"C:/Users/Admin/Documents/STM32/LIB/SERIAL_LIB_STM32_F4" -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_EXT_CD_CHANGER_EMULATOR" -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_CAR_RADIO_DSP_COMMAND_RECIVER" -I"C:/Users/Admin/Documents/STM32/LIB/ADAU_1701_LIB_STM32_F4" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-SERIAL_LIB_STM32_F4

clean-SERIAL_LIB_STM32_F4:
	-$(RM) ./SERIAL_LIB_STM32_F4/Serial.d ./SERIAL_LIB_STM32_F4/Serial.o ./SERIAL_LIB_STM32_F4/Serial.su

.PHONY: clean-SERIAL_LIB_STM32_F4

