# Módulo de Retardos No Bloqueantes (Non-Blocking Delays)

This project implements a C-written software module to manage non-blocking delays. Unlike traditional delay functions, such as delay() or HAL_Delay(), this implementation allows the microcontroller to execute other tasks while waiting for the specified time to elapse.

> [!WARNING]
> This project is related to CESE Practice 2. The PDF file containing the instructions for the practice will be attached to this folder.

## Characteristics

The project is divided into three main development areas.

1. Delay API: Definition of the delay_t structure and the control functions (delayInit, delayRead, delayWrite) in main.h and their respective implementation in main.c.
2. Basic Blinking: Use the API to make an LED on the development board blink at a constant interval (100 ms on, 100 ms off).
3. Automated Blinking Pattern: Implementation of an automated blinking sequence that cycles through different frequencies (1 s, 200 ms, and 100 ms) at a 50% duty cycle using an array.

## Code Structure

### Dependencies
For custom data types to compile correctly, the project includes the following standard C libraries:
* `<stdint.h>`: For the `uint32_t` type (renamed to `tick_t`).
* `<stdbool.h>`: For the `bool` type (renamed to `bool_t`).

### `delay_t` structure
It stores the start time, duration and the running flag for each instance.
```c
typedef struct {
   tick_t startTime;
   tick_t duration;
   bool_t running;
} delay_t;
``` 

## Relevant functions

### Delay init
#### This function initializes the `delay_t` structure pointer with the `start_time` parameter and sets the `running` flag to start the countdown.

```C
void delayInit( delay_t * delay, tick_t duration );
```

### Delay read
#### Given the instance of the `delay_t` structure, a validation is performed to determine whether the time limit has been met; if so, the counter is reset and the return value is true; otherwise, it is false.
```C
bool_t delayRead( delay_t * delay );
```

### Delay write
#### Given an instance of the `delay_t` structure, the duration can be overwritten.
```C
void delayWrite( delay_t * delay, tick_t duration );
``` 