#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include "debug.h"
#include "timebase.h"


void dbg_printf(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	vprintf(fmt, va);
	va_end(va);
}

void dbg_va_base(const char *fmt, const char *tag, va_list va)
{
	ms_time_t now = ms_now();
	uint32_t secs = now / 1000;
	uint32_t ms = now % 1000;

	printf("%4"PRIu32".%03"PRIu32" ", secs, ms);

	dbg_raw(tag);

	vprintf(fmt, va);
	dbg_raw(DEBUG_EOL);
}

/** Print a log message with a DEBUG tag and newline */
void dbg(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	dbg_va_base(fmt, DEBUG_TAG_BASE, va);
	va_end(va);
}


/** Print a log message with an INFO tag and newline */
void info(const char *fmt, ...)
{
	v100_attr(FMT_WHITE);

	va_list va;
	va_start(va, fmt);
	dbg_va_base(fmt, DEBUG_TAG_INFO, va);
	va_end(va);

	v100_attr(FMT_RESET);
}



/** Print a log message with an INFO tag and newline */
void banner(const char *fmt, ...)
{
	v100_attr(FMT_GREEN, FMT_BRIGHT);

	va_list va;
	va_start(va, fmt);
	dbg_va_base(fmt, DEBUG_TAG_INFO, va);
	va_end(va);

	v100_attr(FMT_RESET);
}


/** Print a log message with a warning tag and newline */
void warn(const char *fmt, ...)
{
	v100_attr(FMT_YELLOW, FMT_BRIGHT);

	va_list va;
	va_start(va, fmt);
	dbg_va_base(fmt, DEBUG_TAG_WARN, va);
	va_end(va);

	v100_attr(FMT_RESET);
}


/** Print a log message with an ERROR tag and newline */
void error(const char *fmt, ...)
{
	v100_attr(FMT_RED, FMT_BRIGHT);

	va_list va;
	va_start(va, fmt);
	dbg_va_base(fmt, DEBUG_TAG_ERROR, va);
	va_end(va);

	v100_attr(FMT_RESET);
}


void v100_attr_(uint8_t count, ...)
{
	va_list va;
	va_start(va, count);

	putchar(27);
	putchar('[');

	for (int i = 0; i < count; i++) {
		int attr = va_arg(va, int);

		// comma
		if (i > 0) putchar(';');

		// number
		printf("%d", attr);
	}

	putchar('m');

	va_end(va);
}
