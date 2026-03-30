# Practice 4: Software Debounce FSM

This C program uses a finite-state machine to detect the state of the built-in button on the STM 32 NUCLEO F446RE. This allows the blinking period of an LED to be adjusted using non-blocking delays. Additionally, anti-bounce software is built in to allow for greater accuracy in detecting button states.

> [!WARNING]
> This project is related to CESE Practice 4 and it's based on Practice 3. The PDF file containing the instructions for the practice will be attached to this folder.


## Characteristics

The project is divided into three main development areas.

1. FSM: A finite-state machine is used to identify the state of the button on the board when it is pressed or released, or when it is transitioning between these states.
2. Debounce: The debounce software is built in. After 40 milliseconds have elapsed, the software checks to see if the state in the finite-state machine should change.
3. Non-Blocking Delay: Non-blocking delays are implemented to check the button's transition state and perform other necessary validations

# Code Structure

## API Delay

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

## Delay Debounce

### Debounce init
#### Init debounce parameters. First, the finite-state machine is set to the "button up" state and a flag is set to indicate that the debouncing process has begun. Otherwise, the debouncing process and the finite-state machine will not function.
```C
void debounceFSM_init(void);
``` 

### Debounce FSM update
#### This is where state identification and transitions occur. This function implements the finite-state machine for the button. The states it handles include up, falling, down, and raising. Notably, the function receives a Boolean parameter indicating whether the button transitions to the down state from falling or the up state from raising after a non-blocking delay of 40 ms. This was done to avoid creating dependencies between components.
```C
void debounceFSM_update(bool_t debounce_time);
``` 
### Read key

#### This function returns a Boolean value if the button has been pressed; if so, the variable returns `true` and then switches to `false` until the button is pressed again.
```C
bool_t readKey(void);
``` 

## API Common
API_COMMON was created to promote modularity and independence among components. It includes the necessary headers for integrating unsigned and boolean variable types, as well as the required HAL definitions.
### Dependencies
For custom data types to compile correctly, the API_common includes the following standard C and HAL libraries:
* `<stdint.h>`: For the `uint32_t` type (renamed to `tick_t`).
* `<stdbool.h>`: For the `bool` type (renamed to `bool_t`).
* `stm32f4xx_hal.h`: For the necessary HAL functions definitions.


## Author
### Marcos Nuñez