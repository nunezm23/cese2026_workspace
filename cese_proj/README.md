# CESE Project: Secure Access Control System with Keypad and LCD

This project implements a secure access control system using an STM32F446RE microcontroller. 
The system features a 4-digit password-based authentication mechanism, a matrix keypad for user input,
and an LCD display for user feedback. Access is controlled through an I/O expander and password validation.

> [!WARNING]
> This project is a demonstration of embedded systems integration, combining SPI communication,
> I2C display control, and finite state machine (FSM) design patterns for security applications.

## Characteristics

The project follows a layered architecture with clear separation of concerns:

1. **Hardware Abstraction Layer (ARCH/Port)**: platform-specific I2C, SPI, GPIO, and delay functions that abstract STM32 HAL operations.
2. **LCD Display API** (`API_lcd`): manages a 16x4 character display via I2C communication using an I/O expander.
3. **Keypad Controller API** (`API_mcp23s17`): handles the MCP23S17 SPI-based I/O expander for matrix keypad scanning.
4. **Delay Utility API** (`API_delay`): provides non-blocking delay mechanisms for timing and debouncing.
5. **System Application** (`APP_system`): implements the main FSM-based access control logic with password configuration and validation.

## Code Structure

The project follows a **layered architecture** with clear separation between hardware abstraction and application logic:

```
┌─────────────────────────────────────┐
│  Application Layer (APP)            │
│  - APP_system: FSM & Access Control │
└────────────┬────────────────────────┘
             │
┌────────────┴────────────────────────────────────────────┐
│  API Layer (Drivers/API - Modularized)                  │
│  ┌──────────────────┬────────────────┬─────────────┐    │
│  │  API_lcd/        │  API_mcp23s17/ │ API_delay/  │    │
│  │  - LCD Display   │  - Keypad      │ - Timers    │    │
│  │  - I2C comm      |  - SPI comm    | - Non-block |    |
│  |  - port HAL      │  - port HAL    │ - Port-Hal  |    |
│  └──────────────────┴────────────────┴─────────────┘    │
└────────────┬────────────────────────────────────────────┘
```

### File Organization:

**API Layer (Modularized):**
- `Drivers/API/API_lcd/Inc/API_lcd.h` / `API_lcd/Src/API_lcd.c` — LCD display initialization and character/string output via I2C.
- `Drivers/API/API_mcp23s17/Inc/API_mcp23s17.h` / `API_mcp23s17/Src/API_mcp23s17.c` — MCP23S17 I/O expander initialization and keypad scanning via SPI.
- `Drivers/API/API_delay/Inc/API_delay.h` / `API_delay/Src/API_delay.c` — Non-blocking delay implementation for timing-critical operations.

**Application Layer:**
- `Drivers/APP/APP_system.h` / `APP_system.c` — main application logic with FSM for secure access control.

**Core/HAL:**
- `Core/Src/main.c` / `Core/Inc/main.h` — HAL setup and application entry point.


## Architecture: Hardware Abstraction Layer (ARCH/Port)

The project uses a **dual-layer abstraction strategy**:

1. **Modularized Port Layer** (API-specific): Each API folder contains its own `port_*.c` file with hardware-specific implementations
   - `Drivers/API/API_lcd/Src/port_lcd.c` — I2C peripheral abstraction
   - `Drivers/API/API_mcp23s17/Src/port_mcp23s17.c` — SPI and GPIO abstraction
   - `Drivers/API/API_delay/Src/port_delay.c` — SysTick timer abstraction

This modularized approach provides:

- **Modularity**: Each API is self-contained with its own port layer, simplifying maintenance and updates.
- **Portability**: Hardware-specific code is isolated per API, making it easy to port individual APIs to different microcontrollers.
- **Maintainability**: Port functions are grouped logically with their corresponding API layer.
- **Testability**: Each port layer can be independently tested or mocked without affecting other APIs.
- **Decoupling**: Application and API layers depend on stable port interfaces, not directly on STM32 HAL.


## API: LCD Display (API_lcd)

Purpose: manage a 16x4 character LCD display and I2C communication for user interface output.
This API builds on top of modularized I2C abstraction via `port_lcd.c`.

**Hardware Configuration:**
- **Display**: 16 columns × 4 rows HD44780 LCD controller
- **Communication**: I2C1 peripheral at **100 kHz** clock speed
- **I/O Expander**: PCF8574 I2C-based 8-bit expander (address mode: 7-bit)
- **Duty Cycle**: I2C_DUTYCYCLE_2 for standard mode operation
- **Addressing**: 7-bit addressing mode, no general call or dual address support

**Port Layer Functions** (`API_lcd/Src/port_lcd.c`):

```c
uint8_t port_i2c_init(void);
uint8_t port_i2c_master_transmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
uint8_t port_lcd_delay_ms(uint32_t ms);
```

**API Functions**:

```c
LCD_RET lcd_init(void);           /* Initialize LCD and I2C */
LCD_RET lcd_send_data(uint8_t data);  /* Send single character */
LCD_RET lcd_send_string(char *str);   /* Send string to display */
LCD_RET lcd_put_cur(int row, int col); /* Set cursor position */
LCD_RET lcd_clear(void);          /* Clear display */
```

**Error Codes**:
```c
LCD_OK              /* Successful operation */
LCD_ERR_INIT        /* I2C initialization failure */
LCD_ERR_INVALID_PARAMS  /* NULL pointer or invalid data */
LCD_ERR_UNKNOWN     /* HAL I2C communication error */
```

**Behavior Notes**:

- `lcd_init` initializes I2C1 at 100 kHz and configures the HD44780 in 4-bit mode with proper timing delays.
- Parameter validation: checks for NULL pointers and zero-length data before transmission.
- `lcd_send_string` sends null-terminated strings with proper 4-bit mode enable sequencing.
- `lcd_put_cur` uses HD44780 DDRAM addressing scheme for cursor positioning.
- All I2C operations include timing delays via `port_lcd_delay_ms()` to ensure proper signal propagation and I2C bus compliance.


## API: Keypad Controller (API_mcp23s17)

Purpose: manage the MCP23S17 I/O expander and scan a matrix keypad for user input via SPI bus.
This API builds on top of modularized SPI and GPIO abstractions via `port_mcp23s17.c`.

**Hardware Configuration:**
- **I/O Expander**: MCP23S17 16-bit expander communicates via SPI1 peripheral
- **Communication**: SPI Master mode, 8-bit data size
  - **Clock Polarity**: SPI_POLARITY_LOW (CPOL = 0)
  - **Clock Phase**: SPI_PHASE_1EDGE (CPHA = 0)
  - **Baud Rate**: SPI_BAUDRATEPRESCALER_16 (derived from SPI1 input clock)
  - **Data Transfer**: MSB first, software NSS (Chip Select) control
- **Chip Select Pin**: GPIO Pin 6 on Port B (GPIOB), active low
- **GPIO Ports Enabled**: GPIOC, GPIOH, GPIOA, GPIOB

**Port Layer Functions** (`API_mcp23s17/Src/port_mcp23s17.c`):

```c
uint8_t port_spi_init(void);
uint8_t port_gpio_init(void);
uint8_t port_gpio_set_cs_pin(void);    /* Set CS high */
uint8_t port_gpio_reset_cs_pin(void);  /* Set CS low */
uint8_t port_spi_master_transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout);
uint8_t port_spi_master_receive(uint8_t *pData, uint16_t Size, uint32_t Timeout);
uint8_t port_mcp_delay_ms(uint32_t ms);
```

**API Functions**:

```c
MCP_RET mcp_init(void);           /* Initialize MCP23S17 and SPI */
char mcp_scan_keypad(void);       /* Scan matrix and return pressed key */
```

**Error Codes**:
```c
MCP_OK              /* Successful operation */
MCP_ERR_INIT        /* SPI initialization failure */
MCP_ERR_INVALID_PARAMS  /* NULL pointer or invalid data size */
MCP_ERR_NULL_POINTER    /* Null pointer error */
MCP_ERR_UNKNOWN     /* HAL SPI communication error */
MCP_ERR_SPI_COMM    /* SPI communication error */
```

**Behavior Notes**:

- `mcp_init` configures SPI1 in master mode with proper GPIO and CS pin initialization.
- GPIO initialization enables all ports (C, H, A, B) to ensure proper STM32 clock gating.
- CS pin (PB6) is managed via `port_gpio_set_cs_pin()` (HIGH) and `port_gpio_reset_cs_pin()` (LOW) for proper SPI protocol.
- Parameter validation: checks for NULL pointers and zero-length data before SPI transmission/reception.
- `mcp_scan_keypad` performs row-by-row matrix scanning using SPI transmit/receive with debounce delays.
- Returns ASCII character for pressed key or 0 if no key is pressed.
- All SPI operations include timing delays via `port_mcp_delay_ms()` for debouncing and MCP23S17 response timing.


## API: Delay Utility (API_delay)

Purpose: provide non-blocking delay mechanisms using the port layer's SysTick timer abstraction
for embedded timing requirements.

**Hardware Configuration:**
- **Timer Source**: SysTick (ARM Cortex-M4 system timer)
- **Time Base**: Millisecond resolution via HAL_GetTick()
- **Implementation**: HAL-based blocking and non-blocking delay mechanisms

**Port Layer Functions** (`API_delay/Src/port_delay.c`):

```c
uint8_t port_delay_ms(uint32_t ms);  /* Blocking delay (HAL_Delay wrapper) */
uint32_t port_get_tick(void);         /* Get current SysTick counter (HAL_GetTick wrapper) */
```

**API Functions**:

```c
void delayInit(delay_t *delay, tick_t duration);  /* Initialize delay struct */
bool_t delayRead(delay_t *delay);                  /* Check if delay elapsed (non-blocking) */
void delayWrite(delay_t *delay, tick_t duration);  /* Update delay duration */
```

**Data Structures**:

```c
typedef struct {
    tick_t startTime;  /* Timestamp when delay started */
    tick_t duration;   /* Duration of delay in milliseconds */
    bool_t running;    /* Flag indicating if delay is active */
} delay_t;
```

**Error Codes**:
```c
DELAY_OK              /* Successful operation */
DELAY_ERR_INIT        /* Initialization error */
DELAY_ERR_INVALID_PARAMS  /* Invalid parameters */
DELAY_ERR_NULL_POINTER    /* Null pointer error */
DELAY_ERR_UNKNOWN     /* Unknown error */
```

**Behavior Notes**:

- `port_delay_ms()` is a blocking delay wrapper around STM32 HAL_Delay() for simple timing.
- `port_get_tick()` returns current SysTick counter value in milliseconds via HAL_GetTick().
- `delayInit()` initializes a delay_t structure with start time (current tick) and duration.
- `delayRead()` performs **non-blocking** polling: compares current tick against (startTime + duration).
- `delayWrite()` updates delay duration for reusable delay structures.
- Enables concurrent operation of multiple independent delay timers without blocking the main application.
- Port layer abstracts hardware-specific SysTick implementation, allowing easy porting to other STM32 variants.


## Application System (APP_system)

Purpose: implement the main finite state machine (FSM) for secure access control and password authentication.

Key definitions:

- `MAX_SIZE_PASSWORD` (4) — password length in digits.
- `NO_ACTION_INIT_TICK` (10000) — debounce/idle timeout in milliseconds.
- System states: `SYSTEM_IDLE`, `SYSTEM_VALID_CONFIG`, `SYSTEM_SET_PASSWORD`, 
  `SYSTEM_ENTER_PASSWORD`, `SYSTEM_SAVE_PASSWORD`, `SYSTEM_ACCESS`

Main functions:

```c
SYSTEM_RET system_init(void);
SYSTEM_RET system_fsm_state_update(void);
```

Behavior notes:

- `system_init` initializes all peripherals: GPIO, SPI (for MCP23S17), I2C (for LCD), and system delays.
- `system_fsm_state_update` is the main FSM driver and must be called frequently from the main loop.
- On startup, the system enters `SYSTEM_IDLE` and displays a welcome message.
- First-time setup requires the user to configure a 4-digit password (each digit entered via keypad).
- Subsequent logins require entering the stored password to transition to the `SYSTEM_ACCESS` state.
- Failed password attempts are tracked; repeated failures trigger a lockout timeout.


## System States and User Interaction

The secure access control system operates through the following states:

1. **SYSTEM_IDLE**: Welcome/idle state. Displays system information and waits for keypad input.
2. **SYSTEM_VALID_CONFIG**: Checks if a password has been configured; transitions to password entry or configuration.
3. **SYSTEM_SET_PASSWORD**: Accepts 4 keypad digits to configure the initial system password.
4. **SYSTEM_ENTER_PASSWORD**: Prompts user to enter the password for authentication.
5. **SYSTEM_SAVE_PASSWORD**: Saves the newly configured password to system state.
6. **SYSTEM_ACCESS**: Access granted state; displays success message and resets after timeout.

The FSM handles debouncing, display updates, and resource cleanup between states to ensure reliable operation.

## Hardware Configuration

- **Microcontroller**: STM32F446RE
- **Keypad**: 4×4 matrix keypad connected via MCP23S17 I/O expander (SPI interface)
- **LCD Display**: 16x4 character display with PCF8574 I2C I/O expander
- **Communication Interfaces**:
  - SPI: MCP23S17 I/O expander (keypad scanning)
  - I2C: LCD display control
  - GPIO: System initialization and I/O configuration
- **System Clock**: HSI oscillator with PLL configuration (clock output: 84 MHz)
