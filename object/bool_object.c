#include "bool_object.h"
#include "str_object.h"

static Object* bool_str(Object* obj);

TypeObject type_bool = {
    .name = "bool",
    .str  = bool_str,
};

Object true_object = {
    .refcnt = IMMORTAL_REF,
    .type = &type_bool,
};

Object false_object = {
    .refcnt = IMMORTAL_REF,
    .type = &type_bool,
};


Object* true_new() {
    return &true_object;
}

Object* false_new() {
    return &false_object;
}


static Object* bool_str(Object* obj) {
    if (obj == &true_object) {
        return string_new_cstr("True");
    } else {
        return string_new_cstr("False");
    }

    __builtin_unreachable();
}
