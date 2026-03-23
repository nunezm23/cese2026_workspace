# Practice 3: Modularization & Non-Blocking Delays

This project implements a C-written software module to manage non-blocking delays. Unlike traditional delay functions, such as delay() or HAL_Delay(), this implementation allows the microcontroller to execute other tasks while waiting for the specified time to elapse. In addition, the concept of modularization is built into the code, allowing the driver to be encapsulated in a folder that can later be called by any other file.

> [!WARNING]
> This project is related to CESE Practice 3 and it's based on Practice 2. The PDF file containing the instructions for the practice will be attached to this folder.

## Characteristics

The project is divided into three main development areas.

1. Delay API: Modularization of the delay code into an API folder to manage all aspects related to delay, including structures, functions, and definitions. 
2. Automated Blinking Pattern: Use the API to make an LED on the development board blink at a constant interval with a 50% duty cycle
3. Tick running check: Add validation to a new function to ensure that the delay is not currently running before a new value is entered.

## Code Structure

### Dependencies
For custom data types to compile correctly, the API_delay includes the following standard C libraries:
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

### Delay running
#### Given an instance of the `delay_t` structure, It's verified that the running flag is not set before writing a new value for the non-blocking delay.
```C
bool_t delayIsRunning(delay_t * delay);
``` 

## Functionality

### The program works as follows:
#### The LED begins to blink with an initial duration of 2 seconds for 5 cycles at a 50% duty cycle, and then continues to blink under the same conditions but with the following durations:

```C
const uint32_t period_ms[] = {500, 100, 100, 1000};
``` 

## Author

### Marcos Nuñez