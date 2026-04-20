/**
  ******************************************************************************
  * @file           : API_mcp23s17.c
  * @brief          : API mcp23s17 to manage this IOs expansion that establish communication by SPI bus..
  * @author			: Marcos Nuñez
  * @version		: 1.0.0
  * @date           : 2024-04-18
  ******************************************************************************
*/

#include "API_mcp23s17.h"

#define MCP23S17_ADDR 0x00 // Hardware address pins tied to GND
#define OPCODE_WRITE  (0x40 | (MCP23S17_ADDR << 1))
#define OPCODE_READ   (0x41 | (MCP23S17_ADDR << 1))

// MCP23S17 Register Addresses (BANK = 0)
#define IODIRA  0x00  // I/O Direction Register
#define GPPUA   0x0C  // Pull-Up Resistor Register
#define MCP_GPIOA   0x12  // General Purpose I/O Port Register


#define SPI_CS_Pin GPIO_PIN_6
#define SPI_CS_GPIO_Port GPIOB


static SPI_HandleTypeDef hspi1;

char keymap[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};


void MCP_WriteReg(uint8_t reg, uint8_t data) {
    uint8_t tx_data[3] = {OPCODE_WRITE, reg, data};
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET); // Pull CS Low
    HAL_SPI_Transmit(&hspi1, tx_data, 3, 100);                       // Send 3 bytes
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);   // Pull CS High
}

// Function to read from an MCP23S17 register
uint8_t MCP_ReadReg(uint8_t reg) {
    uint8_t tx_data[2] = {OPCODE_READ, reg};
    uint8_t rx_data = 0;

    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx_data, 2, 100);                       // Send Opcode & Reg
    HAL_SPI_Receive(&hspi1, &rx_data, 1, 100);                       // Read the byte back
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

    return rx_data;
}

// Function to scan the matrix keypad
char mcp_scan_keypad(void) {
    for (int row = 0; row < 4; row++) {
        // Drive only one row LOW at a time (GPA0 to GPA3).
        // 0x0F means all rows high. ~(1 << row) pulls one row low.
        uint8_t rowData = ~(1 << row) & 0x0F;
        MCP_WriteReg(MCP_GPIOA, rowData);

        // Give a tiny delay for signal to settle
        HAL_Delay(1);

        // Read the columns (GPA4 to GPA7)
        uint8_t portStatus = MCP_ReadReg(MCP_GPIOA);

        // Shift right by 4 to get columns to lower bits, isolate them, and invert
        // so a pressed button (which goes low) reads as a '1' bit
        uint8_t cols = (~(portStatus >> 4)) & 0x0F;

        if (cols != 0x00) { // If any column bit is 1, a button is pressed
            for (int col = 0; col < 4; col++) {
                if (cols & (1 << col)) {
                    return keymap[row][col]; // Return the mapped key
                }
            }
        }
    }
    return 0; // No key pressed
}

static MCP_RET mcp_config_params(void)
{
	// 1. Configure Port A Direction
	// GPA0-GPA3 (Rows) = Outputs (0)
	// GPA4-GPA7 (Cols) = Inputs  (1)
	// Therefore IODIRA = 1111 0000 = 0xF0
	MCP_WriteReg(IODIRA, 0xF0);

	// 2. Enable Pull-Up resistors on the input columns (GPA4-GPA7)
	// We want GPPUA = 1111 0000 = 0xF0
	MCP_WriteReg(GPPUA, 0xF0);

	return MCP_OK;
}

static MCP_RET mcp_spi_init(void)
{
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		//Error_Handler();
		return MCP_ERR_INIT;
	}
	return MCP_OK;
}

MCP_RET mcp_get_spi_port(void **spi_port)
{
	*spi_port = (void *)SPI_CS_GPIO_Port;
	return MCP_OK;
}

MCP_RET mcp_get_spi_cs_pin(uint16_t *spi_pin)
{
	*spi_pin = SPI_CS_Pin;
	return MCP_OK;
}

MCP_RET mcp_init(void)
{
	MCP_RET init_ret = MCP_ERR_UNKNOWN;
	init_ret = mcp_spi_init();
	if(MCP_OK != init_ret)
	{
		return init_ret;
	}
	init_ret = mcp_config_params();
	if(MCP_OK != init_ret)
	{
		return init_ret;
	}
	return init_ret;
}
