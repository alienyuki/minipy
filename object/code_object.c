#include "code_object.h"
#include "str_object.h"

#include <stdlib.h>
#include <string.h>


static Object* code_str(Object* obj);
static void code_destr(Object* obj);


TypeObject type_code = {
    .name = "code",
    .destr = code_destr,
    .str = code_str,
};

CodeObject* init_code(CodeCons* CodeCons) {
    StrObject* bytecodes = (StrObject*) CodeCons->code;
    CodeObject* code = malloc(sizeof(CodeObject) + bytecodes->size * sizeof(uint8_t));
    code->base.type = &type_code;
    code->base.refcnt = 1;
    code->size = bytecodes->size;

    memcpy(code->bytecodes, bytecodes->str, bytecodes->size);
    return code;
}

static Object* code_str(Object* obj) {
    CodeObject* code = (CodeObject*) obj;
    Object* str = string_new(code->bytecodes, code->size);
    return str;
}

static void code_destr(Object* obj) {
    free(obj);
}
