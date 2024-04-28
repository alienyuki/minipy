#include "bool_object.h"
#include "str_object.h"
#include "debugger.h"

#include <assert.h>

static Object* bool_str(Object* obj);
static Object* bool_cmp(Object* o1, Object* o2, cmp_op op);


TypeObject type_bool = {
    .name = "bool",
    .str  = bool_str,
    .cmp = bool_cmp,
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

static Object* bool_cmp(Object* o1, Object* o2, cmp_op op) {
    assert(o1->type == &type_bool);
    assert(o2->type == &type_bool);
    switch (op) {
    case CMP_EQ: {
        if (o1 == o2) {
            true_new();
        }
        return false_new();
    }
    case CMP_NE: {
        if (o1 != o2) {
            true_new();
        }
        return false_new();
    }
    default: {
        UNREACHABLE();
    }
    }
}

