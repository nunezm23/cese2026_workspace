/**
  ******************************************************************************
  * @file           : API_cmdparser.h
  * @brief          : API common to avoid repeate definitions and another importants parameters.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  ******************************************************************************
*/

#ifndef __API_cmdparser_H /**< To prevent recursive inclusion*/
#define __API_cmdparser_H

/**
 * @brief Maximum command line.
 * */
#define CMD_MAX_LINE		64

/**
 * @brief Maximum quantity of tokens.
 * */
#define CMD_MAX_TOKENS		3

/**
 * @brief Status errors enum.
 * */
typedef enum
{
	CMD_OK = 0,				/**< Command OK.*/
	CMD_ERR_OVERFLOW,		/**< Overflow.*/
	CMD_ERR_SYNTAX,			/**< Syntax error.*/
	CMD_ERR_UNKNOWN,		/**< Unknown error.*/
	CMD_ERR_ARG,			/**< Argument error.*/
	CMD_ERR_UNKNOWN_COMMAND /**< Unknown command*/
}cmd_status_t;

/**
 * @brief FSM states enum.
 * 
 */
typedef enum
{
	CMD_IDLE = 0,			/**< Waiting for the first character (no-terminator)*/
	CMD_RECEIVING,			/**< Accumulator for characters in buffer.*/
	CMD_PROCESS,			/**< Token, validate commands and arguments.*/
	CMD_EXEC,				/**< Execute actions and return to CMD_IDLE.*/
	CMD_ERROR,				/**< Print error message and return to CMD_IDLE.*/
}cmd_fsm_state_t;

/**
 * @brief Initialize the command parser API.
 * 
 * @return void
 */
void cmdParserInit(void);

/**
 * @brief Process the command parser with incoming uart data.
 * 
 * @attention This function validates the incoming UART data and verify if it corresponds to a valid character for the command parser.
 * At beginning, the function will be in the CMD_IDLE state, waiting for the first character of a command. When a character is received, it will transition to the CMD_RECEIVING state, where it will accumulate characters in a buffer until it receives a command terminator (e.g., newline).
 * Once a complete command is received, it will transition to the CMD_PROCESS state, where it will tokenize the command and validate it against known commands and arguments. If the command is valid, it will transition to the CMD_EXEC state to execute the corresponding actions. 
 * If any error occurs during processing (e.g., syntax error, unknown command), it will transition to the CMD_ERROR state, where it will print an error message and return to CMD_IDLE.
 * 
 * @return void
 */
void cmdPoll(void);

/**
 * @brief Print the help message with the available commands.
 * 
 * @return void
 */
void cmdPrintHelp(void);

#endif /* __API_cmdparser_H */
