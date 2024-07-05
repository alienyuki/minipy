#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>

typedef struct Object Object;
typedef struct TypeObject TypeObject;
typedef uint64_t hash_t;

typedef enum {
    CMP_LT,
    CMP_LE,
    CMP_EQ,
    CMP_NE,
    CMP_GT,
    CMP_GE,
} cmp_op;


typedef Object* (*binary_op_func)(Object*, Object*);

typedef Object* (*str_func)(Object*);
typedef void (*destr_func)(Object*);
typedef hash_t (*hash_func)(Object*);
typedef Object* (*compare_func)(Object*, Object*, cmp_op);
typedef Object* (*getattr_func)(Object*, Object*);
typedef int (*tr_visit)(Object*, void*);
typedef int (*traverse_func)(Object*, tr_visit, void*);

typedef struct {
    binary_op_func add_func;
    binary_op_func fdiv_func;
    binary_op_func mul_func;
    binary_op_func remainder_func;
    binary_op_func sub_func;
    binary_op_func iadd_func;
} number_methods;

typedef struct {
    Object* (*get_sub)(Object*, int);
    int     (*set_sub)(Object*, int, Object*);
} seq_methods;

typedef struct {

} map_methods;


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
    getattr_func get_attr;
    traverse_func traverse;

    number_methods* num;
    seq_methods* seq;
    map_methods* map;

    Object* dict;
};


void object_print(int fd, Object* o);
hash_t object_hash(Object* o);
Object* object_compare(Object* o1, Object* o2, cmp_op op);

Object* object_get_attr(Object* owner, Object* name);

Object* object_get_item(Object* container, Object* sub);
int object_set_item(Object* container, Object* sub, Object* v);

#define IMMORTAL_REF (1 << 30)

void inc_ref(Object* o);
void dec_ref(Object* o);

#define INCREF(o) inc_ref((Object*) o)
#define DECREF(o) dec_ref((Object*) o)

Object* object_binary_add(Object* o1, Object* o2);
Object* object_binary_mul(Object* o1, Object* o2);
Object* object_binary_sub(Object* o1, Object* o2);
Object* object_binary_fdiv(Object* o1, Object* o2);
Object* object_binary_remainder(Object* o1, Object* o2);
Object* object_binary_iadd(Object* o1, Object* o2);

#endif
