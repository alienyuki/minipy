#include "marshal.h"
#include "object.h"
#include "code_object.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

#include <signal.h>
#include "debugger.h"

const char space[] = "                ";

void handler(int signum) {
    if (signum == SIGINT) {
        printf("sigint\n");
    } else {
        printf("sigsegv\n");
    }
    bt();
    exit(0);
}

int endpy(char* filename) {
    int n = strlen(filename);
    if (n < 3) {
        return 0;
    }

    if (strcmp(".py", filename + n - 3) == 0) {
        return n - 3;
    }
    return 0;
}


int ls(char* path, char** pycs, char* stack) {
    DIR* dir = opendir(path);
    int pi = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        int len = endpy(entry->d_name);
        if (len) {
            pycs[pi] = stack;
            memcpy(pycs[pi], entry->d_name, len);
            pi++;
            stack += len;
            *stack = 0;
            stack++;
        }
    }

    if (closedir(dir) != 0) {
        perror("closedir");
        return EXIT_FAILURE;
    }

    return 1;
}


int main(int argc, char** argv) {
    signal(SIGSEGV, handler);
    signal(SIGINT, handler);

    char* pycs[64];
    char  stack[4096];

    memset(pycs, 0, sizeof(pycs));
    ls("test", pycs, stack);
    for (int i = 0; pycs[i] != NULL; i++) {
        printf("%s\n", pycs[i]);
    }

    char** p = (argc == 1) ? pycs : (argv+1);

    char* s = getenv("MINIPY_DBG");
    int default_dbg = 1;
    if (s == NULL) {
        default_dbg = 0;
    }

    while (*p) {
        char filename[128];
        sprintf(filename, "test/__pycache__/%s.cpython-312.pyc", *p);
        int space_len = (sizeof("==========================")
                         - sizeof("testing ") - strlen(*p)) / 2;
        if (space_len < 0) {
            space_len = 0;
        }
        printf("\n"
               "==========================\n"
               "%.*stesting %s\n"
               "==========================\n\n", 
               space_len, space, *p);

        Object* code_obj = unmarshal_pyc(filename);

        printf("\n\n-------- vm start --------\n\n");

        pvm* vm = vm_init(default_dbg);
        pvm_run(vm, (CodeObject*) code_obj);
        DECREF(code_obj);
        vm_destroy(vm);
        printf("\n\n--------  vm end  --------\n\n");
        p++;
    }
    return 0;
}
