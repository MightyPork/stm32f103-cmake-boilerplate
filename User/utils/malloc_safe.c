#include <common.h>

#include "handlers.h"
#include "malloc_safe.h"

static void reset_when_done(void)
{
	//

	HAL_NVIC_SystemReset();
}


void *malloc_safe_do(size_t size, const char* file, uint32_t line)
{
	void *mem = malloc(size);
	if (mem == NULL) {
		// malloc failed
		user_error_file_line("Malloc failed", file, line);
		reset_when_done();
	}

	return mem;
}


void *calloc_safe_do(size_t nmemb, size_t size, const char* file, uint32_t line)
{
	void *mem = calloc(nmemb, size);
	if (mem == NULL) {
		// malloc failed
		user_error_file_line("Calloc failed", file, line);
		reset_when_done();
	}

	return mem;
}
