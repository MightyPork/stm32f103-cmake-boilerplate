# STM32F103 CLion / CubeMX boilerplate

Update the init files using STM32CubeMX to fit your project needs.

This is a Cmake / CLion project.

## Basics

- The default setup is USART1, 115200 baud, at PA9 (Tx), PA10 (Rx); PC13 as output (built-in LED).
- The program branches from `main.c` into `User/user_main.c` where the actual application code starts.
- Initialization of the application code (libs) is done in `User/init.c`. Exception handlers and such are handled in `User/handlers.c`.
- Use the included Debounce module for button inputs, Timebase for periodic and future tasks.
- Functions from `User/utils/debug.h` print messages to USART1, and work like `printf()`. Regular `printf()` works as well.
- Use `malloc_s()` and `calloc_s()` if you want error message on malloc fail instead of a hard fault / memory corruption.
- Flash using `./flash.sh`. Hold the reset button on the board, and release it right after issuing the flash command.
