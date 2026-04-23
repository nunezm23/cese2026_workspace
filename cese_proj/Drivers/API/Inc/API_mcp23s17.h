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

#include "API_common.h"

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

#endif /* __API_mcp23s17_H */
