/**
  ******************************************************************************
  * @file           : port_delay.c
  * @brief          : Porting layer for delay functions using STM32 HAL library.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  ******************************************************************************
*/

#include "stm32f4xx_hal.h"
#include "API_delay.h"

uint8_t port_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
    return DELAY_OK;
}

uint32_t port_get_tick(void)
{
    return HAL_GetTick();
}
