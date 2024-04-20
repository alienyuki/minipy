#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>

typedef struct Object Object;
typedef struct TypeObject TypeObject;
typedef uint64_t hash_t;

typedef enum {
    CMP_EQ,
    CMP_NE,
    CMP_LT,
    CMP_GT,
    CMP_LE,
    CMP_GE,
} cmp_op;

typedef Object* (*str_func)(Object*);
typedef void (*destr_func)(Object*);
typedef hash_t (*hash_func)(Object*);
typedef int (*compare_func)(Object*, Object*, cmp_op);

struct Object {
    int refcnt;
    TypeObject* type;
};


struct TypeObject {
    char* name;
    str_func str;
    destr_func destr;
    hash_func hash;
    compare_func cmp;
};


void object_print(int fd, Object* o);
hash_t object_hash(Object* o);
int object_compare(Object* o1, Object* o2, cmp_op op);

#define IMMORTAL_REF (1 << 30)

void inc_ref(Object* o);
void dec_ref(Object* o);

#define INCREF(o) inc_ref((Object*) o)
#define DECREF(o) dec_ref((Object*) o)

#endif
