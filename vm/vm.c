#include "vm.h"
#include "object.h"
#include "frame_object.h"
#include "tuple_object.h"
#include "bool_object.h"
#include "func_object.h"
#include "str_object.h"
#include "opcode.h"
#include "gc.h"
#include "debugger.h"


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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


static binary_op_func binary_ops[] = {
    [BINOP_ADD] = object_binary_add,
    [BINOP_FDIV] = object_binary_fdiv,
    [BINOP_MUL] = object_binary_mul,
    [BINOP_REMINDER] = object_binary_remainder,
    [BINOP_SUB] = object_binary_sub,
    [BINOP_IADD] = object_binary_iadd,
};

static int pvm_run_frame(pvm* vm) {
    if (vm == NULL) {
        return 0;
    }

    FrameObject* frame = vm->frame;
    uint8_t* p = vm->pc;
    int err = 0;
    while (!err) {
        while (vm->instr_step == 0) {
            char input[256];
            read_line("(yuki) ", input, sizeof(input));
            while (dbg_command(vm, input)) {
                read_line("(yuki) ", input, sizeof(input));
            }
            printf("pc: %ld, sp: %p\n", vm->pc - p, vm->sp);
        }
        vm->instr_step -= 1;

        switch (*vm->pc) {
        case POP_TOP: {
            Object* top = vm->sp[-1];
            DECREF(top);
            vm->sp -= 1;
            vm->pc += 2;
            break;
        }

        case PUSH_NULL: {
            vm->sp += 1;
            vm->sp[-1] = NULL;
            vm->pc += 2;
            break;
        }

        case BINARY_SUBSCR: {
            Object* sub = vm->sp[-1];
            vm->sp -= 1;
            Object* container = vm->sp[-1];
            vm->sp -= 1;

            Object* item = object_get_item(container, sub);
            DECREF(container);
            DECREF(sub);
            vm->sp += 1;
            vm->sp[-1] = item;
            INCREF(item);
            vm->pc += 4;
            break;
        }

        case STORE_SUBSCR: {
            Object* sub = vm->sp[-1];
            vm->sp -= 1;
            Object* container = vm->sp[-1];
            vm->sp -= 1;
            Object* item = vm->sp[-1];
            vm->sp -= 1;

            object_set_item(container, sub, item);
            DECREF(item);
            DECREF(container);
            DECREF(sub);

            vm->pc += 4;

            break;
        }

        case RETURN_VALUE: {
            Object* retval = vm->sp[-1];
            vm->sp -= 1;

            printf("sp: %p, bottom: %p\n", vm->sp, frame->localsplus);
            while (vm->sp > frame->localsplus) {
                DECREF(vm->sp[-1]);
                vm->sp -= 1;
            }
            // Object** f = frame->localsplus + frame->code->localsplusnames->size;
            // while (f < sp) DECREF(*f)

            vm->frame = vm->frame->prev_frame;
            DECREF(frame);
            frame = vm->frame;
            if (vm->frame == NULL) {
                goto done;
            }

            vm->pc = frame->pc - 2;
            vm->sp = frame->sp;
            // object_print(1, retval);
            vm->sp += 1;
            vm->sp[-1] = retval;
            vm->pc += 2;
            p = frame->code->bytecodes;
            break;
        }

        case RESUME: {
            vm->pc += 2;
            break;
        }

        case LOAD_CONST: {
            CodeObject* c = frame->code;
            Object* v = tuple_get(c->consts, *(vm->pc + 1));
            INCREF(v);
            vm->sp += 1;
            vm->sp[-1] = v;
            vm->pc += 2;
            break;
        }

        case STORE_NAME: {
            uint8_t arg = *(vm->pc + 1);
            Object* name = tuple_get(frame->code->names, arg);
            Object* stack_top = vm->sp[-1];
            dict_set(frame->locals, name, stack_top);
            DECREF(stack_top);
            vm->sp -= 1;
            vm->pc += 2;

            break;
        }

        case STORE_GLOBAL: {
            uint8_t arg = *(vm->pc + 1);

            // object_print(1, (Object*) frame->code->names);

            Object* name = tuple_get(frame->code->names, arg);
            Object* stack_top = vm->sp[-1];
            dict_set(vm->globals, name, stack_top);
            DECREF(stack_top);
            vm->sp -= 1;
            vm->pc += 2;

            break;
        }

        case LOAD_NAME: {
            uint8_t arg = *(vm->pc + 1);
            Object* name = tuple_get(frame->code->names, arg);
            Object* v = dict_get(frame->locals, name);
            INCREF(v);
            vm->sp += 1;
            vm->sp[-1] = v;
            vm->pc += 2;
            break;
        }

        case BUILD_LIST: {
            uint8_t arg = *(vm->pc + 1);
            Object* list = list_new(arg);
            for (int i = 0; i < arg; i++) {
                Object* o = vm->sp[-1];
                list_append(list, o);
                DECREF(o);
                vm->sp -= 1;
            }

            vm->sp += 1;
            vm->sp[-1] = list;
            vm->pc += 2;
            break;
        }

        case LOAD_ATTR: {
            Object* owner = vm->sp[-1];
            vm->sp -= 1;

            uint8_t arg = *(vm->pc + 1);
            Object* name = tuple_get(frame->code->names, arg >> 1);
            Object* attr = object_get_attr(owner, name);
            // load as function
            if (arg & 1) {
                vm->sp += 1;
                vm->sp[-1] = attr;
                vm->sp += 1;
                vm->sp[-1] = owner;
            } else {
                TODO("LOAD_ATTR load normal member");
            }
            vm->pc += 20;
            break;
        }

        case COMPARE_OP: {
            uint8_t arg = *(vm->pc + 1);
            Object* v2 = vm->sp[-1];
            vm->sp -= 1;
            Object* v1 = vm->sp[-1];
            vm->sp -= 1;

            Object* result = object_compare(v1, v2, arg >> 4);

            vm->sp += 1;
            vm->sp[-1] = result;
            DECREF(v1);
            DECREF(v2);
            vm->pc += 4;
            break;
        }

        case JUMP_FORWARD: {
            uint8_t arg = *(vm->pc + 1);
            vm->pc += arg * 2 + 2;

            break;
        }

        case LOAD_GLOBAL: {
            uint8_t arg = *(vm->pc + 1);

            Object* name = tuple_get(frame->code->names, arg >> 1);
            Object* gi = dict_get(vm->globals, name);

            if (arg & 1) {
                vm->sp += 1;
                vm->sp[-1] = NULL;
            }
            vm->sp += 1;
            vm->sp[-1] = gi;
            INCREF(gi);

            vm->pc += 10;
            break;
        }

        case POP_JUMP_IF_FALSE: {
            Object* stack_top = vm->sp[-1];
            uint8_t arg = *(vm->pc + 1);

            if (stack_top == true_new()) {
                vm->pc += 2;
            } else {
                vm->pc += arg * 2 + 2;
            }

            vm->sp -= 1;
            DECREF(stack_top);
            break;
        }

        case BINARY_OP: {
            uint8_t arg = *(vm->pc + 1);
            Object* v2 = vm->sp[-1];
            vm->sp -= 1;
            Object* v1 = vm->sp[-1];
            vm->sp -= 1;

            Object* v = binary_ops[arg](v1, v2);
            vm->sp += 1;
            vm->sp[-1] = v;
            DECREF(v1);
            DECREF(v2);
            vm->pc += 4;
            break;
        }

        case LOAD_FAST: {
            uint8_t arg = *(vm->pc + 1);
            Object* v = frame->localsplus[arg];
            INCREF(v);
            vm->sp += 1;
            vm->sp[-1] = v;
            vm->pc += 2;
            break;
        }

        case STORE_FAST: {

            uint8_t arg = *(vm->pc + 1);
            Object* v = vm->sp[-1];
            vm->sp -= 1;

            Object* old = frame->localsplus[arg];

            frame->localsplus[arg] = v;
            if (old != NULL) {
                DECREF(old);
            }

            vm->pc += 2;
            break;
        }

        case RETURN_CONST: {
            if (frame->prev_frame == NULL) {
                printf("\n");
                object_print(1, (Object*) frame->locals);
            }
            Object* retval = tuple_get(frame->code->consts, *(vm->pc + 1));

            // set vm->frame to prev_frame
            // destroy current frame
            // push retval to prev_frame's stack

            while (vm->sp > frame->localsplus) {
                DECREF(vm->sp[-1]);
                vm->sp -= 1;
            }

            vm->frame = vm->frame->prev_frame;
            DECREF(frame);
            frame = vm->frame;
            if (vm->frame == NULL) {
                goto done;
            }

            vm->pc = frame->pc - 2;
            vm->sp = frame->sp;
            // object_print(1, retval);
            INCREF(retval);
            vm->sp += 1;
            vm->sp[-1] = retval;
            vm->pc += 2;
            p = frame->code->bytecodes;
            break;
        }

        case MAKE_FUNCTION: {
            Object* code = vm->sp[-1];

            printf("%p, %s\n", code, code->type->name);
            Object* func = func_new((CodeObject*) code);

            DECREF(code);
            vm->sp -= 1;
            vm->sp += 1;
            vm->sp[-1] = func;

            vm->pc += 2;
            break;            
        }

        case JUMP_BACKWARD: {
            uint8_t arg = *(vm->pc + 1);
            vm->pc -= arg * 2;
            vm->pc += 2;
            break;
        }

        case CALL: {
            // get callable (FuncObject only now)
            // init frame from callable:
            //   set variables on new_frame->localsplus
            // save current frame:
            //   save pc, sp 
            // set frame on link list
            // vm->frame pointer to new frame

            uint8_t arg = *(vm->pc + 1);
            Object* callable = vm->sp[-(1 + arg)];
            Object* method = vm->sp[-(2 + arg)];
            // pop method and callable
            int pop_mc = 2;
            if (method != NULL) {
                callable = method;
                arg++;
                pop_mc = 1;
            }

            // object_print(1, callable);
            if (callable->type == &type_cfunc) {
                CFuncObject* cf = (CFuncObject*) callable;
                TupleObject* args = (TupleObject*) tuple_new(arg);

                // fill tuple for builtin c function;
                for (int i = 0; i < arg; i++) {
                    tuple_set((Object*) args, arg - 1 - i, vm->sp[-1]);
                    vm->sp -= 1;
                }

                // After fill func args, pop the stack(method and callable).
                for (int i = 0; i < pop_mc; i++) {
                    if (vm->sp[-1] != NULL) {
                        DECREF(vm->sp[-1]);
                    }
                    vm->sp -= 1;
                }

                Object* ret = cf->call(args);
                DECREF(args);
                // object_print(1, ret);

                vm->sp += 1;
                vm->sp[-1] = ret;
                vm->pc += 8;
                break;
            }

            FrameObject* new_frame = (FrameObject*) frame_new((FuncObject*) callable);
            new_frame->locals = (DictObject*) dict_new();

            // fill_args()
            // TODO
            for (int i = 0; i < arg; i++) {
                new_frame->localsplus[arg - 1 - i] = vm->sp[-1];
                // object_print(1, new_frame->localsplus[arg - 1 - i]);
                vm->sp -= 1;
            }

            // After fill func args, pop the stack(method and callable).
            for (int i = 0; i < pop_mc; i++) {
                if (vm->sp[-1] != NULL) {
                    DECREF(vm->sp[-1]);
                }
                vm->sp -= 1;
            }

            // save current frame
            vm->frame->pc = vm->pc + 8; // should point to next instr
            vm->frame->sp = vm->sp;

            // set frame on link list
            new_frame->next_frame = NULL;
            new_frame->prev_frame = vm->frame;
            vm->frame->next_frame = new_frame;
            int nlocalsplus = new_frame->code->localsplusnames->size;

            vm->frame = new_frame;
            frame = vm->frame;

            vm->sp = frame->localsplus + nlocalsplus;
            vm->pc = frame->code->bytecodes;
            p = vm->pc;
            break;
        }

        default: {
            TODO("opcode: 0x%x is not implement yet\n", *vm->pc);
            break;
        }

        }
    }

done:
    return err;
}


int pvm_run(pvm* vm, CodeObject* code) {
    int err = 0;
    FrameObject* frame = init_frame(code);
    vm->frame = frame;
    frame->prev_frame = NULL;
    frame->next_frame = NULL;
    vm->pc = frame->code->bytecodes;
    int nlocalsplus = code->localsplusnames->size;
    vm->sp = frame->localsplus + nlocalsplus;

    // Load builtin C functions
    vm->globals = (DictObject*) init_builtin_func();

    frame->locals = vm->globals;
    INCREF(frame->locals);

    pvm_run_frame(vm);

    return err;
}

pvm* vm_init(int default_dbg) {
    string_type_init();
    list_type_init();

    pvm* vm = malloc(sizeof(pvm));
    memset(vm, 0, sizeof(pvm));
    if (default_dbg) {
        vm->instr_step = 0;
    } else {
        vm->instr_step = INT32_MAX;
    }
    return vm;
}

void vm_destroy(pvm* vm) {
    if (vm->globals != NULL) {
        DECREF(vm->globals);
    }
    destroy_builtin_func();
    free(vm);

    list_type_destroy();
    string_type_destroy();
}
