#include "debugger.h"

#include <stdio.h>
#include <stdarg.h>
#include <execinfo.h>
#include <stdlib.h>
#include <assert.h>


void bt() {
    void *buffer[128];
    int nptrs = backtrace(buffer, 128);
    char** strings = backtrace_symbols(buffer, nptrs);

    for (int j = 0; j < nptrs; j++) {   
        printf("%s\n", strings[j]);
    }

    free(strings);
}


void todo(char* prompt, char* filename, int line, ...) {
    va_list ap;
    char tmp[128];
    va_start(ap, line);
    vsprintf(tmp, prompt, ap);
    va_end(ap);

    printf("\n\033[31mTODO(%s:%d): %s\033[0;39m\n", filename, line, tmp);
    exit(1);
}

void panic(char* prompt, ...) {
    printf("\n\033[31mpanic!\n");
    va_list ap;
    va_start(ap, prompt);
    vprintf(prompt, ap);
    va_end(ap);
    printf("\033[0;39m\n");
    bt();
    exit(1);
}
