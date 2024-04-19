#ifndef OBJECT_H
#define OBJECT_H

#include <stdint.h>

typedef struct Object Object;
typedef struct TypeObject TypeObject;
typedef uint64_t hash_t;

typedef Object* (*str_func)(Object*);
typedef void (*destr_func)(Object*);
typedef hash_t (*hash_func)(Object*);

struct Object {
    int refcnt;
    TypeObject* type;
};


struct TypeObject {
    char* name;
    str_func str;
    destr_func destr;
    hash_func hash;
};


void object_print(int fd, Object* o);
hash_t object_hash(Object* o);

#define IMMORTAL_REF (1 << 30)

void inc_ref(Object* o);
void dec_ref(Object* o);

#define INCREF(o) inc_ref((Object*) o)
#define DECREF(o) dec_ref((Object*) o)

#endif
