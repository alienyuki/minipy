#ifndef TUPLE_OBJECT_H
#define TUPLE_OBJECT_H

#include "object.h"
#include <stdint.h>


typedef struct {
    Object base;
    int size;
    Object* items[];
} TupleObject;

extern TypeObject type_tuple;

Object* tuple_new(int size);
int tuple_set(Object* tuple, int index, Object* o);

#endif
