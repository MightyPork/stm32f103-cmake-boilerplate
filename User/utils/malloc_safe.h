#ifndef MPORK_MALLOC_SAFE_H
#define MPORK_MALLOC_SAFE_H

/**
 * Malloc that prints error and restarts the system on failure.
 */

#include <common.h>
#include <stm32f1xx_hal.h>

void *malloc_safe_do(size_t size, const char* file, uint32_t line);
void *calloc_safe_do(size_t nmemb, size_t size, const char* file, uint32_t line);

#define malloc_s(size)        malloc_safe_do(size,        __FILE__, __LINE__)
#define calloc_s(nmemb, size) calloc_safe_do(nmemb, size, __FILE__, __LINE__)

#endif //MPORK_MALLOC_SAFE_H
