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
┌────────────┴────────────────────────┐
│  API Layer (Drivers/API)            │
│  - API_lcd: LCD Display             │
│  - API_mcp23s17: Keypad Scanning    │
│  - API_delay: Non-blocking Timers   │
└────────────┬────────────────────────┘
             │
┌────────────┴────────────────────────┐
│  ARCH/Port Layer (Hardware Abstraction)
│  - port.h / port_stm32f446re.c      │
│  - Abstracts I2C, SPI, GPIO, Timers │
└────────────┬────────────────────────┘
             │
┌────────────┴────────────────────────┐
│  HAL / Hardware Layer               │
│  - STM32 HAL, STM32F4xx drivers     │
└─────────────────────────────────────┘
```

### File Organization:

**Hardware Abstraction (ARCH/Port):**
- `Drivers/ARCH/port.h` — Port interface definitions for platform-independent access to hardware.
- `Drivers/ARCH/Src/port_stm32f446re.c` — STM32F446RE specific implementations of I2C, SPI, GPIO, and timing functions.

**API Layer:**
- `Drivers/API/API_lcd.h` / `API_lcd.c` — LCD display initialization and character/string output via I2C.
- `Drivers/API/API_mcp23s17.h` / `API_mcp23s17.c` — MCP23S17 I/O expander initialization and keypad scanning via SPI.
- `Drivers/API/API_delay.h` / `API_delay.c` — Non-blocking delay implementation for timing-critical operations.
- `Drivers/API/API_common.h` — Common definitions and types used across all APIs.

**Application Layer:**
- `Drivers/APP/APP_system.h` / `APP_system.c` — main application logic with FSM for secure access control.

**Core/HAL:**
- `Core/Src/main.c` / `Core/Inc/main.h` — HAL setup and application entry point.


## Architecture: Hardware Abstraction Layer (ARCH/Port)

### Purpose and Design Philosophy

The ARCH/Port layer serves as a **platform abstraction boundary** between the STM32 HAL and the application APIs. 
This design approach provides:

- **Portability**: APIs depend on the port interface, not directly on HAL. Porting to a different microcontroller requires only updating port implementations.
- **Maintainability**: Hardware-specific code is centralized, making it easier to manage and debug.
- **Testability**: Port functions can be mocked or replaced for testing without modifying application logic.
- **Decoupling**: Application and API layers are isolated from HAL changes and microcontroller specifics.

### Port Functions

The port layer abstracts the following core functionalities:

**I2C Communication:**
```c
uint8_t port_i2c_init(void);
uint8_t port_i2c_master_transmit(uint16_t DevAddress, uint8_t *pData, 
                                  uint16_t Size, uint32_t Timeout);
```

**SPI Communication:**
```c
uint8_t port_spi_init(void);
uint8_t port_spi_master_transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout);
uint8_t port_spi_master_receive(uint8_t *pData, uint16_t Size, uint32_t Timeout);
```

**GPIO Control:**
```c
uint8_t port_spi_cs_high(void);  /* Set SPI chip select high */
uint8_t port_spi_cs_low(void);   /* Set SPI chip select low */
```

**Timing and Delays:**
```c
uint8_t port_delay_ms(uint32_t ms);  /* Blocking delay */
uint32_t port_get_tick(void);         /* Get current system tick in milliseconds */
```

### STM32F446RE Implementation

The file `port_stm32f446re.c` provides the hardware-specific implementations:

- **I2C1**: Configured at 100 kHz for LCD display communication via PCF8574 I/O expander
- **SPI1**: Configured for MCP23S17 I/O expander communication (keypad scanning)
- **GPIO (GPIOB Pin 6)**: SPI chip select line control
- **System Tick**: Used for non-blocking delay and timing operations

All HAL function calls are encapsulated within the port layer, ensuring clean interfaces for upper layers.


## API: LCD Display (API_lcd)

Purpose: manage a 16x4 character LCD display and I2C communication for user interface output.
This API builds on top of the port layer's I2C abstraction.

Key definitions:

- LCD display dimensions: 16 columns × 4 rows
- I2C-based communication using PCF8574 I/O expander
- HD44780 controller compatibility (4-bit mode operation)

Relevant functions:

```c
LCD_RET lcd_init(void);
LCD_RET lcd_send_data(uint8_t data);
LCD_RET lcd_send_string(char *str);
LCD_RET lcd_put_cur(int row, int col);
LCD_RET lcd_clear(void);
```

Behavior notes:

- `lcd_init` initializes the port's I2C interface and configures the HD44780 display controller in 4-bit mode.
- All I2C operations use `port_i2c_master_transmit()` from the port layer, maintaining hardware independence.
- `lcd_send_string` transmits a null-terminated string to the display.
- `lcd_put_cur` sets the cursor position using HD44780 addressing scheme (row, column coordinates).
- All I2C operations include timing delays to ensure proper signal propagation.


## API: Keypad Controller (API_mcp23s17)

Purpose: manage the MCP23S17 I/O expander and scan a matrix keypad for user input via SPI bus.
This API builds on top of the port layer's SPI and GPIO abstractions.

Key definitions:

- MCP23S17 16-bit I/O expander communicates via SPI port layer functions.
- Matrix keypad scanning (rows and columns configured as GPIO through the expander).
- Debounce delay to filter spurious key presses.

Main functions:

```c
MCP_RET mcp_init(void);
char mcp_scan_keypad(void);
```

Behavior notes:

- `mcp_init` configures the MCP23S17 chip using port SPI functions and initializes SPI communication.
- `mcp_scan_keypad` performs row-by-row scanning using `port_spi_master_transmit()` and 
  `port_spi_master_receive()` to detect pressed keys.
- SPI chip select is managed via `port_spi_cs_high()` and `port_spi_cs_low()` port functions.
- Returns the ASCII character representing the key (or 0 if no key is pressed).
- Debouncing is applied to prevent key bounce errors during scanning.


## API: Delay Utility (API_delay)

Purpose: provide non-blocking delay mechanisms using the port layer's SysTick timer abstraction
for embedded timing requirements.

Key definitions:

- `delay_t` — structure to track delay state and remaining time.

Relevant functions:

```c
void delay_init(delay_t *delay, tick_t duration);
bool_t delay_read(delay_t *delay);
void delay_write(delay_t *delay, tick_t duration);
```

Behavior notes:

- Delays are implemented using non-blocking polling via the port's `port_get_tick()` function.
- `delay_init` initializes a delay_t structure with a duration in milliseconds.
- `delay_read` checks if the delay period has elapsed (returns true when complete) using system ticks.
- The port layer abstracts the underlying SysTick mechanism, allowing hardware independence.
- Enables concurrent operation of multiple independent timers.


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
