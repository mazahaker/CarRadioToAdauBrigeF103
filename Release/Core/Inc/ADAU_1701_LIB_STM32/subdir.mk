################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Inc/ADAU_1701_LIB_STM32/DSPEEPROM.cpp \
../Core/Inc/ADAU_1701_LIB_STM32/SigmaDSP.cpp 

OBJS += \
./Core/Inc/ADAU_1701_LIB_STM32/DSPEEPROM.o \
./Core/Inc/ADAU_1701_LIB_STM32/SigmaDSP.o 

CPP_DEPS += \
./Core/Inc/ADAU_1701_LIB_STM32/DSPEEPROM.d \
./Core/Inc/ADAU_1701_LIB_STM32/SigmaDSP.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Inc/ADAU_1701_LIB_STM32/%.o Core/Inc/ADAU_1701_LIB_STM32/%.su: ../Core/Inc/ADAU_1701_LIB_STM32/%.cpp Core/Inc/ADAU_1701_LIB_STM32/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Inc-2f-ADAU_1701_LIB_STM32

clean-Core-2f-Inc-2f-ADAU_1701_LIB_STM32:
	-$(RM) ./Core/Inc/ADAU_1701_LIB_STM32/DSPEEPROM.d ./Core/Inc/ADAU_1701_LIB_STM32/DSPEEPROM.o ./Core/Inc/ADAU_1701_LIB_STM32/DSPEEPROM.su ./Core/Inc/ADAU_1701_LIB_STM32/SigmaDSP.d ./Core/Inc/ADAU_1701_LIB_STM32/SigmaDSP.o ./Core/Inc/ADAU_1701_LIB_STM32/SigmaDSP.su

.PHONY: clean-Core-2f-Inc-2f-ADAU_1701_LIB_STM32

