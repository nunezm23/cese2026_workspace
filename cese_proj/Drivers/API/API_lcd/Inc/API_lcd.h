/**
  ******************************************************************************
  * @file           : API_lcd.h
  * @brief          : API lcd to manage a 16x4 display user-interactions.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  * @date           : 2024-04-18
  ******************************************************************************
*/

#ifndef __API_lcd_H /**< To prevent recursive inclusion*/
#define __API_lcd_H

#include <stdint.h>

/**
 * @brief LCD return type definition.
 * @note Used for function return status codes in LCD API.
 */
#define	LCD_RET		int8_t

/**
 * @brief   LCD reset value for initializations or default states.
 */
#define LCD_RESET_VALUE	0x00U

/**
 * @brief LCD error types.
 */
typedef enum
{
	LCD_OK = 0,             /**< LCD process successful */
	LCD_ERR_INIT,           /**< LCD initialization error */
	LCD_ERR_NULL_POINTER,   /**< Null pointer error in function parameters */
  LCD_ERR_I2C,            /**< I2C communication error */ 
	LCD_ERR_INVALID_PARAMS, /**< Invalid parameters provided to function */
	LCD_ERR_UNKNOWN,        /**< Unknown error occurred */
}lcd_err_t;

/**
 * @brief Initializes the LCD display and I2C communication.
 * @note Must be called before any other LCD functions to set up the display.
 *       Executes HD44780 initialization sequence for 4-bit mode.
 *       Configures I2C peripheral for communication with PCF8574 I/O expander.
 *       Delays are included to ensure proper timing during initialization.
 *       Returns appropriate error codes for initialization failures.
 * 
 * @return LCD_RET Status code indicating success or failure of initialization.
 * @retval LCD_OK on successful initialization, LCD_ERR_INIT on failure.
 * */
LCD_RET lcd_init(void);

/**
 * @brief Sends data character to LCD display via I2C.
 * @param[in] data ASCII character to display.
 * @return LCD_RET Status code.
 * @retval LCD_OK on successful transmission.
 * @retval LCD_ERR_I2C on I2C communication error.
 * @note Data is sent in 4-bit mode with proper enable timing.
 * */
LCD_RET lcd_send_data(uint8_t data);

/**
 * @brief Sends a string of characters to the LCD display via I2C.
 * @note String is sent in 4-bit mode with proper enable timing.
 * 
 * @param[in] str Pointer to the string to display.
 * 
 * @return LCD_RET Status code.
 * @retval LCD_OK on successful transmission.
 * */
LCD_RET lcd_send_string(char *str);

/**
 * @brief Sets the cursor position on the LCD display.
 * @note Cursor position is set using HD44780 addressing scheme.
 * 
 * @param[in] row Row number (0 to LCD_HEIGHT-1).
 * @param[in] col Column number (0 to LCD_WIDTH-1).
 * 
 * @return LCD_RET Status code.
 * @retval LCD_OK on successful cursor positioning.
 * */
LCD_RET lcd_put_cur(int row, int col);

/**
 * @brief Clears the LCD display and resets cursor position.
 * @note Clears display using HD44780 clear command, which takes longer to execute.
 * 
 * @return LCD_RET Status code.
 * @retval LCD_OK on successful clear operation.
 * */
LCD_RET lcd_clear(void);

#endif /* __API_lcd_H */
