#include "str_object.h"
#include "bool_object.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

static Object* string_str(Object* obj);
static void string_destr(Object* obj);
static hash_t string_hash(Object* obj);
static Object* string_cmp(Object* o1, Object* o2, cmp_op op);

TypeObject type_string = {
    .name = "string",
    .str  = string_str,
    .destr = string_destr,
    .hash = string_hash,
    .cmp = string_cmp,
};

static void string_destr(Object* obj) {
    assert(obj->type == &type_string);
    StrObject* o = (StrObject*) obj;
    free(o);
}

Object* string_new(uint8_t* s, int size) {
    StrObject* ret = malloc(sizeof(StrObject) + size * sizeof(uint8_t));
    ret->base.type = &type_string;
    ret->base.refcnt = 1;
    ret->size = size;
    memcpy(ret->str, s, size);
    return (Object*) ret;
}

static Object* string_str(Object* obj) {
    INCREF(obj);
    return obj;
}

static hash_t string_hash(Object* obj) {
    StrObject* s = (StrObject*) obj;
    hash_t hash = 5381;

    // djb2
    for (int i = 0; i < s->size; i++) {
        hash = ((hash << 5) + hash) + s->str[i];
    }

    return hash;
}

static Object* string_cmp(Object* o1, Object* o2, cmp_op op) {
    StrObject* s1 = (StrObject*) o1;
    StrObject* s2 = (StrObject*) o2;
    if (op == CMP_EQ) {
        if (s1->size != s2->size) {
            return false_new();
        }

        for (int i = 0; i < s1->size; i++) {
            if (s1->str[i] != s2->str[i]) {
                return false_new();
            }
        }
        return true_new();
    }

    __builtin_unreachable();
}
