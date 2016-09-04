#include <common.h>

#include "debug.h"
#include "timebase.h"
#include "malloc_safe.h"

// Time base
static volatile ms_time_t SystemTime_ms = 0;


typedef struct {
	/** User callback with arg */
	void (*callback)(void *);
	/** Arg for the arg callback */
	void *cb_arg;
	/** Callback interval */
	ms_time_t interval_ms;
	/** Counter, when reaches interval_ms, is cleared and callback is called. */
	ms_time_t countup;
	/** Unique task ID (for cancelling / modification) */
	task_pid_t pid;
	/** Enable flag - disabled tasks still count, but CB is not run */
	bool enabled;
	/** Marks that the task is due to be run */
	bool enqueue;
} periodic_task_t;


typedef struct {
	/** User callback with arg */
	void (*callback)(void *);
	/** Arg for the arg callback */
	void *cb_arg;
	/** Counter, when reaches 0ms, callback is called and the task is removed */
	ms_time_t countdown_ms;
	/** Unique task ID (for cancelling / modification) */
	task_pid_t pid;
	/** Whether this task is long and needs posting on the queue */
	bool enqueue;
} future_task_t;


static size_t periodic_slot_count = 0;
static size_t future_slot_count = 0;

static periodic_task_t *periodic_tasks;
static future_task_t *future_tasks;


/** Init timebase */
void timebase_init(size_t periodic, size_t future)
{
	periodic_slot_count = periodic;
	future_slot_count = future;

	periodic_tasks = calloc_s(periodic, sizeof(periodic_task_t));
	future_tasks = calloc_s(future, sizeof(future_task_t));
}


static task_pid_t next_task_pid = 1; // 0 (PID_NONE) is reserved


/** Get a valid free PID for a new task. */
static task_pid_t make_pid(void)
{
	task_pid_t pid = next_task_pid++;

	// make sure no task is given PID 0
	if (next_task_pid == PID_NONE) {
		next_task_pid++;
	}

	return pid;
}


/** Take an empty periodic task slot and populate the basics. */
static periodic_task_t* claim_periodic_task_slot(ms_time_t interval, bool enqueue)
{
	for (size_t i = 0; i < periodic_slot_count; i++) {
		periodic_task_t *task = &periodic_tasks[i];
		if (task->pid != PID_NONE) continue; // task is used

		task->countup = 0;
		task->interval_ms = interval - 1;
		task->enqueue = enqueue;
		task->pid = make_pid();
		task->enabled = true;
		return task;
	}

	error("Periodic task table full.");

	return NULL;
}


/** Take an empty future task slot and populate the basics. */
static future_task_t* claim_future_task_slot(ms_time_t delay, bool enqueue)
{
	for (size_t i = 0; i < future_slot_count; i++) {
		future_task_t *task = &future_tasks[i];
		if (task->pid != PID_NONE) continue; // task is used

		task->countdown_ms = delay;
		task->enqueue = enqueue;
		task->pid = make_pid();
		return task;
	}

	//error("Future task table full.");

	return NULL;
}

/** Add a periodic task with an arg. */
task_pid_t add_periodic_task(void (*callback)(void*), void* arg, ms_time_t interval, bool enqueue)
{
	periodic_task_t *task = claim_periodic_task_slot(interval, enqueue);

	if (task == NULL) return PID_NONE;

	task->callback = callback;
	task->cb_arg = arg;

	return task->pid;
}


/** Schedule a future task, with uint32_t argument. */
task_pid_t schedule_task(void (*callback)(void*), void *arg, ms_time_t delay, bool enqueue)
{
	future_task_t *task = claim_future_task_slot(delay, enqueue);

	if (task == NULL) return PID_NONE;

	task->callback = callback;
	task->cb_arg = arg;

	return task->pid;
}


/** Enable or disable a periodic task. */
bool enable_periodic_task(task_pid_t pid, bool enable)
{
	if (pid == PID_NONE) return false;

	for (size_t i = 0; i < periodic_slot_count; i++) {
		periodic_task_t *task = &periodic_tasks[i];
		if (task->pid != pid) continue;

		task->enabled = (enable == ENABLE);
		return true;
	}

	return false;
}


/** Check if a periodic task is enabled */
bool is_periodic_task_enabled(task_pid_t pid)
{
	if (pid == PID_NONE) return false;

	for (size_t i = 0; i < periodic_slot_count; i++) {
		periodic_task_t *task = &periodic_tasks[i];
		if (task->pid != pid) continue;

		return task->enabled;
	}

	return false;
}

bool reset_periodic_task(task_pid_t pid)
{
	if (pid == PID_NONE) return false;

	for (size_t i = 0; i < periodic_slot_count; i++) {
		periodic_task_t *task = &periodic_tasks[i];
		if (task->pid != pid) continue;

		task->countup = 0;
		return true;
	}

	return false;
}


bool set_periodic_task_interval(task_pid_t pid, ms_time_t interval)
{
	if (pid == PID_NONE) return false;

	for (size_t i = 0; i < periodic_slot_count; i++) {
		periodic_task_t *task = &periodic_tasks[i];
		if (task->pid != pid) continue;
		task->interval_ms = interval;
		return true;
	}

	return false;
}


/** Remove a periodic task. */
bool remove_periodic_task(task_pid_t pid)
{
	if (pid == PID_NONE) return false;

	for (size_t i = 0; i < periodic_slot_count; i++) {
		periodic_task_t *task = &periodic_tasks[i];
		if (task->pid != pid) continue;

		task->pid = PID_NONE; // mark unused
		return true;
	}

	return false;
}


/** Abort a scheduled task. */
bool abort_scheduled_task(task_pid_t pid)
{
	if (pid == PID_NONE) return false;

	for (size_t i = 0; i < future_slot_count; i++) {
		future_task_t *task = &future_tasks[i];
		if (task->pid != pid) continue;

		task->pid = PID_NONE; // mark unused
		return true;
	}

	return false;
}


/** Run a periodic task */
static void run_periodic_task(periodic_task_t *task)
{
	if (!task->enabled) return;

	if (task->enqueue) {
		// queued task
		// FIXME re-implement queue
		//tq_post(task->callback, task->cb_arg);
	} else {
		// immediate task
		task->callback(task->cb_arg);
	}
}


/** Run a future task */
static void run_future_task(future_task_t *task)
{
	if (task->enqueue) {
		// queued task
		// FIXME re-implement queue
		//tq_post(task->callback, task->cb_arg);
	} else {
		// immediate task
		task->callback(task->cb_arg);
	}
}


/**
 * @brief Millisecond callback, should be run in the SysTick handler.
 */
void timebase_ms_cb(void)
{
	// increment global time
	SystemTime_ms++;

	// run periodic tasks
	for (size_t i = 0; i < periodic_slot_count; i++) {
		periodic_task_t *task = &periodic_tasks[i];
		if (task->pid == PID_NONE) continue; // unused

		if (task->countup++ >= task->interval_ms) {
			// run if enabled
			run_periodic_task(task);
			// restart counter
			task->countup = 0;
		}
	}

	// run planned future tasks
	for (size_t i = 0; i < future_slot_count; i++) {
		future_task_t *task = &future_tasks[i];
		if (task->pid == PID_NONE) continue; // unused

		if (task->countdown_ms-- == 0) {
			// run
			run_future_task(task);
			// release the slot
			task->pid = PID_NONE;
		}
	}
}



/** Seconds delay */
void delay_s(uint32_t s)
{
	while (s-- != 0) {
		delay_ms(1000);
	}
}


/** Delay N ms */
void delay_ms(ms_time_t ms)
{
	ms_time_t start = SystemTime_ms;
	while ((SystemTime_ms - start) < ms); // overrun solved by unsigned arithmetic
}


/** Get milliseconds elapsed since start timestamp */
ms_time_t ms_elapsed(ms_time_t start)
{
	return SystemTime_ms - start;
}


/** Get current timestamp. */
ms_time_t ms_now(void)
{
	return SystemTime_ms;
}


/** Helper for looping with periodic branches */
bool ms_loop_elapsed(ms_time_t *start, ms_time_t duration)
{
	if (SystemTime_ms - *start >= duration) {
		*start = SystemTime_ms;
		return true;
	}

	return false;
}
