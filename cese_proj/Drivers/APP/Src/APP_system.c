/**
  ******************************************************************************
  * @file           : APP_system.c
  * @brief          : APP functions implementations that manages all security system with the peripherals included.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  * @date           : 2024-04-18
  ******************************************************************************
*/

#include "APP_system.h"
#include "API_mcp23s17.h"
#include "API_lcd.h"
#include "API_delay.h"

#include <string.h>

#define MAX_SIZE_PASSWORD		4

/**
 * @brief Debouncing time to wait until to verify the if the action was carried out correctly.
 * @warning The time is in milliseconds
 * */
#define NO_ACTION_INIT_TICK		5000


/**
 * @brief Secure access system states for the FSM.
 * */
typedef enum
{
	SYSTEM_IDLE = APP_RESET_VALUE,	  /**< Initial state for FSM when any interaction is not happening. */
	SYSTEM_VALID_CONFIG,					    /**< When a key is pressed, the system valid a existing password. */
	SYSTEM_SET_PASSWORD,					    /**< Setting a new password for the system. */
	SYSTEM_ENTER_PASSWORD,					  /**< Entering the password for validation. */
	SYSTEM_SAVE_PASSWORD,             /**< Saving the new password for the system. */
	SYSTEM_ACCESS,                    /**< Granting access to the secured area. */
}system_state_t;

static system_state_t system_state = SYSTEM_IDLE;

static char system_key_pressed;

static uint8_t offset_cfg_password = APP_RESET_VALUE;

/**
 *@brief Instance of delay_t struct for the toggle pin.
 * */
static delay_t delay_debounce_bt = {APP_RESET_VALUE};

static char curr_password[MAX_SIZE_PASSWORD] = {0xFF, 0xFF, 0xFF, 0xFF};

static uint8_t fail_pwd_count = APP_RESET_VALUE;

static uint8_t idle_msg = true;

static char login_user_password[MAX_SIZE_PASSWORD] = {0XFF};
static uint8_t login_pwd_offset = APP_RESET_VALUE;

static SYSTEM_RET system_set_password(char key_pressed)
{
	if(MAX_SIZE_PASSWORD == offset_cfg_password)
	{
		offset_cfg_password = APP_RESET_VALUE;
		system_state = SYSTEM_SAVE_PASSWORD;
		return SYS_OK;
	}
	lcd_put_cur(3, offset_cfg_password);
	lcd_send_data(key_pressed);
	curr_password[offset_cfg_password] = key_pressed;
	offset_cfg_password++;
	return SYS_OK;
}

static SYSTEM_RET system_login_pwd(void)
{
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("Dev configured");
	lcd_put_cur(1, 0);
	lcd_send_string("Enter the pwd:");
	HAL_Delay(1500);
	return SYS_OK;
}

static SYSTEM_RET system_valid_cfg_msg(void)
{
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("Validating config");
	lcd_put_cur(1, 0);
	lcd_send_string("PWD not detected");
	lcd_put_cur(2, 0);
	lcd_send_string("Enter a password:");
	return SYS_OK;
}

static SYSTEM_RET system_idle_msg(void)
{
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("Security System");
	lcd_put_cur(1, 0);
	lcd_send_string("Designed by:");
	lcd_put_cur(2, 0);
	lcd_send_string("Marcos Nunez:");
	lcd_put_cur(3, 0);
	lcd_send_string("CESE 2026 Co26");
	return SYS_OK;
}

static SYSTEM_RET system_reset_resources(void)
{
	memset(login_user_password, 0xFF, MAX_SIZE_PASSWORD);
	login_pwd_offset = APP_RESET_VALUE;
	return SYS_OK;
}

static SYSTEM_RET system_save_password(void)
{
	lcd_clear();
	lcd_put_cur(0, 0);
	lcd_send_string("Saving password:");
	for(size_t i = APP_RESET_VALUE; i < MAX_SIZE_PASSWORD; i++)
	{
		lcd_put_cur(1, i);
		lcd_send_data(curr_password[i]);
	}
	lcd_put_cur(2, 0);
	lcd_send_string("Password saved.");
	system_state = SYSTEM_IDLE;
	HAL_Delay(1500);
	return SYS_OK;
}

static bool_t is_array_zero(void) {
    for (int i = APP_RESET_VALUE; i < MAX_SIZE_PASSWORD; i++) {
        if (curr_password[i] != 0xFF) {
            return false; // Si encuentra un número distinto de 0, se detiene y retorna falso
        }
    }
    return true; // Si termina el bucle sin encontrar otros números, todo es cero
}

static bool_t system_verify_password(char key_pressed)
{
	bool_t pwd_ret = false;
	if(login_pwd_offset < MAX_SIZE_PASSWORD)
	{
		login_user_password[login_pwd_offset] = key_pressed;
		lcd_put_cur(2, login_pwd_offset);
		lcd_send_data(key_pressed);
		login_pwd_offset++;
	}else if(login_pwd_offset == MAX_SIZE_PASSWORD)
	{
		if(SYS_OK == strncmp(login_user_password, curr_password, MAX_SIZE_PASSWORD))
		{
			system_state = SYSTEM_ACCESS;
		}else
		{
			if(2 == fail_pwd_count)
			{
				system_state = SYSTEM_IDLE;
				idle_msg = true;
				return pwd_ret;
			}
			fail_pwd_count++;
			lcd_put_cur(3, 0);
			uint8_t attempt_left = 3 - fail_pwd_count;
			char caracter_lcd = attempt_left + '0';
			lcd_send_data(caracter_lcd);
			lcd_put_cur(3, 2);
			lcd_send_string("Attempt left");
			lcd_put_cur(2, APP_RESET_VALUE);
			lcd_send_string("               ");
			system_state = SYSTEM_ENTER_PASSWORD;
			system_reset_resources();
		}
	}
	return pwd_ret;
}

SYSTEM_RET system_fsm_state_update(void)
{
	system_key_pressed = mcp_scan_keypad();
	switch (system_state)
	{
	case SYSTEM_IDLE:
		if(idle_msg)
		{
			system_reset_resources();
			system_idle_msg();
			idle_msg = false;
		}
		if(APP_RESET_VALUE != system_key_pressed)
		{
			system_state = SYSTEM_VALID_CONFIG;
		}
		break;
	case SYSTEM_VALID_CONFIG:
		if(true == is_array_zero())
		{
			system_valid_cfg_msg();
			system_state = SYSTEM_SET_PASSWORD;
		}else
		{
			system_login_pwd();
			system_state = SYSTEM_ENTER_PASSWORD;
		}
		break;
	case SYSTEM_SET_PASSWORD:
		if(APP_RESET_VALUE != system_key_pressed)
		{
			system_set_password(system_key_pressed);
		}
		break;
	case SYSTEM_ENTER_PASSWORD:
		if(APP_RESET_VALUE != system_key_pressed)
		{
			system_verify_password(system_key_pressed);
		}
		break;
	case SYSTEM_SAVE_PASSWORD:
		system_save_password();
		idle_msg = true;
		break;
	case SYSTEM_ACCESS:
		system_state = SYSTEM_IDLE;
		idle_msg = true;
		break;
	default:
		break;
	}
	HAL_Delay(250);
	return SYS_OK;
}

static void system_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_TypeDef *mcp_spi_port = NULL;
  uint16_t spi_cs_pin = 0;
  mcp_get_spi_port((void **)&mcp_spi_port);
  mcp_get_spi_cs_pin(&spi_cs_pin);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(mcp_spi_port, spi_cs_pin, GPIO_PIN_SET);
  /*Configure GPIO pin : SPI_CS_Pin */
  GPIO_InitStruct.Pin = spi_cs_pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(mcp_spi_port, &GPIO_InitStruct);
}

static SYSTEM_RET system_delay_init(void)
{
	delayInit(&delay_debounce_bt, NO_ACTION_INIT_TICK);
	return SYS_OK;
}

SYSTEM_RET system_init(void)
{
	system_gpio_init();
	lcd_init();
	mcp_init();
	system_delay_init();
	return SYS_OK;
}
