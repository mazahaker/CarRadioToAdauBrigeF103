################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Users/Admin/Documents/STM32/LIB/SERIAL_LIB_STM32_F4/Serial.cpp 

OBJS += \
./SERIAL_LIB_STM32_F4/Serial.o 

CPP_DEPS += \
./SERIAL_LIB_STM32_F4/Serial.d 


# Each subdirectory must supply rules for building sources it contributes
SERIAL_LIB_STM32_F4/Serial.o: C:/Users/Admin/Documents/STM32/LIB/SERIAL_LIB_STM32_F4/Serial.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"C:/Users/Admin/Documents/STM32/LIB/SERIAL_LIB_STM32_F4" -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_EXT_CD_CHANGER_EMULATOR" -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_CAR_RADIO_DSP_COMMAND_RECIVER" -I"C:/Users/Admin/Documents/STM32/LIB/ADAU_1701_LIB_STM32_F4" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"SERIAL_LIB_STM32_F4/Serial.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

