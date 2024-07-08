#include "code_object.h"
#include "str_object.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>


static Object* code_str(Object* obj);
static void code_destr(Object* obj);


TypeObject type_code = {
    .name = "code",
    .destr = code_destr,
    .str = code_str,
};

CodeObject* init_code(CodeCons* code_cons) {
    StrObject* bytecodes = (StrObject*) code_cons->code;
    CodeObject* code = malloc(sizeof(CodeObject) + bytecodes->size * sizeof(uint8_t));
    code->base.type = &type_code;
    code->base.refcnt = 1;
    code->size = bytecodes->size;
    code->consts = code_cons->consts;
    INCREF(code->consts);
    code->stack_size = code_cons->stacksize;
    code->localsplusnames = code_cons->localsplusnames;
    INCREF(code->localsplusnames);
    code->names = code_cons->names;
    INCREF(code->names);
    code->name = code_cons->name;
    INCREF(code->name);

    memcpy(code->bytecodes, bytecodes->str, bytecodes->size);
    return code;
}

static Object* code_str(Object* obj) {
    CodeObject* code = (CodeObject*) obj;
    Object* str = string_new(code->bytecodes, code->size);
    return str;
}

static void code_destr(Object* obj) {
    CodeObject* code = (CodeObject*) obj;
    DECREF(code->consts);
    DECREF(code->localsplusnames);
    DECREF(code->names);
    DECREF(code->name);
    free(obj);
}
