CC = gcc
CFLAGS = -g -Wall -Werror -fsanitize=address

OBJ_DIR = object
INC_DIR = -I./test -I./vm -I./object

OBJ_CFILES = $(wildcard $(OBJ_DIR)/*.c)
OBJ_OFILES = $(patsubst $(OBJ_DIR)/%.c,$(OBJ_DIR)/%.o,$(OBJ_CFILES))

$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

vm/marshal.o: vm/marshal.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

test/test_marshal.o: test/test_marshal.c
	$(CC) $(CFLAGS) $(INC_DIR) -c $< -o $@

test/test_marshal: test/test_marshal.o vm/marshal.o $(OBJ_OFILES)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f object/*.o
	rm -f test/*.o
	rm -f test/test_marshal
	rm -f vm/*.o
