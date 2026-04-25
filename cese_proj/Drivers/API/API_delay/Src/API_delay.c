/**
  ******************************************************************************
  * @file           : API_delay.c
  * @brief          : Delay functions implementations for API.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  ******************************************************************************
*/

/** Section for defining libraries, variables */

/**
 * @brief Including header file for API implementation
 * */
#include "API_delay.h"
#include <string.h>

/**
 * @brief Invalid tick parameter to set in delayWrite function when the duration is 0.
 * */
#define INVALID_TICK_PARAMETER 0

/**
  * @brief Number of half-cycles to change state in LED for 50% duty cycle.
  */
#define NBR_HALF_CYCLES		10

/** Public functions implementations */

void delayInit( delay_t * delay, tick_t duration )
{
	if(NULL == delay || INVALID_TICK_PARAMETER == duration)
	{
		return;
	}
	delay->duration = duration;
	delay->running = false;
	return;
}

bool_t delayRead( delay_t * delay )
{
	bool_t bool_ret = false;
	if(NULL == delay)
	{
		return bool_ret;
	}
	if(false == delay->running)
	{
		delay->startTime = port_get_tick();
		delay->running = true;
		return bool_ret;
	}else if(true == delay->running)
	{
		uint32_t current_time = port_get_tick();;
		uint32_t start_time = delay->startTime;
		if( (current_time - start_time) >= delay->duration)
		{
			delay->running = false;
			bool_ret = true;
			return bool_ret;
		}
	}
	return bool_ret;
}

void delayWrite( delay_t * delay, tick_t duration )
{
	if(NULL == delay || INVALID_TICK_PARAMETER == duration)
	{
		return;
	}
	delay->duration = duration;
	return;
}

bool_t delayIsRunning(delay_t * delay)
{
	bool_t b_ret = true;
	if(NULL == delay)
	{
		return b_ret;
	}
	b_ret = delay->running;
	return b_ret;
}
