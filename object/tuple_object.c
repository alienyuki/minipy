#include "tuple_object.h"
#include <stdlib.h>
#include <assert.h>

TypeObject type_tuple = {
    .name = "tuple",
    .str  = NULL,
};

Object* tuple_new(int n) {
    TupleObject* ret = malloc(sizeof(TupleObject) + n * sizeof(uint8_t));
    ret->size = n;
    ret->base.type = &type_tuple;
    return (Object*) ret;
}

int tuple_set(Object* tuple, int index, Object* o) {
    assert(tuple->type == &type_tuple);
    TupleObject* t = (TupleObject*) tuple;
    assert(index < t->size);
    t->items[index] = o;
    return 0;
}
