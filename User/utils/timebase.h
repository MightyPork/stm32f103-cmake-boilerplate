#ifndef MPORK_TIMEBASE_H
#define MPORK_TIMEBASE_H

/**
 * To use the Timebase functionality,
 * set up SysTick to 1 kHz and call
 * timebase_ms_cb() in the IRQ.
 *
 * If you plan to use pendable future tasks,
 * also make sure you call run_pending_tasks()
 * in your main loop.
 *
 * This is not needed for non-pendable tasks.
 */

#include <common.h>


/** Task PID. */
typedef uint32_t task_pid_t;

/** Time value in ms */
typedef uint32_t ms_time_t;

// PID value that can be used to indicate no task
#define PID_NONE 0

/** Loop until timeout - use in place of while() or for(). break and continue work too! */
#define until_timeout(to_ms) for(uint32_t _utmeo = ms_now(); ms_elapsed(_utmeo) < (to_ms);)

/** Retry a call until a timeout. Variable 'suc' is set to the return value. Must be defined. */
#define retry_TO(to_ms, call)  \
    until_timeout(to_ms) { \
        suc = call; \
        if (suc) break; \
    }

/** Init timebase, allocate slots for tasks. */
void timebase_init(size_t periodic_count, size_t future_count);

/** Must be called every 1 ms */
void timebase_ms_cb(void);


// --- Periodic -----------------------------------------------


/**
 * @brief Add a periodic task with an arg.
 * @param callback : task callback
 * @param arg      : callback argument
 * @param interval : task interval (ms)
 * @param enqueue  : put on the task queue when due
 * @return task PID
 */
task_pid_t add_periodic_task(void (*callback)(void *), void *arg, ms_time_t interval, bool enqueue);


/** Destroy a periodic task. */
bool remove_periodic_task(task_pid_t pid);

/** Enable or disable a periodic task. Returns true on success. */
bool enable_periodic_task(task_pid_t pid, bool cmd);

/** Check if a periodic task exists and is enabled. */
bool is_periodic_task_enabled(task_pid_t pid);

/** Reset timer for a task */
bool reset_periodic_task(task_pid_t pid);

/** Set inteval */
bool set_periodic_task_interval(task_pid_t pid, ms_time_t interval);


// --- Future -------------------------------------------------


/**
 * @brief Schedule a future task, with uint32_t argument.
 * @param callback : task callback
 * @param arg      : callback argument
 * @param delay    : task delay (ms)
 * @param enqueue  : put on the task queue when due
 * @return task PID
 */
task_pid_t schedule_task(void (*callback_arg)(void *), void *arg, ms_time_t delay, bool enqueue);


/** Abort a scheduled task. */
bool abort_scheduled_task(task_pid_t pid);


// --- Waiting functions --------------------------------------

/** Get milliseconds elapsed since start timestamp */
ms_time_t ms_elapsed(ms_time_t start);


/** Get current timestamp. */
ms_time_t ms_now(void);


/** Delay using SysTick */
void delay_ms(ms_time_t ms);


/** Delay N seconds */
void delay_s(uint32_t s);


inline __attribute__((always_inline))
void delay_cycles(uint32_t n)
{
	uint32_t l = n >> 2;

	__asm volatile(
	"0: mov r0,r0;"
			"subs %[count], #1;"
			"bne 0b;"
	: [count] "+r"(l)
	);
}


inline __attribute__((always_inline))
void delay_ns(uint32_t ns)
{
	delay_cycles(ns / 24);
}


/**
 * @brief Microsecond delay.
 * @param us
 */
inline __attribute__((always_inline))
void delay_us(uint32_t us)
{
	delay_ns(us * 1150);
}


/**
 * @brief Check if time since `start` elapsed.
 *
 * If so, sets the *start variable to the current time.
 *
 * Example:
 *
 *   ms_time_t s = ms_now();
 *
 *   while(1) {
 *     if (ms_loop_elapsed(&s, 100)) {
 *       // this is called every 100 ms
 *     }
 *     // ... rest of the loop ...
 *   }
 *
 * @param start     start time variable
 * @param duration  delay length
 * @return delay elapsed; start was updated.
 */
bool ms_loop_elapsed(ms_time_t *start, ms_time_t duration);

#endif //MPORK_TIMEBASE_H
