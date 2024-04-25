#ifndef DEBUGGER_H
#define DEBUGGER_H

void bt();
void todo(char* prompt, char* filename, int line, ...);
void panic(char* prompt, ...);

#define TODO(prompt, ...) todo(prompt, __FILE__, __LINE__, ##__VA_ARGS__)
#define UNREACHABLE() \
    do { \
        panic("unreachable: %s:%d", __FILE__, __LINE__); \
        __builtin_unreachable(); \
    } while(0)

#endif
