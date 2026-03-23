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
#include <stdbool.h>
#include <stdint.h>

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

#endif /* __API_delay_H */
