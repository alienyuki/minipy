#ifndef OBJECT_H
#define OBJECT_H

typedef struct Object Object;
typedef struct TypeObject TypeObject;

typedef Object* (*str_func)(Object*);

struct Object {
    int refcnt;
    TypeObject* type;
};


struct TypeObject {
    char* name;
    str_func str;
};


void object_print(int fd, Object* o);

#endif
