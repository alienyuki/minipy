#include "vm.h"
#include "object.h"
#include "frame_object.h"
#include "tuple_object.h"
#include "bool_object.h"
#include "func_object.h"
#include "str_object.h"
#include "opcode.h"
#include "debugger.h"


#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static binary_op_func binary_ops[] = {
    [BINOP_ADD] = object_binary_add,
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
        printf("pc: %ld\n", vm->pc - p);
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

        case RETURN_CONST: {
            object_print(1, (Object*) frame->locals);
            Object* retval = tuple_get(frame->code->consts, *(vm->pc + 1));

            // set vm->frame to prev_frame
            // destroy current frame
            // push retval to prev_frame's stack

            // maybe use this to free stack?
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
            printf("after sp: %p\nretval\n", vm->sp);
            object_print(1, retval);
            INCREF(retval);
            vm->sp += 1;
            vm->sp[-1] = retval;
            vm->pc += 2;
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
            if (method != NULL) {
                TODO("method");
            }

            object_print(1, callable);
            FrameObject* new_frame = (FrameObject*) frame_new((FuncObject*) callable);
            new_frame->locals = (DictObject*) dict_new();
            // fill_args()
            // TODO
            // After fill func args, pop the stack, which contains method, callable, and args
            for (int i = 0; i < 2 + arg; i++) {
                if (vm->sp[-1] != NULL) {
                    DECREF(vm->sp[-1]);
                }
                vm->sp -= 1;
            }

            // save current frame
            vm->frame->pc = vm->pc + 8; // should point to next instr
            vm->frame->sp = vm->sp;
            // sp should minus args
            printf("before sp!! %p\n\n", vm->sp);

            // set frame on link list
            new_frame->next_frame = NULL;
            new_frame->prev_frame = vm->frame;
            vm->frame->next_frame = new_frame;
            int nlocalsplus = new_frame->code->localsplusnames->size;
            vm->sp = frame->localsplus + nlocalsplus;

            vm->frame = new_frame;
            frame = vm->frame;

            vm->pc = frame->code->bytecodes;
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
    vm->globals = (DictObject*) dict_new();
    frame->locals = vm->globals;
    INCREF(frame->locals);

    pvm_run_frame(vm);

    return err;
}

pvm* vm_init() {
    pvm_run_frame(NULL);
    pvm* vm = malloc(sizeof(pvm));
    memset(vm, 0, sizeof(pvm));
    return vm;
}

void vm_destroy(pvm* vm) {
    if (vm->globals != NULL) {
        DECREF(vm->globals);
    }
    free(vm);
}
