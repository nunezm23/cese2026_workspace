/**
  ******************************************************************************
  * @file           : API_uart.h
  * @brief          : API uart to handle UART communication.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  ******************************************************************************
*/

#ifndef __API_uart_H /**< To prevent recursive inclusion*/
#define __API_uart_H

#include "API_common.h"

/**
 * @brief Maximum size of the UART buffer.
 * 
 */
#define MAX_UART_SIZE 256

/**
 * @brief Minimum size of the UART buffer.
 * 
 */
#define MIN_UART_SIZE 1

/**
 * @brief UART initialization.
 * 
 * @return bool_t
 * @retval true if the initialization was successful, false otherwise.
 * */
bool_t uartInit();

/**
 * @brief UART send string to serial port.
 * 
 * @param[in] pstring Pointer to the string to send.
 * 
 * @return void
 * */
void uartSendString(uint8_t * pstring);

/**
 * @brief UART send string with specified size.
 * 
 * @param[in] pstring Pointer to the string to send.
 * @param[in] pstring_size Size of the string to send.
 * 
 * @return void
 * */
void uartSendStringSize(uint8_t * pstring, uint16_t pstring_size);

/**
 * @brief UART receive string with specified size.
 * 
 * @param[out] pstring Pointer to the buffer to store the received string.
 * @param[out] pstring_size Size of the buffer.
 * 
 * @return uint8_t
 * @retval HAL_OK if the reception was successful, HAL_ERROR or another value otherwise.
 * */
uint8_t uartReceiveStringSize(uint8_t * pstring, uint16_t pstring_size);

/**
 * @brief Receives a string from the serial port and sends it back.
 * @warning This function is just for testing purposes and should not be used in the final code implementation.
 * 
 */
void uartEchoTest(void);

/**
 * @brief Get the UART baudrate.
 * 
 * @param[out] baudrate Pointer to the variable to store the baudrate.
 * 
 * @return void
 * */
void uartGetBaudrate(uint32_t *baudrate);

#endif /* __API_uart_H */
