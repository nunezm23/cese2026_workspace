/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/**
 * @brief Tick type of uint32_t
 * */
typedef uint32_t tick_t;

/**
 * @brief Bool type redefinition
 * */
typedef bool bool_t;

/**
 * @brief Delay struct that contains relevants parameters for implementation.
 * */
typedef struct{
   tick_t startTime;	/**< Start time to get the tick before comparison.*/
   tick_t duration;		/**< Duration of delay*/
   bool_t running;		/**< Flag to know if the time was reached. */
} delay_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/**
 * @brief Init parameters of delay_t struct and set the initial duration of blink.
 *
 * @param[out] delay		Pointer to delay_t struct.
 * @param[in]  duration		Duration to set in delay_t struct.
 *
 * @return void.
 */
void delayInit( delay_t * delay, tick_t duration );

/**
 * @brief Compare between current time and start time to know it the duration delay was reached.
 *
 * @param[in] delay		Pointer to delay_t struct.
 *
 * @return bool_t.
 * @retval true when delay time was reached.
 */
bool_t delayRead( delay_t * delay );

/**
 * @brief Set the new duration of delay for blink LED.
 *
 * @param[out] delay		Pointer to delay_t struct.
 * @param[in]  duration		Duration to set in delay_t struct.
 *
 * @return void.
 */
void delayWrite( delay_t * delay, tick_t duration );

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
