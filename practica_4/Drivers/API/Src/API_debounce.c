/**
  ******************************************************************************
  * @file           : API_debounce.c
  * @brief          : Debounce functions implementations for API.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  ******************************************************************************
*/

/** Section for defining libraries, variables */
#include "API_common.h"

/**
 * @brief GPIO port for button in NUCLEO F446RE.
 * */
#define B1_GPIO_Port_Debounce 		GPIOC

/**
 * @brief GPIO pin for button in NUCLEO F446RE.
 * */
#define B1_Pin_Debounce				GPIO_PIN_13

/**
 * @brief Debounce states for FSM implementation.
 * */
typedef enum
{
	BUTTON_UP = API_RESET_VALUE,	/**< Initial state for FSM when the button is up and its not pressed.. */
	BUTTON_FALLING,					/**< Falling state when the button is being pressed. */
	BUTTON_DOWN,					/**< Down state when the button is totally pressed. */
	BUTTON_RAISING,					/**< Raising state when the button is being released. */
}debounceState_t;

/**
 * @brief Local variable to know if the button was pressed.
 * */
static bool_t buttonIsPressed = API_RESET_VALUE;

/**
 * @brief Debounce state local variable to update the state in the FSM.
 * */
static debounceState_t debounceFSM = API_RESET_VALUE;

/**
 * @brief Indicates if the debounce module was initialized.
 * */
static bool_t debounceIsInitialized = API_RESET_VALUE;

void debounceFSM_init(void)
{
	debounceFSM = BUTTON_UP;
	debounceIsInitialized = true;
}

void debounceFSM_update(bool_t debounce_time)
{
	if(!debounceIsInitialized)
	{
		return;
	}

	switch(debounceFSM)
	{
	case BUTTON_UP:
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(B1_GPIO_Port_Debounce, B1_Pin_Debounce))
		{
			debounceFSM = BUTTON_FALLING;
		}
		break;
	case BUTTON_FALLING:
		if(debounce_time && (GPIO_PIN_RESET == HAL_GPIO_ReadPin(B1_GPIO_Port_Debounce, B1_Pin_Debounce)))
		{
			debounceFSM = BUTTON_DOWN;
			buttonIsPressed = true;
		}
		break;
	case BUTTON_DOWN:
		if(GPIO_PIN_SET == HAL_GPIO_ReadPin(B1_GPIO_Port_Debounce, B1_Pin_Debounce))
		{
			debounceFSM = BUTTON_RAISING;
		}
		break;
	case BUTTON_RAISING:
		if(debounce_time && (GPIO_PIN_SET == HAL_GPIO_ReadPin(B1_GPIO_Port_Debounce, B1_Pin_Debounce)))
		{
			debounceFSM = BUTTON_UP;
		}
		break;
	default:
		debounceIsInitialized = false;
		break;
	}
}

bool_t readKey(void)
{
	if(!debounceIsInitialized)
	{
		return false;
	}
	bool_t temp_button_val = buttonIsPressed;
	buttonIsPressed = false;
	return temp_button_val;
}
