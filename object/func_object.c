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
    FuncObject* f = (FuncObject*) obj;
    char tmp[128];
    char* s = strcpy(tmp, "<function ");
    s += sizeof("<function ") - 1;
    StrObject* func_name = (StrObject*) f->code->name;
    strncpy(s, (char*) func_name->str, func_name->size);
    s += func_name->size;
    strcpy(s, ">");
    s += sizeof(">") - 1;

    return string_new((uint8_t*) tmp, s - tmp);
}

static void func_destr(Object* obj) {
    FuncObject* func = (FuncObject*) obj;
    DECREF(func->code);
    free(obj);
}

static Object* cfunc_str(Object* obj) {
    CFuncObject* cf = (CFuncObject*) obj;

    char tmp[128];
    char* s = tmp;
    s = strcpy(tmp, "<builtin C function ") - 1;
    s += sizeof("<builtin C function ");
    s = strcpy(s, cf->name);
    s += strlen(cf->name);
    s = strcpy(s, ">");
    s += sizeof(">") - 1;

    return string_new((uint8_t*) tmp, s - tmp);
}

static void cfunc_destr(Object* obj) {
    free(obj);
}

#define CF(cfunc) \
static Object* cf_##cfunc##_call(TupleObject* tuple);   \
static CFuncObject cf_##cfunc = {                       \
    .base = {                                           \
        .refcnt = IMMORTAL_REF,                         \
        .type = &type_cfunc,                            \
    },                                                  \
    .call = cf_##cfunc##_call,                          \
    .name = #cfunc,                                     \
}

CF(print);
static Object* cf_print_call(TupleObject* tuple) {
    for (int i = 0; i < tuple->size; i++) {
        object_print(1, tuple->items[i]);
    }

    printf("\n");
    return string_new_cstr("cf ret\n");
}

CF(input);
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

CF(iter);
static Object* cf_iter_call(TupleObject* tuple) {
    if (tuple_size(tuple) != 1) {
        panic("iter() need 1 argument");
    }

    Object* iterable = tuple_get(tuple, 0);
    Object* iter = object_get_iter(iterable);
    return iter;
}

CF(next);
static Object* cf_next_call(TupleObject* tuple) {
    int size = tuple_size(tuple);
    if (size != 1 && size != 2) {
        panic("iter() need 1 or 2 argument(s)");
    }

    Object* iter = tuple_get(tuple, 0);
    Object* next_obj = iter->type->itnext(iter);

    if (next_obj) {
        return next_obj;
    }

    if (size == 1) {
        panic("Iteration ends");
    }

    Object* ret = tuple_get(tuple, 1);
    INCREF(ret);
    return ret;
}

// builtin functions
static struct {
    char* name;
    CFuncObject* func;
    Object* str;
} btfs[] = {
    {"print", &cf_print, NULL},
    {"input", &cf_input, NULL},
    {"iter", &cf_iter, NULL},
    {"next", &cf_next, NULL},
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
