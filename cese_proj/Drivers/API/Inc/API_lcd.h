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

#include "API_common.h"

#define	LCD_RET		int8_t


typedef enum
{
	LCD_OK = 0,
	LCD_ERR_INIT,
	LCD_ERR_NULL_POINTER,
	LCD_ERR_INVALID_PARAMS,
	LCD_ERR_UNKNOWN,
}lcd_err_t;

LCD_RET lcd_init(void);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_send_string(char *str);
void lcd_put_cur(int row, int col);
void lcd_clear(void);


#endif /* __API_lcd_H */
