//
// Created by MightyPork on 2016/9/4
//

#include <common.h>
#include "utils/debounce.h"
#include "init.h"
#include "handlers.h"

static void init_buttons()
{
	debo_init_t debo;
	debo.invert = true;
	debo.debo_time = 50;
	debo.GPIOx = GPIOB; // All buttons are on port B
	debo.callback = ButtonHandler;

	debo.pin = BTN1_Pin;
	debo.cb_payload = 1;
	debo_register_pin(&debo);

	debo.pin = BTN2_Pin;
	debo.cb_payload = 2;
	debo_register_pin(&debo);

	debo.pin = BTN3_Pin;
	debo.cb_payload = 3;
	debo_register_pin(&debo);

	debo.pin = BTN4_Pin;
	debo.cb_payload = 4;
	debo_register_pin(&debo);
}

/** Init the application */
void user_init()
{
	timebase_init(5, 5);
	debounce_init(4);

	init_buttons();
}
