CC = gcc
CFLAGS = -g -Wall -Werror -fsanitize=address -rdynamic
PY312 ?= ./python3.12

OBJ_DIR = object
INC_DIR = -I./test -I./vm -I./object

OBJ_CFILES = $(wildcard $(OBJ_DIR)/*.c)
OBJ_OFILES = $(patsubst $(OBJ_DIR)/%.c,$(OBJ_DIR)/%.o,$(OBJ_CFILES))

$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

.PHONY: default clean

default: test/test_vm update_py
	./test/test_vm $(PYTARGET)

TEST_PYS  = $(wildcard test/*.py)
TEST_PYCS = $(patsubst test/%.py,test/__pycache__/%.cpython-312.pyc,$(TEST_PYS))

update_py: $(TEST_PYCS)
	@echo "Updated"

test/__pycache__/%.cpython-312.pyc: test/%.py
	$(PY312) -m compileall $<

vm/marshal.o: vm/marshal.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

vm/vm.o: vm/vm.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

vm/debugger.o: vm/debugger.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

vm/gc.o: vm/gc.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

vm/pdb.o: vm/pdb.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

test/test_marshal.o: test/test_marshal.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

test/test_vm.o: test/test_vm.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

test/test_dict.o: test/test_dict.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

test/test_marshal: test/test_marshal.o vm/marshal.o vm/debugger.o vm/gc.o $(OBJ_OFILES)
	$(CC) $(CFLAGS) $^ -o $@

test/test_vm: test/test_vm.o vm/marshal.o vm/vm.o vm/debugger.o vm/gc.o vm/pdb.o $(OBJ_OFILES)
	$(CC) $(CFLAGS) $^ -o $@

test/test_dict: test/test_dict.o vm/debugger.o $(OBJ_OFILES)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f object/*.o
	rm -f test/*.o
	rm -f test/test_marshal
	rm -f test/test_vm
	rm -f test/test_dict
	rm -f vm/*.o
	rm -rf test/__pycache__
