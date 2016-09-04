//
// Created by MightyPork on 2016/9/2
//

#include <common.h>
#include "utils/timebase.h"
#include "utils/debug.h"
#include "user_main.h"
#include "init.h"

/** Main function, called from MX-generated main.c */
void user_main()
{
	banner("== USER CODE STARTING ==");

	user_init();

	ms_time_t counter1 = 0;
	while (1) {
		if (ms_loop_elapsed(&counter1, 1000)) {
			// Blink
			HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		}
	}
}
