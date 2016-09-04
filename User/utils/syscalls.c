#include <errno.h>
#include <stdio.h>
#include <usart.h>
#include <sys/stat.h>

register char *stack_ptr asm("sp");

caddr_t _sbrk(int incr)
{
	extern char end __asm("end");
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0)
		heap_end = &end;

	prev_heap_end = heap_end;
	if (heap_end + incr > stack_ptr) {
//		write(1, "Heap and stack collision\n", 25);
//		abort();
		errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}


/**
 * @brief Write to a file by file descriptor.
 *
 * @param fd  : open file descriptor
 * @param buf : data to write
 * @param len : buffer size
 * @return number of written bytes
 */
int _write(int fd, const char *buf, int len)
{
	switch (fd) {
		case 1: // stdout
		case 2: // stderr
			HAL_UART_Transmit(&huart1, (uint8_t*)buf, len, 10);
			return len;

		default:
			return 0;
	}
}

// region stubs

int _read(int fd, char *buf, int len)
{
	return 0;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

int _close(int file)
{
	return -1;
}

int _isatty(int fd)
{
	if (fd == 0 || fd == 1 || fd == 2)
		return 1;
	else
		return 0;
}

// endregion