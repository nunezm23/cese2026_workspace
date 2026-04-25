#include "stm32f4xx_hal.h"

uint8_t port_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
    return PORT_OK;
}

uint32_t port_get_tick(void)
{
    return HAL_GetTick();
}