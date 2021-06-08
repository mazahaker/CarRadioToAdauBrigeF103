################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Users/Admin/Documents/STM32/LIB/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.cpp 

OBJS += \
./MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.o 

CPP_DEPS += \
./MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.d 


# Each subdirectory must supply rules for building sources it contributes
MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.o: C:/Users/Admin/Documents/STM32/LIB/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DUSE_HAL_DRIVER -DSTM32F103xB -DDEBUG -c -I../Core/Inc -I"C:/Users/Admin/Documents/STM32/LIB/SERIAL_LIB_STM32_F4" -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_EXT_CD_CHANGER_EMULATOR" -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_CAR_RADIO_DSP_COMMAND_RECIVER" -I"C:/Users/Admin/Documents/STM32/LIB/ADAU_1701_LIB_STM32_F4" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

