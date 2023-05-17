################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Inc/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.cpp 

OBJS += \
./Core/Inc/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.o 

CPP_DEPS += \
./Core/Inc/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/MAZDA_EXT_CD_CHANGER_EMULATOR/%.o Core/Inc/MAZDA_EXT_CD_CHANGER_EMULATOR/%.su: ../Core/Inc/MAZDA_EXT_CD_CHANGER_EMULATOR/%.cpp Core/Inc/MAZDA_EXT_CD_CHANGER_EMULATOR/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Inc-2f-MAZDA_EXT_CD_CHANGER_EMULATOR

clean-Core-2f-Inc-2f-MAZDA_EXT_CD_CHANGER_EMULATOR:
	-$(RM) ./Core/Inc/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.d ./Core/Inc/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.o ./Core/Inc/MAZDA_EXT_CD_CHANGER_EMULATOR/ExtDeviceCommandReciver.su

.PHONY: clean-Core-2f-Inc-2f-MAZDA_EXT_CD_CHANGER_EMULATOR

