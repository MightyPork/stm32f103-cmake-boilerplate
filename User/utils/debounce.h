#ifndef MPORK_DEBOUNCE_H
#define MPORK_DEBOUNCE_H

#include <common.h>
#include "timebase.h"

// Debouncer requires that you set up timebase first.

/** Debounced pin ID - used for state readout */
typedef uint32_t debo_id_t;

/** debo_id_t indicating unused slot */
#define DEBO_PIN_NONE 0


/**
 * @brief Initialize the debouncer.
 *
 * Registers the callback.
 *
 * @param pin_count : number of pin slots to allocate
 */
void debounce_init(size_t pin_count);


typedef struct {
	GPIO_TypeDef *GPIOx;          ///< GPIO base
	uint16_t pin;                 ///< pin mask
	bool invert;                  ///< invert value read from GPIO (button to ground)
	ms_time_t debo_time;          ///< debounce time in ms, 0 = default (20 ms)
	uint32_t cb_payload;          ///< Value passed to the callback func
	void (*callback)(uint32_t, bool); ///< callback
} debo_init_t;


/**
 * @brief Add a pin for debouncing.
 *
 * The pin state will be checked with the configured hysteresis
 * and callbacks will be called when a state change is detected.
 */
debo_id_t debo_register_pin(debo_init_t *init_struct);


/**
 * @brief Check if a pin is high
 * @param pin_id : Slot ID
 * @return true if the pin is registered and is HIGH
 */
bool debo_pin_state(debo_id_t pin_id);


/**
 * @brief Remove a pin entry from the debouncer.
 * @param pin_id : Slot ID
 * @return true if task found & removed.
 */
bool debo_remove_pin(debo_id_t pin_id);

#endif /* MPORK_DEBOUNCE_H */
