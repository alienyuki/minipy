#include "func_object.h"
#include "str_object.h"

#include <stdlib.h>
#include <stdio.h>

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

CFuncObject cf_print = {
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
