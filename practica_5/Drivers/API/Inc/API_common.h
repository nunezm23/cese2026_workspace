/**
  ******************************************************************************
  * @file           : API_common.h
  * @brief          : API common to avoid repeate definitions and another importants parameters.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  ******************************************************************************
*/

#ifndef __API_common_H /**< To prevent recursive inclusion*/
#define __API_common_H

/** Standard C definitions */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"

/**
 * @brief Reset zero value for validations or init parameters.
 * */
#define API_RESET_VALUE		0

/**
 * @brief Bool type redefinition
 * */
typedef bool bool_t;

/**
 * @brief Tick type of uint32_t
 * */
typedef uint32_t tick_t;

#endif /* __API_common_H */
