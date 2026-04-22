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

/**
 * @brief MCP23S17 I2C hardware address (pins tied to GND).
 * */
#define MCP23S17_ADDR 0x00

/**
 * @brief Write operation opcode for MCP23S17 SPI communication.
 * @note Combines write bit (0x40) with device address.
 * */
#define OPCODE_WRITE  (0x40 | (MCP23S17_ADDR << 1))

/**
 * @brief Read operation opcode for MCP23S17 SPI communication.
 * @note Combines read bit (0x41) with device address.
 * */
#define OPCODE_READ   (0x41 | (MCP23S17_ADDR << 1))

/**
 * @brief Port A I/O Direction Register (BANK = 0).
 * @note Used to configure GPIO pins as inputs (1) or outputs (0).
 * */
#define IODIRA  0x00

/**
 * @brief Port A Pull-Up Resistor Register (BANK = 0).
 * @note Used to enable internal pull-up resistors on input pins.
 * */
#define GPPUA   0x0C

/**
 * @brief Port A General Purpose I/O Register (BANK = 0).
 * @note Used to read/write GPIO pin states on Port A.
 * */
#define MCP_GPIOA   0x12

/**
 * @brief SPI Chip Select pin number.
 * */
#define SPI_CS_Pin GPIO_PIN_6

/**
 * @brief SPI Chip Select GPIO port.
 * */
#define SPI_CS_GPIO_Port GPIOB


/**
 * @brief SPI peripheral handler instance.
 * @note Configured for communication with MCP23S17 I/O expander.
 * */
static SPI_HandleTypeDef hspi1;

/**
 * @brief Keypad matrix mapping for 4x4 key layout.
 * @note Maps physical matrix positions to ASCII character representations.
 * */
char keymap[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};


/**
 * @brief Writes a byte to a specified MCP23S17 register via SPI.
 * @param[in] reg Register address to write to.
 * @param[in] data Byte value to write to the register.
 * @return MCP_RET Status code indicating success or failure.
 * @retval MCP_OK on successful write.
 * @retval MCP_ERR_SPI on SPI transmission error.
 * @note Performs manual chip select management via GPIO.
 * */
static MCP_RET MCP_WriteReg(uint8_t reg, uint8_t data);

/**
 * @brief Reads a byte from a specified MCP23S17 register via SPI.
 * @param[in] reg Register address to read from.
 * @param[out] data Pointer to store the read byte value.
 * @return MCP_RET Status code indicating success or failure.
 * @retval MCP_OK on successful read.
 * @retval MCP_ERR_SPI on SPI transmission/receive error.
 * @note Performs manual chip select management via GPIO.
 * */
static MCP_RET MCP_ReadReg(uint8_t reg, uint8_t *data);

/**
 * @brief Scans the keypad matrix and returns the pressed key.
 * @return char Character representing the pressed key, or 0 if no key is pressed.
 * @note Performs row-by-row scanning with debounce delay.
 * */
static char mcp_scan_keypad(void);

/**
 * @brief Configures MCP23S17 port A for keypad matrix operation.
 * @return MCP_RET Status code.
 * @retval MCP_OK on successful configuration.
 * @retval MCP_ERR_SPI on SPI communication error.
 * @note Configures rows (GPA0-GPA3) as outputs, columns (GPA4-GPA7) as inputs with pull-ups.
 * */
static MCP_RET mcp_config_params(void);

/**
 * @brief Initializes the SPI peripheral for MCP23S17 communication.
 * @return MCP_RET Status code.
 * @retval MCP_OK on successful initialization.
 * @retval MCP_ERR_INIT on initialization failure.
 * @note Configures SPI1 in master mode with appropriate timing parameters.
 * */
static MCP_RET mcp_spi_init(void);

/**
 * @brief Retrieves the SPI chip select GPIO port.
 * @param[out] spi_port Pointer to store the GPIO port address.
 * @return MCP_RET Status code.
 * @retval MCP_OK on success.
 * @retval MCP_ERR_NULL if spi_port is NULL.
 * @note Used by peripheral initialization routines.
 * */
static MCP_RET mcp_get_spi_port_internal(void **spi_port);

/**
 * @brief Retrieves the SPI chip select GPIO pin number.
 * @param[out] spi_pin Pointer to store the GPIO pin value.
 * @return MCP_RET Status code.
 * @retval MCP_OK on success.
 * @retval MCP_ERR_NULL if spi_pin is NULL.
 * @note Used by peripheral initialization routines.
 * */
static MCP_RET mcp_get_spi_cs_pin_internal(uint16_t *spi_pin);

static MCP_RET MCP_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t tx_data[3] = {OPCODE_WRITE, reg, data};
    HAL_StatusTypeDef hal_ret;
    
    /* Pull chip select low to enable device */
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    
    /* Transmit write command, register address, and data */
    hal_ret = HAL_SPI_Transmit(&hspi1, tx_data, 3, 100);
    
    /* Pull chip select high to disable device */
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    
    /* Validate SPI transmission */
    if (HAL_OK != hal_ret)
    {
        return MCP_ERR_SPI;
    }
    
    return MCP_OK;
}

static MCP_RET MCP_ReadReg(uint8_t reg, uint8_t *data)
{
    uint8_t tx_data[2] = {OPCODE_READ, reg};
    uint8_t rx_data = 0;
    HAL_StatusTypeDef hal_ret;
    
    /* Validate input parameter */
    if (NULL == data)
    {
        return MCP_ERR_NULL;
    }
    
    /* Pull chip select low to enable device */
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    
    /* Transmit read command and register address */
    hal_ret = HAL_SPI_Transmit(&hspi1, tx_data, 2, 100);
    if (HAL_OK != hal_ret)
    {
        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
        return MCP_ERR_SPI;
    }
    
    /* Receive the register data */
    hal_ret = HAL_SPI_Receive(&hspi1, &rx_data, 1, 100);
    
    /* Pull chip select high to disable device */
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    
    /* Validate SPI reception */
    if (HAL_OK != hal_ret)
    {
        return MCP_ERR_SPI;
    }
    
    *data = rx_data;
    return MCP_OK;
}

static char mcp_scan_keypad(void)
{
    for (uint8_t row = 0; row < 4U; row++)
    {
        /* Drive only one row LOW at a time (GPA0 to GPA3) */
        /* 0x0F means all rows high. ~(1 << row) pulls one row low */
        uint8_t rowData = (~(1U << row)) & 0x0FU;
        MCP_RET write_ret = MCP_WriteReg(MCP_GPIOA, rowData);
        if (MCP_OK != write_ret)
        {
            continue; /* Skip this row on SPI error */
        }

        /* Give a tiny delay for signal to settle */
        HAL_Delay(1U);

        /* Read the columns (GPA4 to GPA7) */
        uint8_t portStatus = 0;
        MCP_RET read_ret = MCP_ReadReg(MCP_GPIOA, &portStatus);
        if (MCP_OK != read_ret)
        {
            continue; /* Skip this row on SPI error */
        }

        /* Shift right by 4 to get columns to lower bits, isolate them, and invert */
        /* so a pressed button (which goes low) reads as a '1' bit */
        uint8_t cols = (~(portStatus >> 4U)) & 0x0FU;

        if (0x00U != cols) /* If any column bit is 1, a button is pressed */
        {
            for (uint8_t col = 0; col < 4U; col++)
            {
                if (cols & (1U << col))
                {
                    return keymap[row][col]; /* Return the mapped key */
                }
            }
        }
    }
    return 0; /* No key pressed */
}

static MCP_RET mcp_config_params(void)
{
	MCP_RET write_ret = MCP_OK;
	
	/* Configure Port A Direction */
	/* GPA0-GPA3 (Rows) = Outputs (0) */
	/* GPA4-GPA7 (Cols) = Inputs  (1) */
	/* Therefore IODIRA = 1111 0000 = 0xF0 */
	write_ret = MCP_WriteReg(IODIRA, 0xF0U);
	if (MCP_OK != write_ret)
	{
		return write_ret; /* Error configuring I/O direction */
	}

	/* Enable Pull-Up resistors on the input columns (GPA4-GPA7) */
	/* We want GPPUA = 1111 0000 = 0xF0 */
	write_ret = MCP_WriteReg(GPPUA, 0xF0U);
	if (MCP_OK != write_ret)
	{
		return write_ret; /* Error configuring pull-up resistors */
	}

	return MCP_OK;
}

static MCP_RET mcp_spi_init(void)
{
	HAL_StatusTypeDef hal_ret;
	
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
	
	/* Initialize SPI peripheral */
	hal_ret = HAL_SPI_Init(&hspi1);
	if (HAL_OK != hal_ret)
	{
		return MCP_ERR_INIT; /* SPI initialization failed */
	}
	
	return MCP_OK;
}

MCP_RET mcp_get_spi_port(void **spi_port)
{
	/* Validate input parameter */
	if (NULL == spi_port)
	{
		return MCP_ERR_NULL; /* Invalid pointer provided */
	}
	
	*spi_port = (void *)SPI_CS_GPIO_Port;
	return MCP_OK;
}

MCP_RET mcp_get_spi_cs_pin(uint16_t *spi_pin)
{
	/* Validate input parameter */
	if (NULL == spi_pin)
	{
		return MCP_ERR_NULL; /* Invalid pointer provided */
	}
	
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
