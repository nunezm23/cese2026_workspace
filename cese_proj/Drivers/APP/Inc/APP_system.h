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

#include "API_common.h"

#define SYSTEM_RET		int8_t

#define APP_RESET_VALUE		API_RESET_VALUE

typedef enum
{
	SYS_OK = 0,
	SYS_ERR_INIT,
	SYS_ERR_NULL_POINTER,
	SYS_ERR_INVALID_PARAMS,
	SYS_ERR_NO_KEYS_PRESSED,
	SYS_ERR_UNKNOWN,
}sys_err_t;

SYSTEM_RET system_init(void);

SYSTEM_RET system_fsm_state_update(void);


#endif /* __APP_system_H */
