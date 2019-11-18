/*
 * syscalls.c
 *
 *  Created on: 2 Nov 2010
 *     Author: nanoage.co.uk
 *  Changed on: 2012-05-27
 *     Author: Christian Krueger
 *     Changes: Directed _write() output to __io_putchar()
 */
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
//#include "stm32f2xx.h"
//#include "stm322xg_eval.h"

// #include "stm32f303xe.h"
// #include "core_cm4.h"


#undef errno
extern int errno;

/*
 environ
 A pointer to a list of environment variables and their values.
 For a minimal environment, this empty list is adequate:
 */
char *__env[1] = { 0 };
char **environ = __env;


int _write(int file, char *ptr, int len);

#pragma weak __io_putchar /**< Defines following __io_putchar() as weak, it will only be used, if there is not other implementation (e.g. lcd-log) */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int __io_putchar(int ch)
{
	/*
  static int USART_ready = 0;
  static USART_InitTypeDef USART_InitStructure;

  
  
if (USART_ready != 1)
  {
    // Init USART 3
    USART_InitStructure.USART_BaudRate = 19200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    STM_EVAL_COMInit(COM1, &USART_InitStructure);

    // Set ready flag
    USART_ready = 1;
  }

  // Place your implementation of fputc here
  // e.g. write a character to the USART
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  // Loop until the end of transmission
  while(USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {
  }
*/
  return ch;

}



void _exit(int status)
{
  _write(1, "exit", 4);
  while (1)
  {
    ;
  }
}

int _close(int file)
{
  return -1;
}
/*
 execve
 Transfer control to a new process. Minimal implementation (for a system without processes):
 */
  
int _execve(char *name, char **argv, char **env)
{
  errno = ENOMEM;
  return -1;
}
/*
 fork
 Create a new process. Minimal implementation (for a system without processes):
 */

int _fork()
{
  errno = EAGAIN;
  return -1;
}
/*
 fstat
 Status of an open file. For consistency with other minimal implementations in these examples,
 all files are regarded as character special devices.
 The `sys/stat.h' header file required is distributed in the `include' subdirectory for this C library.
 */
int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

/*
 getpid
 Process-ID; this is sometimes used to generate strings unlikely to conflict with other processes. Minimal implementation, for a system without processes:
 */

int _getpid()
{
  return 1;
}

/*
 isatty
 Query whether output stream is a terminal. For consistency with the other minimal implementations,
 */
int _isatty(int file)
{
  
  
switch (file)
  {
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
      return 1;
    default:
      //errno = ENOTTY;
      errno = EBADF;
      return 0;
  }
}

/*
 kill
 Send a signal. Minimal implementation:
 */
int _kill(int pid, int sig)
{
  errno = EINVAL;
  return (-1);
}

/*
 link
 Establish a new name for an existing file. Minimal implementation:
 */
int _link(char *old, char *new)
{
  errno = EMLINK;
  return -1;
}

/*
 lseek
 Set position in a file. Minimal implementation:
 */
int _lseek(int file, int ptr, int dir)
{
  return 0;
}

/*
 sbrk
 Increase program data space.
 Malloc and related functions depend on this
 */
caddr_t _sbrk(int incr)
{
/*
	static unsigned char *heap = NULL;
	    unsigned char *prev_heap;

	    if (heap == NULL) {
	      heap = (unsigned char *)&_end;
	    }
	    prev_heap = heap;
	    // check removed to show basic approach

	    heap += incr;

	    return (caddr_t) prev_heap;
*/

// extern char _ebss; // Defined by the linker
extern char __heap_start; // Defined by the linker
  static char *heap_end;
  char *prev_heap_end;

  if (heap_end == 0)
  {
    // heap_end = &_ebss;
    heap_end = &__heap_start;
  }
  prev_heap_end = heap_end;

  // char * stack = (char*) __get_MSP();
  // if (heap_end + incr > stack)
  // {
  //   _write(STDERR_FILENO, "Heap and stack collision\n", 25);
  //   errno = ENOMEM;
  //   return (caddr_t) -1;
  //   //abort ();
  // }

  heap_end += incr;
  return (caddr_t) prev_heap_end;

}
/*
 read
 Read a character to a file. `libc' subroutines will use this system routine for input from all files, including stdin
 Returns -1 on error or blocks until the number of characters have been read.
 */
int _read(int file, char *ptr, int len)
{
//  int n;
  int num = 0;
  switch (file)
  {
    case STDIN_FILENO:
//      for (n = 0; n < len; n++)
//      {
//        while ((USART3->SR & USART_FLAG_RXNE) == (uint16_t) RESET)
//        {
//        }
//        char c = (char) (USART3->DR & (uint16_t) 0x01FF);
//        *ptr++ = c;
//        num++;
//      }
      break;
    
  
default:
      errno = EBADF;
      return -1;
  }
  return num;
}

/*
 stat
 Status of a file (by name). Minimal implementation:
 int    _EXFUN(stat,( const char *__path, struct stat *__sbuf ));
 */
int _stat(const char *filepath, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

/*
 times
 Timing information for current process. Minimal implementation:
 */
clock_t _times(struct tms *buf)
{
  return -1;
}

/*
 unlink
 Remove a file's directory entry. Minimal implementation:
 */
int _unlink(char *name)
{
  errno = ENOENT;
  return -1;
}

/*
 wait
 Wait for a child process. Minimal implementation:
 */
int _wait(int *status)
{
  errno = ECHILD;
  return -1;
}

  
/*
 write
 Write a character to a file. `libc' subroutines will use this system routine for output to all files, including stdout
 Returns -1 on error or number of bytes sent
 */
int _write(int file, char *ptr, int len)
{
  int n;

  switch (file)
  {
    case STDOUT_FILENO: /*stdout*/
      for (n = 0; n < len; n++)
      {
        __io_putchar((*ptr++));
      }
      break;
    case STDERR_FILENO: /* stderr */
      for (n = 0; n < len; n++)
      {
        __io_putchar((*ptr++));
      }
      break;
    default:
      errno = EBADF;
      return -1;
  }
  return len;
}
