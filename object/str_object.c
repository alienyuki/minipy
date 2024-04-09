#include "str_object.h"
#include <stdlib.h>
#include <string.h>

static Object* string_str(Object* obj);

TypeObject type_string = {
    .name = "string",
    .str  = string_str,
};

Object* string_new(uint8_t* s, int size) {
    StrObject* ret = malloc(sizeof(StrObject) + size * sizeof(uint8_t));
    ret->base.type = &type_string;
    ret->size = size;
    memcpy(ret->str, s, size);
    return (Object*) ret;
}

static Object* string_str(Object* obj) {
    return obj;
}
