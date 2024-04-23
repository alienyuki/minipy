#ifndef BOOL_OBJECT_H
#define BOOL_OBJECT_H

#include "object.h"

extern TypeObject type_bool;

Object* true_new();
Object* false_new();

#endif
