#include "object.h"
#include "str_object.h"

#include <unistd.h>

void object_print(int fd, Object* o) {
    StrObject* s = (StrObject*) o->type->str(o);

    write(fd, s->str, s->size);
    DECREF(s);
}

hash_t object_hash(Object* o) {
    return o->type->hash(o);
}

int object_compare(Object* o1, Object* o2, cmp_op op) {
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
