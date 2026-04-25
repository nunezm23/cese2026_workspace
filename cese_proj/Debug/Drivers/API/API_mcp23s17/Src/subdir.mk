################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/API/API_mcp23s17/Src/API_mcp23s17.c \
../Drivers/API/API_mcp23s17/Src/port_mcp23s17.c 

OBJS += \
./Drivers/API/API_mcp23s17/Src/API_mcp23s17.o \
./Drivers/API/API_mcp23s17/Src/port_mcp23s17.o 

C_DEPS += \
./Drivers/API/API_mcp23s17/Src/API_mcp23s17.d \
./Drivers/API/API_mcp23s17/Src/port_mcp23s17.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/API/API_mcp23s17/Src/%.o Drivers/API/API_mcp23s17/Src/%.su Drivers/API/API_mcp23s17/Src/%.cyclo: ../Drivers/API/API_mcp23s17/Src/%.c Drivers/API/API_mcp23s17/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/APP" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/APP/Inc" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/APP/Src" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/API_delay" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/API_delay/Inc" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/API_delay/Src" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/API_lcd" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/API_lcd/Inc" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/API_lcd/Src" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/API_mcp23s17" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/API_mcp23s17/Inc" -I"C:/Users/mnnun/OneDrive/Desktop/cese2026_workspace/cese_proj/Drivers/API/API_mcp23s17/Src" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-API-2f-API_mcp23s17-2f-Src

clean-Drivers-2f-API-2f-API_mcp23s17-2f-Src:
	-$(RM) ./Drivers/API/API_mcp23s17/Src/API_mcp23s17.cyclo ./Drivers/API/API_mcp23s17/Src/API_mcp23s17.d ./Drivers/API/API_mcp23s17/Src/API_mcp23s17.o ./Drivers/API/API_mcp23s17/Src/API_mcp23s17.su ./Drivers/API/API_mcp23s17/Src/port_mcp23s17.cyclo ./Drivers/API/API_mcp23s17/Src/port_mcp23s17.d ./Drivers/API/API_mcp23s17/Src/port_mcp23s17.o ./Drivers/API/API_mcp23s17/Src/port_mcp23s17.su

.PHONY: clean-Drivers-2f-API-2f-API_mcp23s17-2f-Src

