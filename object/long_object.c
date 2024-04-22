#include "long_object.h"
#include "str_object.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static Object* long_str(Object* obj);
static void long_destr(Object* obj);

static Object* long_add_func(Object* o1, Object* o2);

static number_methods long_number_method = {
    .add_func = long_add_func,
};


TypeObject type_long = {
    .name = "long",
    .str  = long_str,
    .destr = long_destr,
    .num = &long_number_method,
};

Object* long_new(int n) {
    LongObject* ret = malloc(sizeof(LongObject));
    ret->base.type = &type_long;
    ret->base.refcnt = 1;
    ret->n = n;
    return (Object*) ret;
}

static void long_destr(Object* obj) {
    free(obj);
}

static Object* long_str(Object* obj) {
    Object* ret;
    LongObject* o = (LongObject*) obj;
    char tmp[20];
    int size = sprintf(tmp, "%d", o->n);
    ret = string_new((uint8_t*) tmp, size);
    return ret;
}

static Object* long_add_func(Object* o1, Object* o2) {
    assert(o1->type == &type_long);
    assert(o2->type == &type_long);
    LongObject* l1 = (LongObject*) o1;
    LongObject* l2 = (LongObject*) o2;

    return long_new(l1->n + l2->n);
}
