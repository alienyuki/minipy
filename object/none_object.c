#include "none_object.h"
#include "str_object.h"

static Object* none_str(Object* obj);

TypeObject type_none = {
    .name = "None",
    .str  = none_str,
};

Object none_object = {
    .type = &type_none,
};

Object* none_new() {
    return &none_object;
}

static Object* none_str(Object* obj) {
    return string_new((uint8_t*) "None", 4);
}
