#include "str_object.h"
#include "dict_object.h"
#include "func_object.h"
#include "bool_object.h"
#include "debugger.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

static Object* string_str(Object* obj);
static void string_destr(Object* obj);
static hash_t string_hash(Object* obj);
static Object* string_cmp(Object* o1, Object* o2, cmp_op op);
static Object* string_get_attr(Object* owner, Object* attr);

TypeObject type_string = {
    .name = "string",
    .str  = string_str,
    .destr = string_destr,
    .hash = string_hash,
    .cmp = string_cmp,
    .get_attr = string_get_attr,
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

    TODO("string compare");
}

static Object* string_get_attr(Object* owner, Object* attr) {
    Object* ret = dict_get((DictObject*) owner->type->dict, attr);
    if (ret) {
        return ret;
    }

    return NULL;
}

static Object* str_upper_call(TupleObject* tuple);
static CFuncObject str_upper = {
    .base = {
        .refcnt = IMMORTAL_REF,
        .type = &type_cfunc,
    },
    .call = str_upper_call,
};

static Object* str_upper_call(TupleObject* tuple) {
    StrObject* str = (StrObject*) tuple_get(tuple, 0);

    StrObject* ret = malloc(sizeof(StrObject) + str->size * sizeof(uint8_t));
    ret->base.type = &type_string;
    ret->base.refcnt = 1;
    ret->size = str->size;

    for (int i = 0; i < ret->size; i++) {
        ret->str[i] = toupper(str->str[i]);
    }

    return (Object*) ret;
}


static Object* s[256] = {};
static int s_len;

__attribute__((constructor)) static void con() {
    type_string.dict = dict_new();
    s[s_len++] = string_new_cstr("upper");
    dict_set((DictObject*) type_string.dict, s[s_len-1], (Object*) &str_upper);
}

__attribute__((destructor)) static void des() {
    DECREF(type_string.dict);
    for (int i = 0; i < s_len; i++) {
        DECREF(s[i]);
    }
}
