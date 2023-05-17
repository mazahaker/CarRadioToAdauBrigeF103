################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/Users/Admin/Documents/STM32/LIB/ADAU_1701_LIB_STM32_F4/DSPEEPROM.cpp \
C:/Users/Admin/Documents/STM32/LIB/ADAU_1701_LIB_STM32_F4/SigmaDSP.cpp 

OBJS += \
./ADAU_1701_LIB_STM32_F4/DSPEEPROM.o \
./ADAU_1701_LIB_STM32_F4/SigmaDSP.o 

CPP_DEPS += \
./ADAU_1701_LIB_STM32_F4/DSPEEPROM.d \
./ADAU_1701_LIB_STM32_F4/SigmaDSP.d 


# Each subdirectory must supply rules for building sources it contributes
ADAU_1701_LIB_STM32_F4/DSPEEPROM.o: C:/Users/Admin/Documents/STM32/LIB/ADAU_1701_LIB_STM32_F4/DSPEEPROM.cpp ADAU_1701_LIB_STM32_F4/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_EXT_CD_CHANGER_EMULATOR" -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_CAR_RADIO_DSP_COMMAND_RECIVER" -I"C:/Users/Admin/Documents/STM32/LIB/ADAU_1701_LIB_STM32_F4" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
ADAU_1701_LIB_STM32_F4/SigmaDSP.o: C:/Users/Admin/Documents/STM32/LIB/ADAU_1701_LIB_STM32_F4/SigmaDSP.cpp ADAU_1701_LIB_STM32_F4/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_EXT_CD_CHANGER_EMULATOR" -I"C:/Users/Admin/Documents/STM32/LIB/MAZDA_CAR_RADIO_DSP_COMMAND_RECIVER" -I"C:/Users/Admin/Documents/STM32/LIB/ADAU_1701_LIB_STM32_F4" -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-ADAU_1701_LIB_STM32_F4

clean-ADAU_1701_LIB_STM32_F4:
	-$(RM) ./ADAU_1701_LIB_STM32_F4/DSPEEPROM.d ./ADAU_1701_LIB_STM32_F4/DSPEEPROM.o ./ADAU_1701_LIB_STM32_F4/DSPEEPROM.su ./ADAU_1701_LIB_STM32_F4/SigmaDSP.d ./ADAU_1701_LIB_STM32_F4/SigmaDSP.o ./ADAU_1701_LIB_STM32_F4/SigmaDSP.su

.PHONY: clean-ADAU_1701_LIB_STM32_F4

