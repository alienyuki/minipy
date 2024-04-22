#include "tuple_object.h"
#include "str_object.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static Object* tuple_str(Object* obj);
static void tuple_destr(Object* obj);

TypeObject type_tuple = {
    .name = "tuple",
    .str  = tuple_str,
    .destr = tuple_destr,
};

Object* tuple_new(int n) {
    TupleObject* ret = malloc(sizeof(TupleObject) + n * sizeof(Object*));
    ret->base.refcnt = 1;
    ret->base.type = &type_tuple;
    ret->size = n;
    return (Object*) ret;
}

static Object* tuple_str(Object* obj) {
    assert(obj->type == &type_tuple);
    TupleObject* o = (TupleObject*) obj;
    uint8_t tmp[2048];
    int tmp_index = 0;
    tmp[0] = '(';
    tmp_index += 1;
    for (int i = 0; i < o->size; i++) {
        Object* item = o->items[i];
        StrObject* s = (StrObject*) item->type->str(item);
        memcpy(tmp + tmp_index, s->str, s->size);
        tmp_index += s->size;
        tmp[tmp_index] = ',';
        tmp[tmp_index + 1] = ' ';
        tmp_index += 2;
        DECREF(s);
    }

    if (tmp_index != 1) {
        tmp_index -= 2;
    }

    tmp[tmp_index] = ')';
    tmp_index += 1;

    return string_new(tmp, tmp_index);
}

static void tuple_destr(Object* obj) {
    assert(obj->type == &type_tuple);
    TupleObject* o = (TupleObject*) obj;

    for (int i = 0; i < o->size; i++) {
        DECREF(o->items[i]);
    }

    free(o);
}

int tuple_set(Object* tuple, int index, Object* o) {
    assert(tuple->type == &type_tuple);
    TupleObject* t = (TupleObject*) tuple;
    assert(index < t->size);
    t->items[index] = o;
    return 0;
}

Object* tuple_get(TupleObject* tuple, int index) {
    return tuple->items[index];
}
