/**
  ******************************************************************************
  * @file           : API_uart.c
  * @brief          : UART functions implementations for API.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  ******************************************************************************
*/

/**
 * @brief Including header file for API UART implementation
 * */
#include "API_uart.h"

/**
 * @brief Including header file for API common implementation
 * */
#include "API_common.h"

/**
 * @brief UART handle for USART2
 */
static UART_HandleTypeDef huart2;

/**
 * @brief UART configuration constants for 8 bits.
 */
#define UART_CFG_8_BITS			"8 Bits"

/**
 * @brief UART configuration constants for 9 bits.
 */
#define UART_CFG_9_BITS			"9 Bits"

/**
 * @brief UART configuration constants for none parity settings.
 */
#define UART_CFG_PARITY_NONE	"None"

/**
 * @brief UART configuration constants for even parity settings.
 */
#define UART_CFG_PARITY_EVEN	"Even"

/**
 * @brief UART configuration constants for odd parity settings.
 */
#define UART_CFG_PARITY_ODD		"Odd"

/**
 * @brief UART configuration constants for 1 stop bit settings.
 */
#define UART_CFG_1_BITS			"1 Bits"

/**
 * @brief UART configuration constants for 2 stop bit settings.
 */
#define UART_CFG_2_BITS			"2 Bits"

/**
 * @brief UART echo receive size one bit per read.
 * 
 */
#define UART_ECHO_RX_SIZE		1

/**
 * @brief Invalid read value for UART receive functions.
 */
#define INVALID_READ_VALUE		-1

/**
 * @brief Maximum time for UART read operations.
 * 
 */
#define MAX_TIME_READ_UART		100


bool_t uartInit(void)
{
	bool_t uart_ret = false;

	huart2.Instance = USART2;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.BaudRate = 115200;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;

	uart_ret = HAL_UART_Init(&huart2);

	if (HAL_OK != uart_ret)
	{
		return uart_ret;
	}

	char log_buffer[MAX_UART_SIZE] = {API_RESET_VALUE};

	const char* word_length = (huart2.Init.WordLength == UART_WORDLENGTH_8B) ? UART_CFG_8_BITS : UART_CFG_9_BITS;

	const char* parity = (huart2.Init.Parity == UART_PARITY_NONE) ? UART_CFG_PARITY_NONE :
	                       (huart2.Init.Parity == UART_PARITY_EVEN) ? UART_CFG_PARITY_EVEN : UART_CFG_PARITY_ODD;

	const char* stop_bits = (huart2.Init.StopBits == UART_STOPBITS_1) ? UART_CFG_1_BITS : UART_CFG_2_BITS;

	int len = snprintf(log_buffer, sizeof(log_buffer),
	      "Baud Rate:   %lu bps\r\n"
	      "Word Length: %s\r\n"
	      "Parity:      %s\r\n"
	      "Stop Bits:   %s\r\n",
	      huart2.Init.BaudRate,
	      word_length,
	      parity,
	      stop_bits);

    if (len > API_RESET_VALUE){
    	uartSendString((uint8_t*)log_buffer);
    }
	return uart_ret;
}

void uartSendString(uint8_t * pstring)
{
	if(NULL == pstring)
	{
		return;
	}
	size_t len = strlen((char*)pstring);
	if(len > MAX_UART_SIZE || len < MIN_UART_SIZE){
		return;
	}
	HAL_UART_Transmit(&huart2, pstring, len, HAL_MAX_DELAY);
	return;
}

void uartSendStringSize(uint8_t * pstring, uint16_t pstring_size)
{
	if(NULL == pstring || pstring_size < MIN_UART_SIZE || pstring_size > MAX_UART_SIZE)
	{
		return;
	}
	HAL_UART_Transmit(&huart2, pstring, pstring_size, HAL_MAX_DELAY);
	return;
}

uint8_t uartReceiveStringSize(uint8_t * pstring, uint16_t pstring_size)
{
	if(NULL == pstring || pstring_size < MIN_UART_SIZE || pstring_size > MAX_UART_SIZE)
	{
		return INVALID_READ_VALUE;
	}
	return HAL_UART_Receive(&huart2, pstring, pstring_size, MAX_TIME_READ_UART);
}

void uartEchoTest(void)
{
	uint8_t array_buffer[MAX_UART_SIZE] = {API_RESET_VALUE};
	uartReceiveStringSize(array_buffer, UART_ECHO_RX_SIZE);
	uartSendString(array_buffer);
}

void uartGetBaudrate(uint32_t *baudrate)
{
	if(NULL == baudrate)
	{
		return;
	}
	*baudrate = huart2.Init.BaudRate;
}
