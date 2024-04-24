#include "object.h"
#include "str_object.h"

#include <unistd.h>
#include <stdio.h>

void object_print(int fd, Object* o) {
    StrObject* s = (StrObject*) o->type->str(o);

    write(fd, s->str, s->size);
    DECREF(s);
}

hash_t object_hash(Object* o) {
    return o->type->hash(o);
}

Object* object_compare(Object* o1, Object* o2, cmp_op op) {
    if (o1->type == o2->type) {
        return o1->type->cmp(o1, o2, op);
    }

    // incomplete implementation
    __builtin_unreachable();
}

void inc_ref(Object* o) {
    if (o->refcnt != IMMORTAL_REF) {
        o->refcnt += 1;
    }
}

void dec_ref(Object* o) {
    if (o->refcnt == IMMORTAL_REF) {
        return;
    }

    o->refcnt -= 1;
    if (o->refcnt == 0) {
        o->type->destr(o);
    }
}

Object* object_binary_add(Object* o1, Object* o2) {
    if (o1->type != o2->type) {
        printf("Not implement yet %s:%d\n", __FILE__, __LINE__);
        __builtin_unreachable();
    }

    return o1->type->num->add_func(o1, o2);
}

Object* object_binary_iadd(Object* o1, Object* o2) {
    if (o1->type != o2->type) {
        printf("Not implement yet %s:%d\n", __FILE__, __LINE__);
        __builtin_unreachable();
    }

    if (o1->type->num->iadd_func != NULL) {
        return o1->type->num->iadd_func(o1, o2);
    }

    return o1->type->num->add_func(o1, o2);
}
