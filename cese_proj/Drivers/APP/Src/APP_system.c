/**
  ******************************************************************************
  * @file           : APP_system.c
  * @brief          : APP functions implementations that manages all security system with the peripherals included.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  * @date           : 2024-04-18
  ******************************************************************************
*/

#include "APP_system.h"
#include "API_mcp23s17.h"
#include "API_lcd.h"


static void system_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_TypeDef *mcp_spi_port = NULL;
  uint16_t spi_cs_pin = 0;
  mcp_get_spi_port((void **)&mcp_spi_port);
  mcp_get_spi_cs_pin(&spi_cs_pin);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(mcp_spi_port, spi_cs_pin, GPIO_PIN_SET);
  /*Configure GPIO pin : SPI_CS_Pin */
  GPIO_InitStruct.Pin = spi_cs_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(mcp_spi_port, &GPIO_InitStruct);
}

SYSTEM_RET system_init(void)
{
	system_gpio_init();
	lcd_init();
	mcp_init();
	return SYS_OK;
}
