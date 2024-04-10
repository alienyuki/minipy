#include "long_object.h"
#include "str_object.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static Object* long_str(Object* obj);

TypeObject type_long = {
    .name = "long",
    .str  = long_str,
};

Object* long_new(int n) {
    LongObject* ret = malloc(sizeof(LongObject));
    ret->n = n;
    ret->base.type = &type_long;
    return (Object*) ret;
}

static Object* long_str(Object* obj) {
    Object* ret;
    LongObject* o = (LongObject*) obj;
    char tmp[20];
    int size = sprintf(tmp, "%d", o->n);
    ret = string_new((uint8_t*) tmp, size);
    return ret;
}
