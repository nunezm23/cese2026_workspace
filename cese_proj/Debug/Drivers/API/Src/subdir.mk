################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/API/Src/API_delay.c \
../Drivers/API/Src/API_lcd.c \
../Drivers/API/Src/API_mcp23s17.c 

OBJS += \
./Drivers/API/Src/API_delay.o \
./Drivers/API/Src/API_lcd.o \
./Drivers/API/Src/API_mcp23s17.o 

C_DEPS += \
./Drivers/API/Src/API_delay.d \
./Drivers/API/Src/API_lcd.d \
./Drivers/API/Src/API_mcp23s17.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/API/Src/%.o Drivers/API/Src/%.su Drivers/API/Src/%.cyclo: ../Drivers/API/Src/%.c Drivers/API/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/Inc" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/Src" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/APP" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/APP/Inc" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/APP/Src" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/ARCH" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/ARCH/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-API-2f-Src

clean-Drivers-2f-API-2f-Src:
	-$(RM) ./Drivers/API/Src/API_delay.cyclo ./Drivers/API/Src/API_delay.d ./Drivers/API/Src/API_delay.o ./Drivers/API/Src/API_delay.su ./Drivers/API/Src/API_lcd.cyclo ./Drivers/API/Src/API_lcd.d ./Drivers/API/Src/API_lcd.o ./Drivers/API/Src/API_lcd.su ./Drivers/API/Src/API_mcp23s17.cyclo ./Drivers/API/Src/API_mcp23s17.d ./Drivers/API/Src/API_mcp23s17.o ./Drivers/API/Src/API_mcp23s17.su

.PHONY: clean-Drivers-2f-API-2f-Src

