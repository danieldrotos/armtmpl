/**************************************************************************//*****
 * @file     stdio.c
 * @brief    Implementation of newlib syscall
 ********************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>

#include "ols_fs.h"

#undef errno
extern int errno;
extern int  _end;
extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

register char * stack_ptr asm("sp");

char *__env[1] = { 0 };
char **environ = __env;


/* Functions */
void initialise_monitor_handles()
{
}

int _getpid(void)
{
	return 1;
}

int _kill(int pid, int sig)
{
	errno = EINVAL;
	return -1;
}

void _exit (int status)
{
	_kill(status, -1);
	while (1) {}		/* Make sure we hang here */
}

/*This function is used for handle heap option*/
/*caddr_t _sbrk(int incr)
{
	extern char end asm("end");
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0)
		heap_end = &end;

	prev_heap_end = heap_end;
	if (heap_end + incr > stack_ptr)
	{
//		write(1, "Heap and stack collision\n", 25);
//		abort();
		errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}
*/

__attribute__ ((used))
int link(char *old, char *new)
{
    return -1;
}

__attribute__ ((used))
int _close(int file)
{
    return ols_close(file);
}


//__attribute__ ((used))
int _open(const char *fn, int fl, int m)
{
  int i;
  i= ols_open(fn, fl);
  if (i>=0)
    return i;
  else
    {
      errno= i;
      return -1;
    }
}

/*
__attribute__ ((used))
int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}
*/

__attribute__ ((used))
int _isatty(int file)
{
    return ols_isatty(file);
}

__attribute__ ((used))
int _lseek(int file, int ptr, int dir)
{
    return ols_seek(file, ptr, dir);
}

/*Low layer read(input) function*/
__attribute__ ((used))
int _read(int file, char *ptr, int len)
{
	return ols_read(file, ptr, len);
/*
	int i;
	//if (file >= 3)
	{
	   i= ols_read(file, ptr, len);
	   if (i >= 0)
		   return i;
	   else
	   {
		  errno= i;
		  return -1;
	   }
	}
	return 0;
*/
#if 0
     //user code example
     int i;
     (void)file;

     for(i = 0; i < len; i++)
     {
        // UART_GetChar is user's basic input function
        *ptr++ = UART_GetChar();
     }

#endif

    return len;
}


/*Low layer write(output) function*/
__attribute__ ((used))
int _write(int file, char *ptr, int len)
{
	//if (file < 0) return 0;
	return ols_write(file, ptr, len);
	/*
	{
	   int i= ols_write(file, ptr, len);
	   //printf("ols_write(%d,%p,%d)=%d\n",file,ptr,len, i);
	   if (i >= 0)
		   return i;
	   else
	   {
		  if (file > 2)
		    ;//printf("_write error %d file=%d\n", i, file);
		  errno= i;
		  return -1;
	   }
	}
	return 0;
	*/
#if 0
     //user code example

     int i;
     (void)file;

     for(i = 0; i < len; i++)
     {
        // UART_PutChar is user's basic output function
        UART_PutChar(*ptr++);
     }
#endif

    return len;
}

__attribute__ ((used))
void abort(void)
{
    /* Abort called */
    while(1);
}

int ioctl(int fdes, unsigned long int request, ...)
{
	va_list ap;
	int i;

	va_start(ap, request);
	i= ols_ioctl(fdes, request, ap);
	va_end(ap);

	return i;
}

int _wait(int *status)
{
	errno = ECHILD;
	return -1;
}

int _unlink(char *name)
{
	errno = ENOENT;
	return -1;
}

/*
int _times(struct tms *buf)
{
	return -1;
}
*/

int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

int _execve(char *name, char **argv, char **env)
{
	errno = ENOMEM;
	return -1;
}

int
_gettimeofday(struct timeval *TP, void *TZP)
{
  if (TP)
    {
      extern uint32_t HAL_GetTick(void);
      uint32_t l= HAL_GetTick();
      TP->tv_sec= l / 1000;
      TP->tv_usec= (l % 1000)*1000;
    }
  return 0;
}

/* --------------------------------- End Of File ------------------------------ */
