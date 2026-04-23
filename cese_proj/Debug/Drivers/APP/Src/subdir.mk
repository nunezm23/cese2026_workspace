################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/APP/Src/APP_system.c 

OBJS += \
./Drivers/APP/Src/APP_system.o 

C_DEPS += \
./Drivers/APP/Src/APP_system.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/APP/Src/%.o Drivers/APP/Src/%.su Drivers/APP/Src/%.cyclo: ../Drivers/APP/Src/%.c Drivers/APP/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/Inc" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/Src" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/APP" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/APP/Inc" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/APP/Src" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/ARCH" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/ARCH/Inc" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/ARCH/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-APP-2f-Src

clean-Drivers-2f-APP-2f-Src:
	-$(RM) ./Drivers/APP/Src/APP_system.cyclo ./Drivers/APP/Src/APP_system.d ./Drivers/APP/Src/APP_system.o ./Drivers/APP/Src/APP_system.su

.PHONY: clean-Drivers-2f-APP-2f-Src

