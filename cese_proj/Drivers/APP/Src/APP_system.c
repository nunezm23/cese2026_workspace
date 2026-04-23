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

#include "port.h"

/**
 * @brief Maximum size of password.
 * */
#define MAX_SIZE_PASSWORD		4

/**
 * @brief Debouncing time to wait until to verify the if the action was carried out correctly.
 * @warning The time is in milliseconds
 * */
#define NO_ACTION_INIT_TICK		10000

/**
 * @brief Index for the first row on the LCD display.
 * */
#define LCD_FIRST_ROW_IDX	0

/**
 * @brief Index for the second row on the LCD display.
 * */
#define LCD_SECOND_ROW_IDX	1

/**
 * @brief Index for the third row on the LCD display.
 * */
#define LCD_THIRD_ROW_IDX	2

/**
 * @brief Index for the fourth row on the LCD display.
 * */
#define LCD_FOURTH_ROW_IDX	3

/**
 * @brief Index for the first column on the LCD display.
 * */
#define LCD_COL_IDX_0		0

/**
 * @brief Index for the second column on the LCD display.
 * */
#define LCD_COL_IDX_1		2

/**
 * @brief Delay for 1500 milliseconds.
 */
#define DELAY_1500_MS			1500

/**
 * @brief Delay for 3000 milliseconds.
 */
#define DELAY_3000_MS			3000

/**
 * @brief Delay for 250 milliseconds.
 */
#define DELAY_250_MS			250

/**
 * @brief Secure access system states for the FSM.
 * */
typedef enum
{
	SYSTEM_IDLE = APP_RESET_VALUE,	  	/**< Initial state for FSM when any interaction is not happening. */
	SYSTEM_VALID_CONFIG,				/**< When a key is pressed, the system valid a existing password. */
	SYSTEM_SET_PASSWORD,				/**< Setting a new password for the system. */
	SYSTEM_ENTER_PASSWORD,				/**< Entering the password for validation. */
	SYSTEM_SAVE_PASSWORD,             	/**< Saving the new password for the system. */
	SYSTEM_ACCESS,                    	/**< Granting access to the secured area. */
}system_state_t;

/**
 * @brief System state related to Secure System FSM.
 * */
static system_state_t system_state = SYSTEM_IDLE;

/**
 * @brief Variable to save the key pressed on keypad.
 * */
static char system_key_pressed;

/**
 * @brief Offset to identify the keys position in the array.
 * */
static uint8_t offset_cfg_password = APP_RESET_VALUE;

/**
 *@brief Instance of delay_t struct for the timer during FSM states.
 * */
static delay_t delay_fsm_stage = {APP_RESET_VALUE};

/**
 * @brief Array to save the password entered by the user.
 * */
static char curr_password[MAX_SIZE_PASSWORD] = {0xFF, 0xFF, 0xFF, 0xFF};

/**
 * @brief Attempts to enter a valid password in the system.
 * */
static uint8_t fail_pwd_count = APP_RESET_VALUE;

/**
 * @brief Flag to show the idle message just a once time.
 * */
static uint8_t idle_msg = true;

/**
 * @brief Array to save the password entered by the user when trying to login in a system session.
 * */
static char login_user_password[MAX_SIZE_PASSWORD] = {0XFF};

/**
 * @brief Offset variable to save the position of password entered.
 * */
static uint8_t login_pwd_offset = APP_RESET_VALUE;

/**
 * @brief Sets a character of the password during configuration.
 * @note When password reaches MAX_SIZE_PASSWORD, transitions to SYSTEM_SAVE_PASSWORD state.
 * 
 * @param[in] key_pressed The character key pressed by the user.
 * 
 * @return SYSTEM_RET Status code.
 * @retval SYS_OK on success.
 * */
static SYSTEM_RET system_set_password(char key_pressed);

/**
 * @brief Displays login prompt message on LCD display.
 * @note Displays message indicating device is configured and prompts user to enter password.
 * 
 * @return SYSTEM_RET Status code (SYS_OK on success).
 * */
static SYSTEM_RET system_login_pwd(void);

/**
 * @brief Displays configuration validation message when no password is detected.
 * 
 * @note Used during first-time system setup to indicate password configuration is required.
 * 
 * @return SYSTEM_RET Status code.
 * @retval SYS_OK on success.
 * */
static SYSTEM_RET system_valid_cfg_msg(void);

/**
 * @brief Displays idle state message on LCD display.
 * 
 * @note Shows system welcome message with designer information.
 * 
 * @return SYSTEM_RET Status code.
 * @retval SYS_OK on success.
 * */
static SYSTEM_RET system_idle_msg(void);

/**
 * @brief Displays access granted message on LCD display.
 * 
 * @note Displayed upon successful password authentication.
 * 
 * @return SYSTEM_RET Status code.
 * @retval SYS_OK on success.
 * */
static SYSTEM_RET system_access_msg(void);

/**
 * @brief Resets login-related resources and counters.
 * 
 * @note Clears login password buffer and resets login offset counter.
 * 
 * @return SYSTEM_RET Status code (SYS_OK on success).
 * @retval SYS_OK on success.
 * */
static SYSTEM_RET system_reset_resources(void);

/**
 * @brief Saves the newly configured password to system state.
 * 
 * @note Displays confirmation message and transitions back to SYSTEM_IDLE state.
 * 
 * @return SYSTEM_RET Status code..
 * @retval SYS_OK on success.
 * */
static SYSTEM_RET system_save_password(void);

/**
 * @brief Checks if the current password array is in uninitialized state (all 0xFF).
 * 
 * @return bool_t status of password configuration.
 * @retval true if password is unconfigured, false if password has been set.
 * */
static bool_t is_array_zero(void);

/**
 * @brief Verifies user-entered password against the stored password.
 * @note Implements 3-attempt lockout mechanism for failed authentication.
 * 
 * @param[in] key_pressed The character key pressed by the user.
 * 
 * @return bool_t Result of password verification.
 * @retval true If password verification succeeds, false otherwise.
 * */
static bool_t system_verify_password(char key_pressed);

/**
 * @brief Initializes debounce delay timer for the system.
 * @note Configures delay with NO_ACTION_INIT_TICK milliseconds timeout.
 * 
 * @return SYSTEM_RET Status code.
 * @retval SYS_OK on successful initialization, SYS_ERROR on failure.
 * */
static SYSTEM_RET system_delay_init(void);

/**
 * @brief Displays timeout message on LCD display when user interaction times out.
 * @note Used in SYSTEM_SET_PASSWORD and SYSTEM_ENTER_PASSWORD states to indicate session timeout.
 * 
 * @return SYSTEM_RET Status code.
 * @retval SYS_OK on success.
 */
static SYSTEM_RET system_timeout_msg(void);

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
	lcd_put_cur(LCD_FIRST_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Dev configured");
	lcd_put_cur(LCD_SECOND_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Enter the pwd:");
	port_delay_ms(DELAY_1500_MS);
	return SYS_OK;
}

static SYSTEM_RET system_valid_cfg_msg(void)
{
	lcd_clear();
	lcd_put_cur(LCD_FIRST_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Validating config");
	lcd_put_cur(LCD_SECOND_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("PWD not detected");
	lcd_put_cur(LCD_THIRD_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Enter a password:");
	return SYS_OK;
}

static SYSTEM_RET system_idle_msg(void)
{
	lcd_clear();
	lcd_put_cur(LCD_FIRST_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Security System");
	lcd_put_cur(LCD_SECOND_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Designed by:");
	lcd_put_cur(LCD_THIRD_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Marcos Nunez");
	lcd_put_cur(LCD_FOURTH_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("CESE 2026 Co26");
	return SYS_OK;
}

static SYSTEM_RET system_access_msg(void)
{
	lcd_clear();
	lcd_put_cur(LCD_FIRST_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Security System");
	lcd_put_cur(LCD_SECOND_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Access");
	lcd_put_cur(LCD_THIRD_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Successfull");
	lcd_put_cur(LCD_FOURTH_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("CESE 2026 Co26");
	port_delay_ms(DELAY_3000_MS);
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
	lcd_put_cur(LCD_FIRST_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Saving password:");
	for(size_t i = APP_RESET_VALUE; i < MAX_SIZE_PASSWORD; i++)
	{
		lcd_put_cur(LCD_SECOND_ROW_IDX, i);
		lcd_send_data(curr_password[i]);
	}
	lcd_put_cur(LCD_THIRD_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("Password saved.");
	system_state = SYSTEM_IDLE;
	port_delay_ms(DELAY_1500_MS);
	return SYS_OK;
}

static bool_t is_array_zero(void)
{
    for (uint8_t i = APP_RESET_VALUE; i < MAX_SIZE_PASSWORD; i++)
    {
        if (curr_password[i] != 0xFF)
        {
            return false; /* Password has been configured */
        }
    }
    return true; /* All bytes uninitialized (0xFF) */
}

static bool_t system_verify_password(char key_pressed)
{
	bool_t pwd_ret = false;
	if(login_pwd_offset < MAX_SIZE_PASSWORD)
	{
		login_user_password[login_pwd_offset] = key_pressed;
		lcd_put_cur(LCD_THIRD_ROW_IDX, login_pwd_offset);
		lcd_send_data(key_pressed);
		login_pwd_offset++;
	}
	else if(login_pwd_offset == MAX_SIZE_PASSWORD)
	{
		/* Password entry complete, verify against stored password */
		int cmp_result = strncmp((const char *)login_user_password, (const char *)curr_password, MAX_SIZE_PASSWORD);
		if(APP_RESET_VALUE == cmp_result)
		{
			/* Password matches - grant access */
			system_state = SYSTEM_ACCESS;
			pwd_ret = true;
		}
		else
		{
			/* Password mismatch - check attempt count */
			if(fail_pwd_count >= 2U)
			{
				/* Maximum attempts exceeded - lock system */
				system_state = SYSTEM_IDLE;
				idle_msg = true;
				return pwd_ret;
			}
			fail_pwd_count++;
			lcd_put_cur(LCD_FOURTH_ROW_IDX, LCD_COL_IDX_0);
			uint8_t attempt_left = (3U - fail_pwd_count);
			char caracter_lcd = (char)(attempt_left + '0');
			lcd_send_data(caracter_lcd);
			lcd_put_cur(LCD_FOURTH_ROW_IDX, LCD_COL_IDX_1);
			lcd_send_string("Attempt left");
			lcd_put_cur(LCD_THIRD_ROW_IDX, LCD_COL_IDX_0);
			lcd_send_string("               ");
			system_state = SYSTEM_ENTER_PASSWORD;
			system_reset_resources();
		}
	}
	return pwd_ret;
}

static SYSTEM_RET system_delay_init(void)
{
	SYSTEM_RET ret = SYS_OK;
	
	delayInit(&delay_fsm_stage, NO_ACTION_INIT_TICK);
	return ret;
}

static SYSTEM_RET system_timeout_msg(void)
{
	lcd_clear();
	lcd_put_cur(LCD_SECOND_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("      TIMEOUT   ");
	lcd_put_cur(LCD_THIRD_ROW_IDX, LCD_COL_IDX_0);
	lcd_send_string("      REACHED   ");
	port_delay_ms(DELAY_1500_MS);
	return SYS_OK;
}

SYSTEM_RET system_fsm_state_update(void)
{
	SYSTEM_RET ret = SYS_OK;
	
	system_key_pressed = mcp_scan_keypad();
	switch (system_state)
	{
	case SYSTEM_IDLE:
		if(idle_msg)
		{
			ret = system_reset_resources();
			if (SYS_OK != ret)
			{
				return ret; /* Error resetting resources */
			}
			ret = system_idle_msg();
			if (SYS_OK != ret)
			{
				return ret; /* Error displaying idle message */
			}
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
			ret = system_valid_cfg_msg();
			if (SYS_OK != ret)
			{
				return ret; /* Error displaying config message */
			}
			system_state = SYSTEM_SET_PASSWORD;
		}
		else
		{
			ret = system_login_pwd();
			if (SYS_OK != ret)
			{
				return ret; /* Error displaying login prompt */
			}
			system_state = SYSTEM_ENTER_PASSWORD;
		}
		break;
	case SYSTEM_SET_PASSWORD:
		if(APP_RESET_VALUE != system_key_pressed)
		{
			delayInit(&delay_fsm_stage, NO_ACTION_INIT_TICK);
			ret = system_set_password(system_key_pressed);
			if (SYS_OK != ret)
			{
				return ret; /* Error setting password */
			}
		}else if(delayRead(&delay_fsm_stage))
		{
			system_timeout_msg();
			memset(curr_password, 0xFF, sizeof(curr_password));
			offset_cfg_password = APP_RESET_VALUE;
			system_state = SYSTEM_IDLE;
			idle_msg = true;
		}
		break;
	case SYSTEM_ENTER_PASSWORD:
		if(APP_RESET_VALUE != system_key_pressed)
		{
			delayInit(&delay_fsm_stage, NO_ACTION_INIT_TICK);
			system_verify_password(system_key_pressed);
		}else if(delayRead(&delay_fsm_stage))
		{
			system_timeout_msg();
			system_state = SYSTEM_IDLE;
			idle_msg = true;
		}
		break;
	case SYSTEM_SAVE_PASSWORD:
		ret = system_save_password();
		if (SYS_OK != ret)
		{
			return ret; /* Error saving password */
		}
		idle_msg = true;
		break;
	case SYSTEM_ACCESS:
		ret = system_access_msg();
		if (SYS_OK != ret)
		{
			return ret; /* Error displaying access message */
		}
		system_state = SYSTEM_IDLE;
		idle_msg = true;
		break;
	default:
		return SYS_ERR_UNKNOWN; /* Invalid state detected */
	}
	port_delay_ms(DELAY_250_MS);
	return ret;
}

SYSTEM_RET system_init(void)
{
	SYSTEM_RET ret = SYS_OK;
	
	/* Initialize GPIO ports and SPI interface */
	ret = port_gpio_init();
	if (SYS_OK != ret)
	{
		return ret; /* GPIO initialization failed */
	}
	
	/* Initialize LCD display */
	ret = lcd_init();
	if (SYS_OK != ret)
	{
		return ret; /* LCD initialization failed */
	}
	
	/* Initialize MCP23S17 keypad controller */
	ret = mcp_init();
	if (SYS_OK != ret)
	{
		return ret; /* MCP initialization failed */
	}
	
	/* Initialize system delay timer */
	ret = system_delay_init();
	if (SYS_OK != ret)
	{
		return ret; /* Delay initialization failed */
	}
	
	/* Reset attempt counter and login resources on startup */
	fail_pwd_count = APP_RESET_VALUE;

	return SYS_OK;
}
