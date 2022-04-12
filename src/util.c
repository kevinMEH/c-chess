#include <stdarg.h>
#include <stdio.h>

int println(const char *format, ...) {
    va_list arg;
    int done;
    
    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    va_end(arg);
    puts("");
    
    return ++done;
}