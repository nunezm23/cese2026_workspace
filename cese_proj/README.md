# CESE Project: Secure Access Control System with Keypad and LCD

This project implements a secure access control system using an STM32F446RE microcontroller. 
The system features a 4-digit password-based authentication mechanism, a matrix keypad for user input,
and an LCD display for user feedback. Access is controlled through an I/O expander and password validation.

> [!WARNING]
> This project is an advanced demonstration of embedded systems integration, combining SPI communication,
> I2C display control, and finite state machine (FSM) design patterns for security applications.

## Characteristics

The project is organized around four main functional areas:

1. **LCD Display API** (`API_lcd`): manages a 16x4 character display via I2C communication using an I/O expander.
2. **Keypad Controller API** (`API_mcp23s17`): handles the MCP23S17 SPI-based I/O expander for matrix keypad scanning.
3. **Delay Utility API** (`API_delay`): provides non-blocking delay mechanisms for timing and debouncing.
4. **System Application** (`APP_system`): implements the main FSM-based access control logic with password configuration and validation.

## Code Structure

The important modules are under `Drivers/API`, `Drivers/APP`, and `Core`:

- `Drivers/API/API_lcd.h` / `API_lcd.c` — LCD display initialization and character/string output via I2C.
- `Drivers/API/API_mcp23s17.h` / `API_mcp23s17.c` — MCP23S17 I/O expander initialization and keypad scanning via SPI.
- `Drivers/API/API_delay.h` / `API_delay.c` — Non-blocking delay implementation for timing-critical operations.
- `Drivers/API/API_common.h` — Common definitions and types used across all APIs.
- `Drivers/APP/APP_system.h` / `APP_system.c` — main application logic with FSM for secure access control.
- `Core/Src/main.c` / `Core/Inc/main.h` — HAL setup and application entry point.


## API: LCD Display (API_lcd)

Purpose: manage a 16x4 character LCD display and I2C communication for user interface output.

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

- `lcd_init` initializes the I2C peripheral and configures the HD44780 display controller in 4-bit mode.
- `lcd_send_string` transmits a null-terminated string to the display.
- `lcd_put_cur` sets the cursor position using HD44780 addressing scheme (row, column coordinates).
- All I2C operations include timing delays to ensure proper signal propagation.


## API: Keypad Controller (API_mcp23s17)

Purpose: manage the MCP23S17 I/O expander and scan a matrix keypad for user input via SPI bus.

Key definitions:

- MCP23S17 16-bit I/O expander communicates via SPI.
- Matrix keypad scanning (rows and columns configured as GPIO through the expander).
- Debounce delay to filter spurious key presses.

Main functions:

```c
MCP_RET mcp_init(void);
char mcp_scan_keypad(void);
```

Behavior notes:

- `mcp_init` configures the MCP23S17 chip and initializes SPI communication.
- `mcp_scan_keypad` performs row-by-row scanning to detect pressed keys and returns the ASCII character
  representing the key (or 0 if no key is pressed).
- Debouncing is applied to prevent key bounce errors during scanning.


## API: Delay Utility (API_delay)

Purpose: provide non-blocking delay mechanisms using SysTick timer for embedded timing requirements.

Key definitions:

- `delay_t` — structure to track delay state and remaining time.

Relevant functions:

```c
void delay_init(delay_t *delay, tick_t duration);
bool_t delay_read(delay_t *delay);
void delay_write(delay_t *delay, tick_t duration);
```

Behavior notes:

- Delays are implemented using non-blocking polling via SysTick timer.
- `delay_init` initializes a delay_t structure with a duration in milliseconds.
- `delay_read` checks if the delay period has elapsed (returns true when complete).
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
