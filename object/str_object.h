#ifndef STR_OBJECT_H
#define STR_OBJECT_H

#include "object.h"
#include <stdint.h>


typedef struct {
    Object base;
    int size;
    uint8_t str[];
} StrObject;

extern TypeObject type_string;

Object* string_new(uint8_t* s, int size);

#endif
