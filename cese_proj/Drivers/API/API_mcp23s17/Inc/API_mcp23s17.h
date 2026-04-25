/**
  ******************************************************************************
  * @file           : API_mcp23s17.h
  * @brief          : API mcp23s17 to manage this IOs expansion that establish communication by SPI bus..
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  * @date           : 2024-04-18
  ******************************************************************************
*/

#ifndef __API_mcp23s17_H /**< To prevent recursive inclusion*/
#define __API_mcp23s17_H

/**
 * @brief Ret type for MCP23S17 API functions.
 */
#define MCP_RET		int8_t

/**
 * @brief Error types for MCP23S17 API functions.
 * @note Used for consistent error handling across MCP23S17 operations.
 */
typedef enum
{
	MCP_OK = 0,               /**< Successful operation */
	MCP_ERR_INIT,           /**< Initialization error */
	MCP_ERR_NULL_POINTER,   /**< Null pointer error */
	MCP_ERR_INVALID_PARAMS, /**< Invalid parameters error */
	MCP_ERR_UNKNOWN,        /**< Unknown error */
	MCP_ERR_SPI_COMM,		/**< SPI error communication.*/
}mcp_err_t;

/**
 * @brief Scans the keypad matrix and returns the pressed key.
 * @note Performs row-by-row scanning with debounce delay.
 * 
 * @return char Character representing the pressed key, or 0 if no key is pressed.
 */
char mcp_scan_keypad(void);

/**
 * @brief Initializes the MCP23S17 I/O expander.
 * 
 * @return MCP_RET Status code.
 * @retval MCP_OK on successful initialization.
 */
MCP_RET mcp_init(void);

/**
 * @brief  Initializes the GPIO pins for SPI communication.
 * 
 * @return uint8_t 
 */
uint8_t port_gpio_init(void);

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

#endif /* __API_mcp23s17_H */
