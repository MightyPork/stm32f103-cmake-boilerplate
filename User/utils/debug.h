#ifndef MPORK_DEBUG_H
#define MPORK_DEBUG_H

#include <common.h>
#include <stdarg.h>

// helper to mark printf functions
#define PRINTF_LIKE __attribute__((format(printf, 1, 2)))

// formatting symbols
#define DEBUG_EOL "\r\n"
#define DEBUG_TAG_WARN  "[W] "
#define DEBUG_TAG_ERROR "[E] "
#define DEBUG_TAG_BASE  "[ ] "
#define DEBUG_TAG_INFO  "[i] "


/** Print a log message with no tag and no newline */
void dbg_printf(const char *fmt, ...) PRINTF_LIKE;

/** Print via va_list */
void dbg_va_base(const char *fmt, const char *tag, va_list va);

/** Print a string to the debug interface (length not limited) */
static inline void dbg_raw(const char *str)
{
	fputs(str, stdout);
}


/** Print a char to the debug interface */
static inline void dbg_raw_c(char c)
{
	putchar((uint8_t) c);
}

/** Print a log message with a "debug" tag and newline */
void dbg(const char *fmt, ...) PRINTF_LIKE;

/** Print a log message with an "info" tag and newline */
void info(const char *fmt, ...) PRINTF_LIKE;

/** Print a log message with a "banner" tag and newline */
void banner(const char *fmt, ...) PRINTF_LIKE;

/** Print a log message with a "warning" tag and newline */
void warn(const char *fmt, ...) PRINTF_LIKE;


/** Print a log message with an "error" tag and newline */
void error(const char *fmt, ...) PRINTF_LIKE;


/** ANSI formatting attributes */
typedef enum {
	// Non-colour Attributes
	FMT_RESET = 0, // Reset all attributes
	FMT_BRIGHT = 1, // Bright
	FMT_DIM = 2, // Dim
	FMT_UNDER = 4, // Underscore
	FMT_BLINK = 5, // Blink
	FMT_INVERS = 7, // Reverse
	FMT_HIDDEN = 8, // Hidden
	FMT_ITALIC = 16, // Italic font
	FMT_FAINT = 32, // Faint color

	// Foreground Colours
	FMT_BLACK = 30, // Black
	FMT_RED = 31, // Red
	FMT_GREEN = 32, // Green
	FMT_YELLOW = 33, // Yellow
	FMT_BLUE = 34, // Blue
	FMT_MAGENTA = 35, // Magenta
	FMT_CYAN = 36, // Cyan
	FMT_WHITE = 37, // White

	// Background Colours
	FMT_BLACK_BG = 40, // Black
	FMT_RED_BG = 41, // Red
	FMT_GREEN_BG = 42, // Green
	FMT_YELLOW_BG = 43, // Yellow
	FMT_BLUE_BG = 44, // Blue
	FMT_MAGENTA_BG = 45, // Magenta
	FMT_CYAN_BG = 46, // Cyan
	FMT_WHITE_BG = 47, // White
} ANSI_attr_t;

#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5,4,3,2,1)
#define VA_NUM_ARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N

#define v100_attr(...) v100_attr_(VA_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

/**
 * Send formatting code to a com interface
 */
void v100_attr_(uint8_t count, ...);

#endif //MPORK_DEBUG_H
