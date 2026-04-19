#include "i2c_lcd.h"

// Obtenemos la referencia al I2C1 que CubeIDE generó en main.c
extern I2C_HandleTypeDef hi2c1;

// Dirección I2C desplazada a la izquierda (0x27 << 1)
#define SLAVE_ADDRESS_LCD 0x4E

void lcd_send_cmd(char cmd) {
    char data_u, data_l;
    uint8_t data_t[4];

    // Separamos el comando en 4 bits altos y 4 bits bajos
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);

    // Armamos la trama I2C: [D7 D6 D5 D4 | Luz(1) En(1/0) RW(0) RS(0)]
    data_t[0] = data_u | 0x0C;  // Enable = 1
    data_t[1] = data_u | 0x08;  // Enable = 0
    data_t[2] = data_l | 0x0C;  // Enable = 1
    data_t[3] = data_l | 0x08;  // Enable = 0

    // Enviamos los 4 bytes por I2C
    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *) data_t, 4, 100);
}

void lcd_send_data(char data) {
    char data_u, data_l;
    uint8_t data_t[4];

    data_u = (data & 0xf0);
    data_l = ((data << 4) & 0xf0);

    // Igual que el comando, pero con RS = 1 (Registro de datos)
    data_t[0] = data_u | 0x0D;  // Enable = 1, RS = 1
    data_t[1] = data_u | 0x09;  // Enable = 0, RS = 1
    data_t[2] = data_l | 0x0D;  // Enable = 1, RS = 1
    data_t[3] = data_l | 0x09;  // Enable = 0, RS = 1

    HAL_I2C_Master_Transmit(&hi2c1, SLAVE_ADDRESS_LCD, (uint8_t *) data_t, 4, 100);
}

void lcd_init(void) {
    // Secuencia mágica de inicialización para 4-bits requerida por el chip HD44780
    HAL_Delay(50);
    lcd_send_cmd(0x30);
    HAL_Delay(5);
    lcd_send_cmd(0x30);
    HAL_Delay(1);
    lcd_send_cmd(0x30);
    HAL_Delay(10);
    lcd_send_cmd(0x20);  // Cambiar a modo 4-bits
    HAL_Delay(10);

    lcd_send_cmd(0x28); // Función set: 4-bit, 2 líneas, 5x8
    HAL_Delay(1);
    lcd_send_cmd(0x08); // Apagar display
    HAL_Delay(1);
    lcd_send_cmd(0x01); // Limpiar display
    HAL_Delay(2);
    lcd_send_cmd(0x06); // Modo de entrada: incrementar cursor
    HAL_Delay(1);
    lcd_send_cmd(0x0C); // Encender display, apagar cursor
}

void lcd_send_string(char *str) {
    while (*str) lcd_send_data(*str++);
}

void lcd_put_cur(int row, int col) {
    // Direcciones de memoria para pantallas de 20x4
    switch (row) {
        case 0: col |= 0x80; break;
        case 1: col |= 0xC0; break;
        case 2: col |= 0x94; break;
        case 3: col |= 0xD4; break;
    }
    lcd_send_cmd(col);
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    HAL_Delay(2); // Limpiar toma más tiempo
}
