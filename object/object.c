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
