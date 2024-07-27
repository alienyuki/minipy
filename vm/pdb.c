#include "pdb.h"
#include "debugger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int read_line(char* prompt, char* input, int size) {
    printf("%s", prompt);
    while (!fgets(input, size, stdin)) {
        if (feof(stdin)) {
            clearerr(stdin);
            printf("^D\n%s", prompt);
            continue;
        } else {
            perror("Error reading input");
            break;
        }
    }

    return 0;
}

static int dbg_command(pvm* vm, char* input) {
    // convert input to cmd
    char* cmd[10];
    while (*input && *input == ' ') {
        input++;
    }
    cmd[0] = input;
    int cmd_index = 1;
    input++;

    while (*input) {
        if (*input == ' ') {
            *input = '\0';
        }
        if (*input && !(*(input-1))) {
            cmd[cmd_index] = input;
            cmd_index += 1;
        }
        input++;
    }

    *(input-1) = '\0';

    // for (int i = 0; i < cmd_index; i++) {
    //     printf("cmd: %s\n", cmd[i]);
    // }

    // action
    switch (*cmd[0]) {
    case 'b': {
        // function or line number
        if (strcmp(cmd[0], "b") == 0) {
            TODO("'b' command in debugger");
        } else {
            goto unknown;
        }
        break;
    }
    case 'c': {
        if (strcmp(cmd[0], "c") == 0) {
            vm->instr_step = INT32_MAX;
        } else {
            goto unknown;
        }
        break;
    }
    case 's': {
        if (strcmp(cmd[0], "si") == 0) {
            if (cmd_index == 1) {
                vm->instr_step = 1;
            } else {
                vm->instr_step = atoi(cmd[1]);
                if (vm->instr_step == 0) {
                    printf("invalid argument: %s\n", cmd[1]);
                }
            }
        } else if (strcmp(cmd[0], "s") == 0) {
            TODO("'s' command in debugger");
        } else {
            goto unknown;
        } 
        break;
    }
    default:
        goto unknown;
    }
    return 0;
unknown:
    printf("Unknown command %s\n", cmd[0]);
    return 1;
}


int pdb(pvm* vm) {
    while (vm->instr_step == 0) {
        char input[256];
        read_line("(yuki) ", input, sizeof(input));
        while (dbg_command(vm, input)) {
            read_line("(yuki) ", input, sizeof(input));
        }

        uint8_t* pc_base = vm->frame->code->bytecodes;
        Object** sp_base = vm->frame->localsplus;
        printf("pc: %ld, sp: %ld\n", vm->pc - pc_base, vm->sp - sp_base);
    }
    return 0;
}
