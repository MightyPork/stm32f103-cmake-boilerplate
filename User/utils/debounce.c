#include <common.h>
#include "debounce.h"
#include "timebase.h"
#include "malloc_safe.h"

// ms debounce time

#define DEF_DEBO_TIME 20

typedef struct {
	GPIO_TypeDef *GPIOx;         ///< GPIO base
	uint16_t pin;                ///< bit mask
	bool state;                  ///< current state
	bool invert;                 ///< invert pin
	debo_id_t id;                ///< pin ID
	uint32_t cb_payload;         ///< payload passed to the callbac
	ms_time_t debo_time;         ///< debouncing time (ms)
	ms_time_t counter_0;         ///< counter for falling edge (ms)
	ms_time_t counter_1;         ///< counter for rising edge (ms)
	void (*callback)(uint32_t, bool);
} debo_slot_t;


/** Number of allocated slots */
static size_t debo_slot_count = 0;

/** Slots array */
static debo_slot_t *debo_slots;

/** Next free pin ID for make_id() */
static debo_id_t next_pin_id = 1;

void debo_periodic_task(void *unused);


/**
 * @brief Get a valid free pin ID for a new entry.
 * @return the ID.
 */
static debo_id_t make_id(void)
{
	debo_id_t id = next_pin_id++;

	// make sure no task is given PID 0
	if (next_pin_id == DEBO_PIN_NONE) {
		next_pin_id++;
	}

	return id;
}


/** Init the debouncer */
void debounce_init(size_t slot_count)
{
	debo_slots = calloc_s(slot_count, sizeof(debo_slot_t));
	debo_slot_count = slot_count;

	add_periodic_task(debo_periodic_task, NULL, 1, false);
}


/** Register a pin */
debo_id_t debo_register_pin(debo_init_t *init)
{
	for (size_t i = 0; i < debo_slot_count; i++) {
		debo_slot_t *slot = &debo_slots[i];

		if (slot->id != DEBO_PIN_NONE) continue; // slot is used

		slot->GPIOx = init->GPIOx;
		slot->pin = init->pin;
		slot->callback = init->callback;
		slot->cb_payload = init->cb_payload;
		slot->invert = init->invert;
		slot->counter_0 = 0;
		slot->counter_1 = 0;
		slot->debo_time = (init->debo_time == 0) ? DEF_DEBO_TIME : init->debo_time;

		bool state = HAL_GPIO_ReadPin(slot->GPIOx, slot->pin);
		if (slot->invert) state = !state;
		slot->state = state;

		slot->id = make_id();

		return slot->id;
	}

	return DEBO_PIN_NONE;
}


/** Callback that must be called every 1 ms */
void debo_periodic_task(void *unused)
{
	UNUSED(unused);

	for (size_t i = 0; i < debo_slot_count; i++) {
		debo_slot_t *slot = &debo_slots[i];
		if (slot->id == DEBO_PIN_NONE) continue; // unused

		bool state = HAL_GPIO_ReadPin(slot->GPIOx, slot->pin);
		if (slot->invert) state = !state;

		if (slot->state != state) {
			if (state == 0) {
				// falling

				if (slot->counter_0++ == slot->debo_time) {
					slot->state = 0;

					if (slot->callback != NULL) {
						slot->callback(slot->cb_payload, false);
					}
				}
			} else {
				// rising

				if (slot->counter_1++ == slot->debo_time) {
					slot->state = 1;

					if (slot->callback != NULL) {
						slot->callback(slot->cb_payload, true);
					}
				}
			}
		} else {
			// reset counters
			slot->counter_0 = 0;
			slot->counter_1 = 0;
		}
	}
}


/**
 * @brief Check if a pin is high
 * @param pin_id : Slot ID
 * @return true if the pin is registered and is HIGH
 */
bool debo_pin_state(debo_id_t pin_id)
{
	if (pin_id == DEBO_PIN_NONE) return false;

	for (size_t i = 0; i < debo_slot_count; i++) {
		debo_slot_t *slot = &debo_slots[i];
		if (slot->id != pin_id) continue;

		return slot->state;
	}

	return false;
}


/**
 * @brief Remove a pin entry from the debouncer.
 * @param pin_id : Slot ID
 * @return true if task found & removed.
 */
bool debo_remove_pin(debo_id_t pin_id)
{
	if (pin_id == DEBO_PIN_NONE) return false;

	for (size_t i = 0; i < debo_slot_count; i++) {
		debo_slot_t *slot = &debo_slots[i];
		if (slot->id != pin_id) continue;

		slot->id = DEBO_PIN_NONE;
		return true;
	}

	return false;
}
