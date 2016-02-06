/**************************************************************************/ /*****
 * @file     stdio.c
 * @brief    Implementation of newlib syscall
 ********************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

#undef errno
extern int errno;
extern int _end;

#define UNUSED(x) ((void)x)

/*This function is used for handle heap option*/
__attribute__((used))
caddr_t
_sbrk(int incr)
{
    static unsigned char* heap = NULL;
    unsigned char* prev_heap;

    if (heap == NULL) {
        heap = (unsigned char*)&_end;
    }
    prev_heap = heap;

    heap += incr;

    return (caddr_t)prev_heap;
}

__attribute__((used)) int link(char* old, char* new)
{
    UNUSED(old);
    UNUSED(new);
    return -1;
}

__attribute__((used)) int _close(int file)
{
    UNUSED(file);
    return -1;
}

__attribute__((used)) int _fstat(int file, struct stat* st)
{
    UNUSED(file);
    st->st_mode = S_IFCHR;
    return 0;
}

__attribute__((used)) int _isatty(int file)
{
    UNUSED(file);
    return 1;
}

__attribute__((used)) int _lseek(int file, int ptr, int dir)
{
    UNUSED(file);
    UNUSED(ptr);
    UNUSED(dir);
    return 0;
}

/*Low layer read(input) function*/
__attribute__((used)) int _read(int file, char* ptr, int len)
{
    UNUSED(file);
    UNUSED(ptr);
    return len;
}

/*Low layer write(output) function*/
__attribute__((used)) int _write(int file, char* ptr, int len)
{
    UNUSED(file);
    UNUSED(ptr);
    return len;
}

__attribute__((used)) void abort(void)
{
    /* Abort called */
    while (1)
        ;
}
/* --------------------------------- End Of File ------------------------------ */
