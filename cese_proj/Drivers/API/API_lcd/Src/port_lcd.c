
#include "stm32f4xx_hal.h"
#include "API_lcd.h"

/**
 * @brief I2C peripheral handler instance.
 * @note Configured for communication with LCD display via PCF8574 I2C expander.
 * */
static I2C_HandleTypeDef hi2c1;

/**
 * @brief Clock speed for I2C peripheral.
 */
#define CLOCK_SPEED_100KHZ 	100000U

uint8_t port_i2c_init(void)
{
    HAL_StatusTypeDef hal_ret;
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = CLOCK_SPEED_100KHZ;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = LCD_RESET_VALUE;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = LCD_RESET_VALUE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  
    hal_ret = HAL_I2C_Init(&hi2c1);
    if (HAL_OK != hal_ret)
    {
        return LCD_ERR_INIT;
    }

    return LCD_OK;
}

uint8_t port_i2c_master_transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if(NULL == pData || LCD_RESET_VALUE == Size)
    {
        return LCD_ERR_INVALID_PARAMS;
    }

    HAL_StatusTypeDef hal_ret;

    hal_ret = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, pData, Size, Timeout);
    if (HAL_OK != hal_ret)
    {
        return LCD_ERR_UNKNOWN;
    }
    
    return LCD_OK;
}
