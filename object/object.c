#include "object.h"
#include "str_object.h"

#include <unistd.h>

void object_print(int fd, Object* o) {
    StrObject* s = (StrObject*) o->type->str(o);

    write(fd, s->str, s->size);
}
