


#include "stm32f4xx_hal.h"


/**
 * @brief SPI peripheral handler instance.
 * @note Configured for communication with MCP23S17 I/O expander.
 * */
static SPI_HandleTypeDef hspi1;

/**
 * @brief SPI Chip Select (CS) pin.
 * 
 */
#define SPI_CS_Pin 			GPIO_PIN_6

/**
 * @brief GPIO port for SPI Chip Select (CS) pin.
 */
#define SPI_CS_GPIO_Port 	GPIOB



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
    
    hal_ret = HAL_SPI_Init(&hspi1);
    if (HAL_OK != hal_ret)
    {
        return PORT_ERR_INIT;
    }
    
    return PORT_OK;
}

uint8_t port_gpio_set_cs_pin(void)
{
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    return PORT_OK;
}

uint8_t port_gpio_reset_cs_pin(void)
{
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    return PORT_OK;
}

uint8_t port_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {PORT_RESET_VALUE};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    
    GPIO_InitStruct.Pin = SPI_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SPI_CS_GPIO_Port, &GPIO_InitStruct);
    
    return PORT_OK;
}

uint8_t port_spi_master_transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if(NULL == pData || PORT_RESET_VALUE == Size)
    {
        return PORT_ERR_INVALID_PARAMS;
    }

    HAL_StatusTypeDef hal_ret;

    hal_ret = HAL_SPI_Transmit(&hspi1, pData, Size, Timeout);
    if (HAL_OK != hal_ret)
    {
        return PORT_ERR_UNKNOWN;
    }
    
    return PORT_OK;
}

uint8_t port_spi_master_receive(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if(NULL == pData || PORT_RESET_VALUE == Size)
    {
        return PORT_ERR_NULL_POINTER;
    }

    HAL_StatusTypeDef hal_ret;

    hal_ret = HAL_SPI_Receive(&hspi1, pData, Size, Timeout);
    if (HAL_OK != hal_ret)
    {
        return PORT_ERR_UNKNOWN;
    }
    
    return PORT_OK;
}