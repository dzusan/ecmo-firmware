#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <assert.h>

#define DEBUG_VIEW_LEN (20)

void assert_func(const char* file, int line, const char* func, const char* expr)
{

    (void) line;
    (void) expr;
    int i = 0;
    int k = 0;
    char sym = 0;
    volatile char debug_view_file[DEBUG_VIEW_LEN] = {0};
    volatile char debug_view_func[DEBUG_VIEW_LEN] = {0};

    i = -1;

    do
    {
        i++;
        sym = file[i];
    }
    while (sym != '\0');

    if (i < DEBUG_VIEW_LEN)
    {
        k = i;
    }
    else
    {
        k = DEBUG_VIEW_LEN - 1;
    }

    while (i >= 0 && k >= 0)
    {
        debug_view_file[k] = file[i];
        i--;
        k--;
    }

    i = -1;

    do
    {
        i++;
        sym = func[i];
    }
    while (sym != '\0');

    if (i < DEBUG_VIEW_LEN)
    {
        k = i;
    }
    else
    {
        k = DEBUG_VIEW_LEN - 1;
    }

    while (i >= 0 && k >= 0)
    {
        debug_view_func[k] = func[i];
        i--;
        k--;
    }

    for (;;)
    {

    }
}

int _getpid(void)
{

    return 1;
}

int _kill(int pid, int sig)
{

    (void) pid;
    (void) sig;
    errno = EINVAL;
    return -1;
}

void _exit(int status)
{

    (void) _kill(status, -1);

    while (1) {}		/* Make sure we hang here */
}

__attribute__((weak)) int _read(int file, char* ptr, int len)
{

    (void) file;
    (void) ptr;
    assert(0);
    errno = EBADF;
    return -1;
}

__attribute__((weak)) int _write(int file, char* ptr, int len)
{

    (void) file;
    (void) ptr;
    (void) len;
    errno = EBADF;
    return -1;
}

int _close(int file)
{

    (void) file;
    return -1;
}

int _fstat(int file, struct stat* st)
{

    (void) file;
    (void) st;
    st->st_mode = (uint32_t) S_IFCHR;
    return 0;
}

int _isatty(int file)
{

    (void) file;
    return 1;
}

int _lseek(int file, int ptr, int dir)
{

    (void) file;
    (void) ptr;
    (void) dir;
    return 0;
}

int _open(char* path, int flags, ...)
{

    (void) path;
    (void) flags;
    /* Pretend like we always fail */
    return -1;
}

int _wait(int* status)
{

    (void) status;
    errno = ECHILD;
    return -1;
}

int _unlink(char* name)
{

    (void) name;
    errno = ENOENT;
    return -1;
}

int _times(struct tms* buf)
{

    (void) buf;
    return -1;
}

int _stat(char* file, struct stat* st)
{

    (void) file;
    (void) st;
    st->st_mode = (uint32_t) S_IFCHR;
    return 0;
}

int _link(char* old, char* new)
{

    (void) old;
    (void) new;
    errno = EMLINK;
    return -1;
}

int _fork(void)
{

    errno = EAGAIN;
    return -1;
}

int _execve(char* name, char** argv, char** env)
{

    (void) name;
    (void) argv;
    (void) env;
    errno = ENOMEM;
    return -1;
}
