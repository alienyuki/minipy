#include "marshal.h"
#include "object.h"
#include "code_object.h"
#include "vm.h"
#include <stdio.h>

int main() {
    Object* code_obj = unmarshal_pyc("test/__pycache__/arithmetic.cpython-312.pyc");
    // Object* code_obj = unmarshal_pyc("test/__pycache__/perfect_number.cpython-312.pyc");
    // Object* code_obj = unmarshal_pyc("test/__pycache__/func_2.cpython-312.pyc");
    // Object* code_obj = unmarshal_pyc("test/__pycache__/func.cpython-312.pyc");
    // Object* code_obj = unmarshal_pyc("test/__pycache__/while.cpython-312.pyc");
    // Object* code_obj = unmarshal_pyc("test/__pycache__/if.cpython-312.pyc");
    // Object* code_obj = unmarshal_pyc("test/__pycache__/add.cpython-312.pyc");
    // Object* code_obj = unmarshal_pyc("test/__pycache__/number.cpython-312.pyc");

    printf("\n\n-------- vm start --------\n\n");

    pvm* vm = vm_init();
    pvm_run(vm, (CodeObject*) code_obj);
    DECREF(code_obj);
    vm_destroy(vm);
    printf("\n\n--------  vm end  --------\n\n");
    return 0;
}
