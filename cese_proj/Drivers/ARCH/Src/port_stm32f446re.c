/**
  ******************************************************************************
  * @file           : port.c
  * @brief          : Porting layer for the project, to adapt the code to the hardware and software environment.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  * @date           : 2024-04-22
  ******************************************************************************
*/

#include "port.h"
#include "stm32f4xx_hal.h"


#define SPI_CS_Pin 			GPIO_PIN_6
#define SPI_CS_GPIO_Port 	GPIOB

/**
 * @brief I2C peripheral handler instance.
 * @note Configured for communication with LCD display via PCF8574 I2C expander.
 * */
static I2C_HandleTypeDef hi2c1;

/**
 * @brief SPI peripheral handler instance.
 * @note Configured for communication with MCP23S17 I/O expander.
 * */
static SPI_HandleTypeDef hspi1;

uint8_t port_i2c_init(void)
{
    HAL_StatusTypeDef hal_ret;
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  
  /* Initialize I2C peripheral */
  hal_ret = HAL_I2C_Init(&hi2c1);
  if (HAL_OK != hal_ret)
  {
    return 0U; /* I2C initialization failed */
  }

    return 0U; /* Success */
}


uint8_t port_i2c_master_transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if(NULL == pData || PORT_RESET_VALUE == Size)
    {
        return 1U; /* Invalid parameters */
    }

    HAL_StatusTypeDef hal_ret;

    hal_ret = HAL_I2C_Master_Transmit(&hi2c1, DevAddress, pData, Size, Timeout);
    if (HAL_OK != hal_ret)
    {
        return 1U; /* Transmission error */
    }
    
    return hal_ret; /* Success */
}

uint8_t port_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
    return 0U; /* Success */
}

uint32_t port_get_tick(void)
{
    return HAL_GetTick(); /* Return current tick value */
}

uint8_t port_spi_master_transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if(NULL == pData || PORT_RESET_VALUE == Size)
    {
        return 1U; /* Invalid parameters */
    }

    HAL_StatusTypeDef hal_ret;

    hal_ret = HAL_SPI_Transmit(&hspi1, pData, Size, Timeout);
    if (HAL_OK != hal_ret)
    {
        return 1U; /* Transmission error */
    }
    
    return hal_ret; /* Success */
}

uint8_t port_spi_master_receive(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if(NULL == pData || PORT_RESET_VALUE == Size)
    {
        return 1U; /* Invalid parameters */
    }

    HAL_StatusTypeDef hal_ret;

    hal_ret = HAL_SPI_Receive(&hspi1, pData, Size, Timeout);
    if (HAL_OK != hal_ret)
    {
        return 1U; /* Reception error */
    }
    
    return hal_ret; /* Success */
}

uint8_t port_spi_init(void)
{
    HAL_StatusTypeDef hal_ret;
    
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    
    /* Initialize SPI peripheral */
    hal_ret = HAL_SPI_Init(&hspi1);
    if (HAL_OK != hal_ret)
    {
        return 1U; /* SPI initialization failed */
    }
    
    return 0U; /* Success */
}

uint8_t port_gpio_set_cs_pin(void)
{
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    return 0U; /* Success */
}

uint8_t port_gpio_reset_cs_pin(void)
{
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    return 0U;
}

uint8_t port_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    
    /* Configure GPIO pin : SPI_CS_Pin */
    GPIO_InitStruct.Pin = SPI_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SPI_CS_GPIO_Port, &GPIO_InitStruct);
    
    return 0U; /* Success */
}
