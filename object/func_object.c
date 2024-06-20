#include "func_object.h"
#include "dict_object.h"
#include "str_object.h"
#include "debugger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static Object* func_str(Object* obj);
static void func_destr(Object* obj);

static Object* cfunc_str(Object* obj);
static void cfunc_destr(Object* obj);


TypeObject type_func = {
    .name = "function",
    .str  = func_str,
    .destr = func_destr,
};


TypeObject type_cfunc = {
    .name = "cfunction",
    .str  = cfunc_str,
    .destr = cfunc_destr,
};


Object* func_new(CodeObject* code) {
    FuncObject* func = malloc(sizeof(FuncObject));
    // set func->code
    func->base.type = &type_func;
    func->base.refcnt = 1;
    func->code = code;
    INCREF(func->code);

    return (Object*) func;
}

static Object* func_str(Object* obj) {
    return string_new_cstr("func!");
}

static void func_destr(Object* obj) {
    FuncObject* func = (FuncObject*) obj;
    DECREF(func->code);
    free(obj);
}

static Object* cfunc_str(Object* obj) {
    return string_new_cstr("C func!");
}

static void cfunc_destr(Object* obj) {
    free(obj);
}

static Object* cf_print_call(TupleObject* tuple);

static CFuncObject cf_print = {
    .base = {
        .refcnt = IMMORTAL_REF,
        .type = &type_cfunc,
    },
    .call = cf_print_call,
};

static Object* cf_print_call(TupleObject* tuple) {
    for (int i = 0; i < tuple->size; i++) {
        object_print(1, tuple->items[i]);
    }

    Object* nl = string_new_cstr("\n");
    object_print(1, nl);
    DECREF(nl);
    return string_new_cstr("cf ret\n");
}

static Object* cf_input_call(TupleObject* tuple);

static CFuncObject cf_input = {
    .base = {
        .refcnt = IMMORTAL_REF,
        .type = &type_cfunc,
    },
    .call = cf_input_call,
};

static Object* cf_input_call(TupleObject* tuple) {
    if (tuple_size(tuple) == 1) {
        object_print(1, tuple_get(tuple, 0));
    }

    char input[128];
    int size;

    if (fgets(input, sizeof(input), stdin)) {
        int len = strlen(input);
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        size = len - 1;
    } else {
        UNREACHABLE();
    }

    return string_new((uint8_t*) input, size);;
}


// builtin functions
static struct {
    char* name;
    CFuncObject* func;
    Object* str;
} btfs[] = {
    {"print", &cf_print, NULL},
    {"input", &cf_input, NULL},
    {NULL, NULL, NULL}
};


Object* init_builtin_func() {
    DictObject* ret = (DictObject*) dict_new();
    for (int i = 0; btfs[i].name != NULL; i++) {
        if (btfs[i].str == NULL) {
            int size = strlen(btfs[i].name);
            btfs[i].str = string_new((uint8_t*) btfs[i].name, size);

        }
        dict_set(ret, btfs[i].str, (Object*) btfs[i].func);
    }

    return (Object*) ret;
}

void destroy_builtin_func() {
    for (int i = 0; btfs[i].name != NULL; i++) {
        DECREF(btfs[i].str);
        btfs[i].str = NULL;
    }
}
