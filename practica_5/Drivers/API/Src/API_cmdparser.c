/**
  ******************************************************************************
  * @file           : API_cmdparser.c
  * @brief          : Command parser functions implementations for API.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  ******************************************************************************
*/

/**
 * @brief Including header file for API common implementation
 * */
#include "API_common.h"

/**
 * @brief Including header file for API command parser implementation
 * */
#include "API_cmdparser.h"

/**
 * @brief Including header file for API UART implementation
 * */
#include "API_uart.h"

/**
 * @brief Character carriage return for command termination.
 * */
#define CHAR_CR '\r'

/**
 * @brief Character line feed for command termination.
 * */
#define CHAR_LF '\n'

/**
 * @brief Invalid character num for command parsing.
 */
#define INVALID_ASTERISK	'#'

/**
 * @brief Invalid character comment for command parsing.
 */
#define INVALID_COMMENT		'\\'

/**
 * @brief GPIO pin for LD2 LED.
 */
#define LD2_Pin 		GPIO_PIN_5

/**
 * @brief GPIO port for LD2 LED.
 */
#define LD2_GPIO_Port 	GPIOA

/**
 * @brief Help command string.
 */
#define HELP_COMMAND		"HELP"

/**
 * @brief Default invalid command value.
 */
#define DEFAULT_INVALID_COMMAND		0xFF

/**
 * @brief One value for omitting the help command from the menu array.
 */
#define ONE_VALUE					1

/**
 * @brief Current finite state machine state for command parsing.
 */
static cmd_fsm_state_t current_fsm_state = CMD_IDLE;

/**
 * @brief Command buffer for storing incoming commands.
 */
static char buffer_cmd[MAX_UART_SIZE] = {API_RESET_VALUE};

/**
 * @brief Size of the command currently stored in the buffer.
 */
static uint16_t buffer_cmd_size = API_RESET_VALUE;

/**
 * @brief Flag to indicate if the command parser API has been initialized.
 */
static bool_t is_ParsedInit = false;

/**
 * @brief Structure for defining serial options.
 */
typedef struct {
    int id;						/**< ID of the serial option */
    const char* description;	/**< Description of the serial option */
}serial_options_st;

/**
 * @brief Enum for defining available commands.
 */
typedef enum
{
	CMD_HELP = 0,				/**< Help command */
	CMD_LED_ON,					/**< LED ON command */
	CMD_LED_OFF,				/**< LED OFF command */
	CMD_LED_TOGGLE,				/**< LED TOGGLE command */
	CMD_LED_STATUS,				/**< LED STATUS command */
	CMD_BAUD_GET,				/**< BAUD? command */
	CMD_BAUD_CHANGE,			/**< BAUD= command */
}cmd_parser_t;

/**
 * @brief Array of available commands in the menu.
 */
static serial_options_st menu[] = {
	{CMD_HELP, 			"HELP"},				/**< Help command */
    {CMD_LED_ON, 		"LED ON"},				/**< LED ON command */
    {CMD_LED_OFF, 		"LED OFF"},				/**< LED OFF command */
	{CMD_LED_TOGGLE, 	"LED TOGGLE"},			/**< LED TOGGLE command */
	{CMD_LED_STATUS, 	"STATUS"},				/**< STATUS command */
	{CMD_BAUD_GET, 		"BAUD?"},				/**< BAUD? command */
	{CMD_BAUD_CHANGE, 	"BAUD="}				/**< BAUD= command */
};

/**
 * @brief Array of available finite state machine states.
 */
static serial_options_st fsm_states[] = {
	{CMD_IDLE, 		"FSM_IDLE"},			/**< FSM_IDLE state */
    {CMD_RECEIVING, "FSM_RECEIVING"},		/**< FSM_RECEIVING state */
    {CMD_PROCESS, 	"FSM_PROCESS"},			/**< FSM_PROCESS state */
	{CMD_EXEC, 		"FSM_EXEC"},			/**< FSM_EXEC state */
	{CMD_ERROR, 	"FSM_ERROR"}			/**< FSM_ERROR state */
};

/**
 * @brief Size of the menu array.
 */
#define MENU_SIZE (sizeof(menu) / sizeof(menu[API_RESET_VALUE]))

/**
 * @brief Logs an error based on the error type.
 * 
 * @param[in] error_type The type of error to log.
 * 
 * @return void
 */
static void cmdParserLogError(uint8_t error_type);

/**
 * @brief Cleans up the command buffer and resets its size.
 * 
 * @return void
 */
static void cmdCleanResources(void);

/**
 * @brief Prints the current finite state machine state.
 * 
 * @param[in] fsm_id The ID of the finite state machine state to print.
 * 
 * @return void
 */
static __attribute__((unused)) void cmdPrintCurrentFSMState(uint8_t fsm_id);

/**
 * @brief Prints the current LED state.
 * 
 * @param[in] led_state The state of the LED to print.
 * 
 * @return void
 */
static void cmdLEDPrintState(uint8_t led_state);

/**
 * @brief Sets the state of the LED.
 * 
 * @param[in] led_state The state to set the LED to (ON, OFF, TOGGLE, STATUS).
 * 
 * @return void
 */
static void cmdLEDSetState(uint8_t led_state);

/**
 * @brief Processes the action corresponding to the given menu action ID.
 * 
 * @param[in] menu_act The ID of the menu action to process.
 * @return void
 */
static void cmdProcessAction(uint8_t menu_act);

/**
 * @brief Gets the current UART baudrate.
 * 
 * @return void
 */
static void cmdBaudrateGet(void);

/**
 * @brief Processes a line of input and determines the corresponding command.
 * 
 * @return uint8_t
 * @retval DEFAULT_INVALID_COMMAND if the command is invalid or not recognized.
 */
static uint8_t cmdProcessLine(void);

static void cmdCleanResources(void)
{
	memset(buffer_cmd, API_RESET_VALUE, sizeof(buffer_cmd_size));
	buffer_cmd_size = API_RESET_VALUE;
}

static __attribute__((unused)) void cmdPrintCurrentFSMState(uint8_t fsm_id)
{
	char fsm_state_buffer[MAX_UART_SIZE] = {API_RESET_VALUE};
	snprintf(fsm_state_buffer, sizeof(fsm_state_buffer), "%d. %s\r\n", fsm_states[fsm_id].id + ONE_VALUE, fsm_states[fsm_id].description);
	uartSendString((uint8_t*)fsm_state_buffer);
}

static void cmdLEDPrintState(uint8_t led_state)
{
	if(true == led_state)
	{
		const char *log = "STATE LED: ON\r\n";
		uartSendString((uint8_t *)log);
	}else
	{
		const char *log = "STATE LED: OFF\r\n";
		uartSendString((uint8_t *)log);
	}
}

static void cmdLEDSetState(uint8_t led_state)
{
	if(CMD_LED_ON == led_state)
	{
		const char *log = "LED ON\r\n";
		uartSendString((uint8_t*)log);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	}else if(CMD_LED_OFF == led_state)
	{
		const char *log = "LED OFF\r\n";
		uartSendString((uint8_t*)log);
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	}else if(CMD_LED_TOGGLE == led_state)
	{
		const char *log = "LED TOGGLE\r\n";
		uartSendString((uint8_t*)log);
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}else if(CMD_LED_STATUS == led_state)
	{
		const char *log = "LED STATE\r\n";
		uartSendString((uint8_t*)log);
		uint8_t current_led_state = HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin);
		cmdLEDPrintState(current_led_state);
	}
}

static void cmdBaudrateGet(void)
{
	char buffer[MAX_UART_SIZE] = {API_RESET_VALUE};
	uint32_t curr_uart_baudrate = API_RESET_VALUE;
	uartGetBaudrate(&curr_uart_baudrate);
	if(API_RESET_VALUE != curr_uart_baudrate)
	{
		uint16_t len = snprintf(buffer, sizeof(buffer), "BAUDRATE: %ld[bps]\r\n", curr_uart_baudrate);
		uartSendStringSize((uint8_t *)buffer, len);
	}
	return;
}

static void cmdProcessAction(uint8_t menu_act)
{
	const char *log = "COMMAND PROCESSED\r\n";
	switch(menu_act)
	{
		case CMD_HELP:
			const char *log_help = "HELP\r\n";
			uartSendString((uint8_t*)log_help);
			cmdPrintHelp();
			break;
		case CMD_LED_ON:
		case CMD_LED_OFF:
		case CMD_LED_TOGGLE:
		case CMD_LED_STATUS:
			cmdLEDSetState(menu_act);
			break;
		case CMD_BAUD_GET:
			const char *log_get_baud = "GET CURRENT BAUDRATE\r\n";
			uartSendString((uint8_t*)log_get_baud);
			cmdBaudrateGet();
			break;
		case CMD_BAUD_CHANGE:
			break;
		default:
			break;
	}
	uartSendString((uint8_t *)log);
}

static uint8_t cmdProcessLine(void)
{
	const char *change_baudrate = "BAUD=";
	for(uint8_t i=API_RESET_VALUE; i < MENU_SIZE; i++)
	{
		if(CMD_OK == strncmp(buffer_cmd, menu[i].description, buffer_cmd_size))
		{
			const char *log = "\r\nCOMMAND DETECTED: ";
			uartSendString((uint8_t *)log);
			return i;
		}else if(CMD_OK == strncmp(buffer_cmd, change_baudrate, strlen(change_baudrate)))
		{
			const char *log_change_baud = "\r\nCOMMAND NOT YET IMPLEMENTED: CHANGE BAUDRATE\r\n ";
			uartSendString((uint8_t *)log_change_baud);
			return DEFAULT_INVALID_COMMAND;
		}
	}
	cmdParserLogError(CMD_ERR_UNKNOWN_COMMAND);
	return DEFAULT_INVALID_COMMAND;
}

static void cmdParserLogError(uint8_t error_type)
{
	switch(error_type)
	{
	case CMD_ERR_OVERFLOW:
		const char *log_err_overflow = "ERROR: line too long\r\n";
		uartSendString((uint8_t *)log_err_overflow);
		break;
	case CMD_ERR_ARG:
		const char *log_err_arg = "ERROR: bad arguments\r\n";
		uartSendString((uint8_t *)log_err_arg);
		break;
	case CMD_ERR_UNKNOWN_COMMAND:
		const char *log_err_cmd = "ERROR: unknown command\r\n";
		uartSendString((uint8_t *)log_err_cmd);
		break;
	}
	return;
}

void cmdPrintHelp(void){
    char buffer[MAX_UART_SIZE] = {API_RESET_VALUE};

    snprintf(buffer, sizeof(buffer), "\r\n--- AVAILABLE COMMANDS ---\r\n");
    uartSendString((uint8_t*)buffer);

    for (int i = 1; i < MENU_SIZE; i++) {
        snprintf(buffer, sizeof(buffer), "%d. %s\r\n", menu[i].id, menu[i].description);
        uartSendString((uint8_t*)buffer);
    }

    snprintf(buffer, sizeof(buffer), "HINT: TYPE COMMAND\r\n");
    uartSendString((uint8_t*)buffer);
}

void cmdParserInit(void)
{
	uint8_t parser_ret = CMD_ERR_ARG;
	parser_ret = uartInit();
	if(CMD_OK != parser_ret)
	{
		return;
	}
	const char *log = "HINT: TYPE HELP FOR SHOW COMMANDS AVAILABLES:\r\n";
	uartSendString((uint8_t *)log);
	is_ParsedInit = true;
	return;
}

void cmdPoll(void)
{
	if(false == is_ParsedInit)
	{
		return;
	}
	uint8_t rx_byte = API_RESET_VALUE;
	static uint8_t current_cmd;

	if(HAL_OK != uartReceiveStringSize(&rx_byte, sizeof(rx_byte)))
	{
		if(CMD_PROCESS != current_fsm_state && CMD_EXEC != current_fsm_state && CMD_ERROR != current_fsm_state)
		{
			return;
		}
	}
	if(INVALID_ASTERISK == rx_byte || INVALID_COMMENT == rx_byte)
	{
		cmdParserLogError(CMD_ERR_ARG);
		return;
	}
	switch(current_fsm_state)
	{
		case CMD_IDLE:
			cmdCleanResources();
			current_cmd = DEFAULT_INVALID_COMMAND;
			if((CHAR_CR != rx_byte) && (CHAR_LF != rx_byte))
			{
				buffer_cmd[buffer_cmd_size] = rx_byte;
				buffer_cmd_size++;
				current_fsm_state = CMD_RECEIVING;
			}
			break;
		case CMD_RECEIVING:
			if((CHAR_CR == rx_byte) || (CHAR_LF == rx_byte))
			{
				current_fsm_state = CMD_PROCESS;
			}else if(MAX_UART_SIZE == buffer_cmd_size)
			{
				cmdParserLogError(CMD_ERR_OVERFLOW);
				current_fsm_state = CMD_ERROR;
				return;
			}else
			{
				buffer_cmd[buffer_cmd_size] = rx_byte;
				buffer_cmd_size++;
			}
			break;
		case CMD_PROCESS:
			current_cmd = cmdProcessLine();
			if(DEFAULT_INVALID_COMMAND != current_cmd)
			{
				current_fsm_state = CMD_EXEC;
			}else
			{
				current_fsm_state = CMD_ERROR;
			}
			break;
		case CMD_EXEC:
			cmdProcessAction(current_cmd);
			current_fsm_state = CMD_IDLE;
			break;
		case CMD_ERROR:
			current_fsm_state = CMD_IDLE;
			break;
		default:
			break;
	}
}
