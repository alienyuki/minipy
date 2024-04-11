#include "marshal.h"
#include "object.h"
#include <stdio.h>

int main() {
    Object* code_obj = unmarshal_pyc("test/__pycache__/number.cpython-312.pyc");
    // unmarshal_pyc("test/__pycache__/eight_queens.cpython-312.pyc");

    object_print(1, code_obj);
    DECREF(code_obj);
    printf("\n");
}
