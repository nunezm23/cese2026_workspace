/**
  ******************************************************************************
  * @file           : APP_system.h
  * @brief          : APP to make definitions for manages all security system with the peripherals included.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  * @date           : 2024-04-18
  ******************************************************************************
*/

#ifndef __APP_system_H /**< To prevent recursive inclusion*/
#define __APP_system_H

#include <stdint.h>

#define SYSTEM_RET		int8_t

/**
 * @brief Reset value for the application.
 */
#define APP_RESET_VALUE		0x00U

/**
 * @brief Error types for system functions.
 */
typedef enum
{
	SYS_OK = 0,               /**< System operation successful */
	SYS_ERR_INIT,             /**< Initialization error */
	SYS_ERR_NULL_POINTER,     /**< Null pointer error */
	SYS_ERR_INVALID_PARAMS,   /**< Invalid parameters error */
	SYS_ERR_NO_KEYS_PRESSED,  /**< No keys pressed error */
	SYS_ERR_UNKNOWN,          /**< Unknown error */
}sys_err_t;

/**
 * @brief Initializes the system peripherals and resources for the security system application.
 * @note Initializes GPIO ports, SPI interface, LCD display, MCP23S17 keypad controller, and system delay timer.
 * 
 * @return SYSTEM_RET Status code indicating success or failure of initialization.
 * @retval SYS_OK on successful initialization, SYS_ERROR on failure.
 */
SYSTEM_RET system_init(void);

/**
 * @brief Updates the state of the system finite state machine.
 * 
 * @return SYSTEM_RET Status code.
 * @retval SYS_OK on success.
 */
SYSTEM_RET system_fsm_state_update(void);

#endif /* __APP_system_H */
