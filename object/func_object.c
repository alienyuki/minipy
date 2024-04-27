#include "func_object.h"
#include "str_object.h"

#include <stdlib.h>
#include <stdio.h>

static Object* func_str(Object* obj);
static void func_destr(Object* obj);


TypeObject type_func = {
    .name = "function",
    .str  = func_str,
    .destr = func_destr,
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
