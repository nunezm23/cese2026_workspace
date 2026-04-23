/**
  ******************************************************************************
  * @file           : port.h
  * @brief          : Porting layer for the project, to adapt the code to the hardware and software environment.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  * @date           : 2024-04-18
  ******************************************************************************
*/

#ifndef __PORT_H /**< To prevent recursive inclusion */
#define __PORT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


/**
 * @brief Reset zero value for validations or init parameters.
 * 
 */
#define PORT_RESET_VALUE		0U

/**
 * @brief Transmits data over I2C as a master.
 * @note This function abstracts the HAL_I2C_Master_Transmit for portability.
 * 
 * @param[in] hi2c          I2C handle pointer.
 * @param[in] DevAddress    Device address.
 * @param[in] pData         Data pointer.
 * @param[in] Size          Data size.
 * @param[in] Timeout       Timeout value.
 *  
 * @return uint8_t
 * @retval 0U on success, 1U on error.
 */
uint8_t port_i2c_master_transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief   Initializes the I2C peripheral.
 * @note    It configures I2C peripheral with standard settings for LCD communication.
 * 
 * @return uint8_t 
 * @retval 0U on success, 1U on initialization failure.
 */
uint8_t port_i2c_init(void);

/**
 * @brief   Delays execution for a specified number of milliseconds.
 * 
 * @param[in] ms    Number of milliseconds to delay.
 * 
 * @return uint8_t 
 * @retval 0U on success, 1U on error.
 */
uint8_t port_delay_ms(uint32_t ms);

/**
 * @brief   Retrieves the current tick value in milliseconds.
 * 
 * @return uint32_t Current tick value in milliseconds.
 */
uint32_t port_get_tick(void);

/**
 * @brief   Transmits data over SPI as a master.
 * 
 * @param[in] pData         Data pointer.
 * @param[in] Size          Data size.
 * @param[in] Timeout       Timeout value.
 *  
 * @return uint8_t
 * @retval 0U on success, 1U on error.
 */
uint8_t port_spi_master_transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief   Receives data over SPI as a master.
 * 
 * @param[in] pData         Data pointer to store received data.
 * @param[in] Size          Data size.
 * @param[in] Timeout       Timeout value.
 *  
 * @return uint8_t
 * @retval 0U on success, 1U on error.
 */
uint8_t port_spi_master_receive(uint8_t *pData, uint16_t Size, uint32_t Timeout);

/**
 * @brief   Initializes the SPI peripheral for master mode communication.
 * 
 * @return uint8_t 
 * @retval 0U on success, 1U on initialization failure.
 */
uint8_t port_spi_init(void);

/**
 * @brief   Sets the SPI chip select pin high.
 * 
 * @return uint8_t 
 */
uint8_t port_gpio_set_cs_pin(void);

/**
 * @brief   Resets the SPI chip select pin.
 * 
 * @return uint8_t 
 */
uint8_t port_gpio_reset_cs_pin(void);

/**
 * @brief  Initializes the GPIO pins for SPI communication.
 * 
 * @return uint8_t 
 */
uint8_t port_gpio_init(void);

#endif /* __PORT_H */
