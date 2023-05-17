################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.cpp 

OBJS += \
./Core/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.o 

CPP_DEPS += \
./Core/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/MAZDA_EXT_CD_CHANGER_EMULATOR/%.o Core/MAZDA_EXT_CD_CHANGER_EMULATOR/%.su: ../Core/MAZDA_EXT_CD_CHANGER_EMULATOR/%.cpp Core/MAZDA_EXT_CD_CHANGER_EMULATOR/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"C:/Users/Admin/Documents/STM32/LIB/SERIAL_LIB_STM32_F4" -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_EXT_CD_CHANGER_EMULATOR" -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_CAR_RADIO_DSP_COMMAND_RECIVER" -I"C:/Users/Admin/Documents/STM32/LIB/ADAU_1701_LIB_STM32_F4" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-MAZDA_EXT_CD_CHANGER_EMULATOR

clean-Core-2f-MAZDA_EXT_CD_CHANGER_EMULATOR:
	-$(RM) ./Core/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.d ./Core/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.o ./Core/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.su

.PHONY: clean-Core-2f-MAZDA_EXT_CD_CHANGER_EMULATOR

