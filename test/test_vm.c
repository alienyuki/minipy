#include "marshal.h"
#include "object.h"
#include "code_object.h"
#include "vm.h"
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include "debugger.h"

const char space[] = "                ";

void handler(int signum) {
    printf("sigsegv\n");
    bt();
}

int main(int argc, char** argv) {
    signal(SIGSEGV, handler);
    signal(SIGINT, handler);
    char* pycs[] = {
        "arithmetic",
        "perfect_number",
        "func_2",
        "func",
        "while",
        "if",
        "add",
        "number",
        NULL,
    };

    char** p = (argc == 1) ? pycs : (argv+1);

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

        pvm* vm = vm_init();
        pvm_run(vm, (CodeObject*) code_obj);
        DECREF(code_obj);
        vm_destroy(vm);
        printf("\n\n--------  vm end  --------\n\n");
        p++;
    }
    return 0;
}
