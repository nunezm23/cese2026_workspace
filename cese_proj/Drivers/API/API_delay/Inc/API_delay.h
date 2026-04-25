/**
  ******************************************************************************
  * @file           : API_delay.h
  * @brief          : API delay definitions
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  ******************************************************************************
*/

/** Section for defining libraries, variables */

#ifndef __API_delay_H /**< To prevent recursive inclusion*/
#define __API_delay_H

/** Standard C definitions */
#include "API_common.h"

/**
 * @brief Delay struct that contains relevants parameters for implementation.
 * */
typedef struct{
   tick_t startTime;	/**< Start time to get the tick before comparison.*/
   tick_t duration;		/**< Duration of delay*/
   bool_t running;		/**< Flag to know if the time was reached. */
}delay_t;

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

/**
 * @brief Get the running flag parameter to know if the delay is currently running.
 *
 * @param[in] delay		Pointer to delay_t struct.
 *
 * @return bool_t.
 * @retval true if the delay is running or null pointer is received, false if the delay is not running and the pointer is valid.
 */
bool_t delayIsRunning(delay_t * delay);

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

#endif /* __API_delay_H */
