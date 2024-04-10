#ifndef LONG_OBJECT_H
#define LONG_OBJECT_H

#include "object.h"
#include <stdint.h>


typedef struct {
    Object base;
    int n;
} LongObject;

extern TypeObject type_long;

Object* long_new(int n);

#endif
