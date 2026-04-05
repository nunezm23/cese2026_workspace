# Practice 5: UART Command Parser and Simple Serial Menu

This project implements a small UART-based command parser and a set of serial
commands to interact with the on-board LED (LD2) and query UART settings.
The implementation is split into a lightweight `API_uart` for basic UART I/O and
an FSM-based `API_cmdparser` that receives characters over UART, tokenizes
commands and executes simple actions.

> [!WARNING]
> This project is related to CESE Practice 5 and it's based on older practices . The PDF file containing the instructions for the practice will be attached to this folder.

## Characteristics

The project is organized around three main areas:

1. UART API (`API_uart`): initializes USART2 and provides transmit/receive helpers.
2. Command parser FSM (`API_cmdparser`): a small FSM that accumulates characters,
	 recognizes complete lines, tokenizes the input and executes commands.
3. Simple command menu: Commands to turn the LED on/off/toggle, get LED status
	 and query / (partially) change baudrate.

## Code Structure

The important modules are under `Drivers/API` and `Core`:

- `Drivers/API/API_uart.h` / `API_uart.c` — UART initialization and helpers.
- `Drivers/API/API_cmdparser.h` / `API_cmdparser.c` — command parser FSM, menu and actions.
- `Core/Src` / `Core/Inc` — application entry (`main.c`) and HAL setup.


## API: UART (API_uart)

Purpose: simplify UART usage (USART2) for sending and receiving strings.

Key definitions:

- `MAX_UART_SIZE` (256) — maximum buffer size for UART operations.
- `MIN_UART_SIZE` (1)

Relevant functions:

```c
bool_t uartInit(void);
void uartSendString(uint8_t * pstring);
void uartSendStringSize(uint8_t * pstring, uint16_t pstring_size);
uint8_t uartReceiveStringSize(uint8_t * pstring, uint16_t pstring_size);
void uartEchoTest(void);
void uartGetBaudrate(uint32_t *baudrate);
```

Behavior notes:

- `uartInit` configures USART2 with 115200 8N1 and prints the configuration over
	UART during initialization.
- `uartReceiveStringSize` returns HAL status (e.g. `HAL_OK`) or an error/invalid value
	when arguments are out of range.


## API: Command Parser (API_cmdparser)

Purpose: read incoming UART bytes, accumulate a command line, process it using
an FSM and execute predefined commands from a menu.

Key definitions:

- `CMD_MAX_LINE` (64) — maximum command length
- `CMD_MAX_TOKENS` (3) — maximum tokens per command (not all commands use tokens)
- `cmd_status_t` — status enum (CMD_OK, CMD_ERR_OVERFLOW, CMD_ERR_SYNTAX, ...)
- `cmd_fsm_state_t` — FSM states (CMD_IDLE, CMD_RECEIVING, CMD_PROCESS, CMD_EXEC, CMD_ERROR)

Main functions:

```c
void cmdParserInit(void);
void cmdPoll(void);       // call frequently from main loop to process incoming bytes
void cmdPrintHelp(void);
```

Behavior notes:

- `cmdParserInit` initializes the UART (calls `uartInit`) and prints a hint message.
- `cmdPoll` is the FSM driver: it reads a byte from UART, updates the FSM and when
	a full line is received it tokenizes and processes the command.
- On errors (invalid characters, overflow) the FSM enters `CMD_ERROR` and prints
	a corresponding message before returning to idle.


## Available commands (serial menu)

The command menu supports the following textual commands (case sensitive as
implemented):

- HELP — prints the available commands and usage hint
- LED ON — set LD2 on
- LED OFF — set LD2 off
- LED TOGGLE — toggle LD2
- STATUS — print the current LED state
- BAUD? — query the current UART baudrate
- BAUD=... — change baudrate (placeholder — not fully implemented)

Example interaction:

```
> HELP
1. LED ON
2. LED OFF
...
HINT: TYPE COMMAND
```


## Sample session (UART)

Below is a short example UART transcript showing typical interaction with the
command parser. Lines prefixed with ">" represent user input (type + Enter), the
other lines are text sent by the MCU over UART.

```
# (device boot / init)
HINT: TYPE HELP FOR SHOW COMMANDS AVAILABLES:

> HELP
--- AVAILABLE COMMANDS ---
1. LED ON

2. LED OFF

3. LED TOGGLE

4. STATUS

5. BAUD?

6. BAUD=

HINT: TYPE COMMAND

> LED ON

COMMAND DETECTED: LED ON
COMMAND PROCESSED

> STATUS

COMMAND DETECTED: LED STATE
STATE LED: ON
COMMAND PROCESSED

> BAUD?

COMMAND DETECTED: GET CURRENT BAUDRATE
BAUDRATE: 115200[bps]
COMMAND PROCESSED

> BAUD=9600

COMMAND NOT YET IMPLEMENTED: CHANGE BAUDRATE

```

## Implementation details

- The parser uses a `buffer_cmd` and `buffer_cmd_size` to accumulate incoming
	bytes until a newline/carriage-return is received, then `cmdProcessLine` tries
	to match the buffer with known menu entries.
- When a command is recognized, `cmdProcessAction` runs the corresponding action
	(e.g., toggling the LED using `HAL_GPIO_TogglePin`) and sends textual feedback
	over UART.
- Error reporting functions print clear error messages such as `ERROR: line too long`
	`ERROR: unknown command` or `ERROR: bad arguments`.

## Author
Marcos Nuñez