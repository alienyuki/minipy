#include "object.h"
#include "str_object.h"
#include "debugger.h"

#include <unistd.h>
#include <stdio.h>

void object_print(int fd, Object* o) {
    StrObject* s = (StrObject*) o->type->str(o);

    if (fd == 1) {
        printf("%.*s", s->size, (char*) s->str);
    } else {
        write(fd, s->str, s->size);
    }
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
    TODO("different types compare");
}

Object* object_get_attr(Object* owner, Object* name) {
    TypeObject* tp = owner->type;
    Object* ret;
    if (tp->get_attr) {
        ret = tp->get_attr(owner, name);
        if (ret) {
            return ret;
        }
    }

    TODO("load attr when attr not in type");
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
        TODO("different types in binary add");
    }

    return o1->type->num->add_func(o1, o2);
}

Object* object_binary_fdiv(Object* o1, Object* o2) {
    if (o1->type != o2->type) {
        TODO("different types in binary add");
    }

    return o1->type->num->fdiv_func(o1, o2);
}

Object* object_binary_mul(Object* o1, Object* o2) {
    if (o1->type != o2->type) {
        TODO("different types in binary mul");
    }

    return o1->type->num->mul_func(o1, o2);
}

Object* object_binary_remainder(Object* o1, Object* o2) {
    if (o1->type != o2->type) {
        TODO("different types in binary remainder");
    }

    return o1->type->num->remainder_func(o1, o2);
}

Object* object_binary_sub(Object* o1, Object* o2) {
    if (o1->type != o2->type) {
        TODO("different types in binary sub");
    }

    return o1->type->num->sub_func(o1, o2);
}

Object* object_binary_iadd(Object* o1, Object* o2) {
    if (o1->type != o2->type) {
        TODO("different types in binary iadd");
    }

    if (o1->type->num->iadd_func != NULL) {
        return o1->type->num->iadd_func(o1, o2);
    }

    return o1->type->num->add_func(o1, o2);
}
