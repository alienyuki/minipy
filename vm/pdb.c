#include "str_object.h"
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
    if (*input == '\n') {
        strcpy(input, vm->last_dbg_cmd);
    } else {
        strcpy(vm->last_dbg_cmd, input);
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
            if (cmd_index == 2) {
                // Add :
                Object* s = string_new((uint8_t*) cmd[1], strlen(cmd[1]));
                Object* v = dict_get(vm->globals, s);
                DECREF(s);
                if (!v) {
                    printf("%s is not a loaded function\n"
                           "If %s is a function, use bf to break it\n",
                           cmd[1], cmd[1]);
                    break;
                }
                if (!(v->type == &type_func || v->type == &type_cfunc)) {
                    printf("%s is not a function\n", cmd[1]);
                    break;
                }

                vm->breakpoints[vm->bpn].type = BP_FUNCTION;
                strcpy(vm->breakpoints[vm->bpn].f.func, cmd[1]);
                vm->bpn += 1;

            } else if (cmd_index == 1) {
                TODO("print break points");
            }
        } else if (strcmp(cmd[0], "bf") == 0) {
            TODO("bf command");
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
    case 'p': {
        if (strcmp(cmd[0], "p") == 0) {
            if (cmd_index < 2) {
                printf("SyntaxError: %s should follow one argument", cmd[0]);
            }

            TupleObject* locals = vm->frame->code->localsplusnames;
            for (int i = 0; i < tuple_size(locals); i++) {
                StrObject* s = (StrObject*) tuple_get(locals, i);
                if ((strlen(cmd[1]) == s->size)
                    && memcmp(s->str, cmd[1], s->size) == 0) {
                    object_print(1, vm->frame->localsplus[i]);
                    printf("\n");
                    break;
                }
            }

            Object* s = string_new((uint8_t*) cmd[1], strlen(cmd[1]));
            Object* v = dict_get(vm->globals, s);
            DECREF(s);
            if (v) {
                object_print(1, v);
                printf("\n");
                break;
            }
            printf("name '%s' is not defined\n", cmd[1]);
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

int pdb_triggered(pvm* vm) {
    uint8_t* pc_base = vm->frame->code->bytecodes;
    if (vm->pc - pc_base == 0) {
        StrObject* s = (StrObject*) vm->frame->code->name;
        for (int i = 0; i < vm->bpn; i++) {
            if (oscscmp(s, vm->breakpoints[i].f.func) == 0) {
                vm->instr_step = 0;
                printf("break at function %s\n", vm->breakpoints[i].f.func);
                return 1;
            }
        }
    }
    return vm->instr_step == 0;
}

int pdb(pvm* vm) {
    while (vm->instr_step == 0) {
        char input[256];
        read_line("(yuki) ", input, sizeof(input));
        while (dbg_command(vm, input)) {
            read_line("(yuki) ", input, sizeof(input));
        }

        uint8_t* pc_base = vm->frame->code->bytecodes;
        int nlocalsplus = vm->frame->code->localsplusnames->size;
        Object** sp_base = vm->frame->localsplus + nlocalsplus;
        printf("pc: %ld, sp: %ld\n", vm->pc - pc_base, vm->sp - sp_base);
    }
    return 0;
}
