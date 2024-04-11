#include "str_object.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

static Object* string_str(Object* obj);
static void string_destr(Object* obj);

TypeObject type_string = {
    .name = "string",
    .str  = string_str,
    .destr = string_destr,
};

static void string_destr(Object* obj) {
    assert(obj->type == &type_string);
    StrObject* o = (StrObject*) obj;
    free(o);
}

Object* string_new(uint8_t* s, int size) {
    StrObject* ret = malloc(sizeof(StrObject) + size * sizeof(uint8_t));
    ret->base.type = &type_string;
    ret->base.refcnt = 1;
    ret->size = size;
    memcpy(ret->str, s, size);
    return (Object*) ret;
}

static Object* string_str(Object* obj) {
    INCREF(obj);
    return obj;
}
