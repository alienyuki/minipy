#include "long_object.h"
#include "str_object.h"
#include "bool_object.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static Object* long_str(Object* obj);
static void long_destr(Object* obj);
static Object* long_cmp(Object* o1, Object* o2, cmp_op op);
static hash_t long_hash(Object* obj);

static Object* long_add_func(Object* o1, Object* o2);
static Object* long_fdiv_func(Object* o1, Object* o2);
static Object* long_mul_func(Object* o1, Object* o2);
static Object* long_reminder_func(Object* o1, Object* o2);
static Object* long_sub_func(Object* o1, Object* o2);
static Object* long_iadd_func(Object* o1, Object* o2);

static number_methods long_number_method = {
    .add_func = long_add_func,
    .fdiv_func = long_fdiv_func,
    .mul_func = long_mul_func,
    .remainder_func = long_reminder_func,
    .sub_func = long_sub_func,
    .iadd_func = long_iadd_func,
};


TypeObject type_long = {
    .name = "long",
    .str  = long_str,
    .destr = long_destr,
    .cmp = long_cmp,
    .hash = long_hash,
    .num = &long_number_method,
};

Object* long_new(int n) {
    LongObject* ret = malloc(sizeof(LongObject));
    ret->base.type = &type_long;
    ret->base.refcnt = 1;
    ret->n = n;
    return (Object*) ret;
}

static void long_destr(Object* obj) {
    free(obj);
}

static Object* long_str(Object* obj) {
    Object* ret;
    LongObject* o = (LongObject*) obj;
    char tmp[20];
    int size = sprintf(tmp, "%d", o->n);
    ret = string_new((uint8_t*) tmp, size);
    return ret;
}

static Object* long_cmp(Object* o1, Object* o2, cmp_op op) {
    assert(o1->type == &type_long);
    assert(o2->type == &type_long);
    LongObject* l1 = (LongObject*) o1;
    LongObject* l2 = (LongObject*) o2;

    int diff = l1->n - l2->n;
    switch (op) {
    case CMP_EQ: {
        if (diff == 0) {
            return true_new();
        }
        break;
    }
    case CMP_GE: {
        if (diff >= 0) {
            return true_new();
        }
        break;
    }
    case CMP_GT: {
        if (diff > 0) {
            return true_new();
        }
        break;
    }
    case CMP_LE: {
        if (diff <= 0) {
            return true_new();
        }
        break;
    }
    case CMP_LT: {
        if (diff < 0) {
            return true_new();
        }
        break;
    }
    case CMP_NE: {
        if (diff != 0) {
            return true_new();
        }
        break;
    }

    }
    return false_new();
}

static hash_t long_hash(Object* obj) {
    LongObject* o = (LongObject*) obj;
    return o->n;
}

static Object* long_add_func(Object* o1, Object* o2) {
    assert(o1->type == &type_long);
    assert(o2->type == &type_long);
    LongObject* l1 = (LongObject*) o1;
    LongObject* l2 = (LongObject*) o2;

    return long_new(l1->n + l2->n);
}

static Object* long_mul_func(Object* o1, Object* o2) {
    assert(o1->type == &type_long);
    assert(o2->type == &type_long);
    LongObject* l1 = (LongObject*) o1;
    LongObject* l2 = (LongObject*) o2;

    return long_new(l1->n * l2->n);
}

static Object* long_sub_func(Object* o1, Object* o2) {
    assert(o1->type == &type_long);
    assert(o2->type == &type_long);
    LongObject* l1 = (LongObject*) o1;
    LongObject* l2 = (LongObject*) o2;

    return long_new(l1->n - l2->n);
}

static Object* long_fdiv_func(Object* o1, Object* o2) {
    assert(o1->type == &type_long);
    assert(o2->type == &type_long);
    LongObject* l1 = (LongObject*) o1;
    LongObject* l2 = (LongObject*) o2;

    return long_new(l1->n / l2->n);
}

static Object* long_reminder_func(Object* o1, Object* o2) {
    assert(o1->type == &type_long);
    assert(o2->type == &type_long);
    LongObject* l1 = (LongObject*) o1;
    LongObject* l2 = (LongObject*) o2;

    return long_new(l1->n % l2->n);
}


static Object* long_iadd_func(Object* o1, Object* o2) {
    assert(o1->type == &type_long);
    assert(o2->type == &type_long);
    LongObject* l1 = (LongObject*) o1;
    LongObject* l2 = (LongObject*) o2;

    return long_new(l1->n + l2->n);
}
