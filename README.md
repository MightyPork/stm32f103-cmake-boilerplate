# STM32F103 CLion CubeMX boilerplate

## Basics

- This is a CMake / CLion project, using `arm-none-eabi-gcc` and `arm-none-eabi-newlib`.
- It's built on Linux. Whether it works on Windows or MacOS is left as an exercise to the reader :stuck_out_tongue:
- Update the init files using **STM32CubeMX** to fit your project needs. Open `bluepill.ioc` in CubeMX, then export 
  to the same folder.
- Since we're using CubeMX, this project is based on HAL. It's not so bad, if you don't look at the source too much.

## Getting Started - Usage

- The default setup is USART1, 115200 baud, at PA9 (Tx), PA10 (Rx); PC13 as output (built-in LED).
- The program branches from `main.c` into `User/user_main.c` where the actual application code starts.
- Initialization of the application code (libs) is done in `User/init.c`. Exception handlers and such are handled in 
  `User/handlers.c`.
- Use the included Debounce module for button inputs, Timebase for periodic and future tasks.
- Functions from `User/utils/debug.h` print messages to USART1, and work like `printf()`. Regular `printf()` works as well.
- Use `malloc_s()` and `calloc_s()` if you want error message on malloc fail instead of a hard fault / memory corruption.
- Flash using `./flash.sh`. Hold the reset button on the board, and release it right after issuing the flash command.
