/**
  ******************************************************************************
  * @file           : API_lcd.c
  * @brief          : API lcd to manage a 16x4 display user-interactions.
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  * @date           : 2024-04-18
  ******************************************************************************
*/

#include "API_lcd.h"

/**
 * @brief I2C slave address for LCD display with backlight controller (PCF8574).
 * @note Standard address 0x27 shifted left by 1 for I2C protocol.
 * */
#define SLAVE_ADDRESS_LCD 0x4E

/**
 * @brief LCD display width in characters.
 * */
#define LCD_WIDTH 20U

/**
 * @brief LCD display height in rows.
 * */
#define LCD_HEIGHT 4U

/**
 * @brief I2C timeout value in milliseconds for communication.
 * */
#define I2C_TIMEOUT_MS 100U

/**
 * @brief Initial delay after I2C transmission in milliseconds.
 * */
#define LCD_INIT_DELAY_50MS 50U

/**
 * @brief Secondary initialization delay in milliseconds.
 * */
#define LCD_INIT_DELAY_5MS 5U

/**
 * @brief Short operation delay in milliseconds.
 * */
#define LCD_INIT_DELAY_1MS 1U

/**
 * @brief Medium initialization delay in milliseconds.
 * */
#define LCD_INIT_DELAY_10MS 10U

/**
 * @brief Delay for clear display operation in milliseconds.
 * */
#define LCD_CLEAR_DELAY_2MS 2U

/**
 * @brief HD44780 data bits mask (D7-D4 in 8-bit representation).
 * */
#define LCD_DATA_BITS_MASK 0xF0U

/**
 * @brief I2C control byte with backlight enabled and enable signal high.
 * */
#define LCD_CONTROL_EN_HIGH 0x0CU

/**
 * @brief I2C control byte with backlight enabled and enable signal low.
 * */
#define LCD_CONTROL_EN_LOW 0x08U

/**
 * @brief I2C control byte with backlight enabled, enable high, and RS=1 (data mode).
 * */
#define LCD_CONTROL_DATA_EN_HIGH 0x0DU

/**
 * @brief I2C control byte with backlight enabled, enable low, and RS=1 (data mode).
 * */
#define LCD_CONTROL_DATA_EN_LOW 0x09U

/**
 * @brief HD44780 command: 8-bit mode initialization (first step).
 * */
#define LCD_CMD_INIT_8BIT_1 0x30U

/**
 * @brief HD44780 command: Switch to 4-bit mode.
 * */
#define LCD_CMD_SWITCH_4BIT 0x20U

/**
 * @brief HD44780 command: Function set - 4-bit mode, 2 lines, 5x8 font.
 * */
#define LCD_CMD_FUNCTION_SET 0x28U

/**
 * @brief HD44780 command: Turn off display.
 * */
#define LCD_CMD_DISPLAY_OFF 0x08U

/**
 * @brief HD44780 command: Clear display and return cursor to home.
 * */
#define LCD_CMD_CLEAR_DISPLAY 0x01U

/**
 * @brief HD44780 command: Entry mode - increment cursor position.
 * */
#define LCD_CMD_ENTRY_MODE 0x06U

/**
 * @brief HD44780 command: Display on, cursor off.
 * */
#define LCD_CMD_DISPLAY_ON 0x0CU

/**
 * @brief LCD row address for row 0 (first line).
 * */
#define LCD_ROW_0_ADDR 0x80U

/**
 * @brief LCD row address for row 1 (second line).
 * */
#define LCD_ROW_1_ADDR 0xC0U

/**
 * @brief LCD row address for row 2 (third line).
 * */
#define LCD_ROW_2_ADDR 0x94U

/**
 * @brief LCD row address for row 3 (fourth line).
 * */
#define LCD_ROW_3_ADDR 0xD4U

/**
 * @brief I2C peripheral handler instance.
 * @note Configured for communication with LCD display via PCF8574 I2C expander.
 * */
static I2C_HandleTypeDef hi2c1;

/**
 * @brief Sends an HD44780 command to the LCD display via I2C.
 * @param[in] cmd Command byte to send to the display.
 * @return LCD_RET Status code.
 * @retval LCD_OK on successful transmission.
 * @retval LCD_ERR_I2C on I2C communication error.
 * @note Command is sent in 4-bit mode with proper enable timing.
 * */
static LCD_RET lcd_send_cmd_internal(uint8_t cmd);

/**
 * @brief Initializes the I2C peripheral for LCD communication.
 * @return LCD_RET Status code.
 * @retval LCD_OK on successful initialization.
 * @retval LCD_ERR_INIT on initialization failure.
 * @note Configures I2C1 with 100kHz clock speed.
 * */
static LCD_RET lcd_i2c_init(void);

/**
 * @brief Configures LCD display parameters and initializes HD44780 controller.
 * @return LCD_RET Status code.
 * @retval LCD_OK on successful configuration.
 * @retval LCD_ERR_I2C on I2C communication error during configuration.
 * @note Executes HD44780 power-up sequence for 4-bit mode.
 * */
static LCD_RET lcd_config(void);

/**
 * @brief Sends an HD44780 command to the LCD display via I2C.
 * @param[in] cmd Command byte to send to the display.
 * @return LCD_RET Status code.
 * @retval LCD_OK on successful transmission.
 * @retval LCD_ERR_I2C on I2C communication error.
 * @note Command is sent in 4-bit mode with proper enable timing.
 * */
static LCD_RET lcd_send_cmd_internal(uint8_t cmd)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4];
    HAL_StatusTypeDef hal_ret;

    /* Separate command into high nibble (D7-D4) and low nibble (D3-D0) */
    data_u = (cmd & LCD_DATA_BITS_MASK);
    data_l = ((cmd << 4U) & LCD_DATA_BITS_MASK);

    /* Build I2C transmission sequence: [D7 D6 D5 D4 | Backlight(1) Enable(1/0) RW(0) RS(0)] */
    data_t[0] = data_u | LCD_CONTROL_EN_HIGH;  /* Enable = 1 */
    data_t[1] = data_u | LCD_CONTROL_EN_LOW;   /* Enable = 0 */
    data_t[2] = data_l | LCD_CONTROL_EN_HIGH;  /* Enable = 1 */
    data_t[3] = data_l | LCD_CONTROL_EN_LOW;   /* Enable = 0 */

    /* Transmit 4 bytes via I2C */
    hal_ret = HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *)data_t, 4U, I2C_TIMEOUT_MS);
    if (HAL_OK != hal_ret)
    {
        return LCD_ERR_I2C; /* I2C transmission error */
    }
    
    return LCD_OK;
}


LCD_RET lcd_send_data(uint8_t data)
{
    uint8_t data_u, data_l;
    uint8_t data_t[4];
    HAL_StatusTypeDef hal_ret;

    /* Separate data into high nibble (D7-D4) and low nibble (D3-D0) */
    data_u = (data & LCD_DATA_BITS_MASK);
    data_l = ((data << 4U) & LCD_DATA_BITS_MASK);

    /* Build I2C transmission sequence with RS=1 (data mode) */
    data_t[0] = data_u | LCD_CONTROL_DATA_EN_HIGH;  /* Enable = 1, RS = 1 */
    data_t[1] = data_u | LCD_CONTROL_DATA_EN_LOW;   /* Enable = 0, RS = 1 */
    data_t[2] = data_l | LCD_CONTROL_DATA_EN_HIGH;  /* Enable = 1, RS = 1 */
    data_t[3] = data_l | LCD_CONTROL_DATA_EN_LOW;   /* Enable = 0, RS = 1 */

    /* Transmit 4 bytes via I2C */
    hal_ret = HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *)data_t, 4U, I2C_TIMEOUT_MS);
    if (HAL_OK != hal_ret)
    {
        return LCD_ERR_I2C; /* I2C transmission error */
    }
    
    return LCD_OK;
}

static LCD_RET lcd_i2c_init(void)
{
  HAL_StatusTypeDef hal_ret;
  
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  
  /* Initialize I2C peripheral */
  hal_ret = HAL_I2C_Init(&hi2c1);
  if (HAL_OK != hal_ret)
  {
    return LCD_ERR_INIT; /* I2C initialization failed */
  }

  return LCD_OK;
}

static LCD_RET lcd_config(void)
{
    LCD_RET config_ret = LCD_OK;
    
    /* HD44780 power-up sequence for 4-bit mode initialization */
    /* These delays and commands are critical for proper initialization */
    
    HAL_Delay(LCD_INIT_DELAY_50MS);
    config_ret = lcd_send_cmd_internal(LCD_CMD_INIT_8BIT_1);
    if (LCD_OK != config_ret) return config_ret;
    
    HAL_Delay(LCD_INIT_DELAY_5MS);
    config_ret = lcd_send_cmd_internal(LCD_CMD_INIT_8BIT_1);
    if (LCD_OK != config_ret) return config_ret;
    
    HAL_Delay(LCD_INIT_DELAY_1MS);
    config_ret = lcd_send_cmd_internal(LCD_CMD_INIT_8BIT_1);
    if (LCD_OK != config_ret) return config_ret;
    
    HAL_Delay(LCD_INIT_DELAY_10MS);
    config_ret = lcd_send_cmd_internal(LCD_CMD_SWITCH_4BIT);  /* Switch to 4-bit mode */
    if (LCD_OK != config_ret) return config_ret;
    
    HAL_Delay(LCD_INIT_DELAY_10MS);
    config_ret = lcd_send_cmd_internal(LCD_CMD_FUNCTION_SET);  /* Function set: 4-bit, 2 lines, 5x8 */
    if (LCD_OK != config_ret) return config_ret;
    
    HAL_Delay(LCD_INIT_DELAY_1MS);
    config_ret = lcd_send_cmd_internal(LCD_CMD_DISPLAY_OFF);   /* Turn off display */
    if (LCD_OK != config_ret) return config_ret;
    
    HAL_Delay(LCD_INIT_DELAY_1MS);
    config_ret = lcd_send_cmd_internal(LCD_CMD_CLEAR_DISPLAY); /* Clear display */
    if (LCD_OK != config_ret) return config_ret;
    
    HAL_Delay(LCD_CLEAR_DELAY_2MS);
    config_ret = lcd_send_cmd_internal(LCD_CMD_ENTRY_MODE);    /* Entry mode: increment cursor */
    if (LCD_OK != config_ret) return config_ret;
    
    HAL_Delay(LCD_INIT_DELAY_1MS);
    config_ret = lcd_send_cmd_internal(LCD_CMD_DISPLAY_ON);    /* Display on, cursor off */
    
    return config_ret;
}

LCD_RET lcd_init(void)
{
	LCD_RET init_ret = LCD_OK;
	lcd_i2c_init();
	lcd_config();
	return init_ret;
}

LCD_RET lcd_send_string(char *str) {
    while (*str) {
        lcd_send_data(*str++);
    }
    return LCD_OK;
}

LCD_RET lcd_put_cur(int row, int col) {
    // Direcciones de memoria para pantallas de 20x4
    switch (row) {
        case 0: col |= 0x80; break;
        case 1: col |= 0xC0; break;
        case 2: col |= 0x94; break;
        case 3: col |= 0xD4; break;
    }
    lcd_send_cmd_internal(col);
    return LCD_OK;
}

LCD_RET lcd_clear(void){
    lcd_send_cmd_internal(0x01);
    HAL_Delay(1);
    return LCD_OK;
}
