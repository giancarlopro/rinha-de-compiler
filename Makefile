ifndef CFLAGS
	CFLAGS=-I/vcpkg/installed/x64-linux/include
	LDFLAGS=-L/vcpkg/installed/x64-linux/lib
endif

interpreter: src/stack.c src/error.c src/interpreter.c src/alloc.c src/main.c
	gcc $(CFLAGS) $(LDFLAGS) -o build/interpreter src/stack.c src/error.c src/interpreter.c src/alloc.c src/main.c -ljson-c

test: src/stack.c src/error.c src/interpreter.c src/alloc.c tests/test_interpreter.c
	gcc $(CFLAGS) $(LDFLAGS) -g -o build/test_interpreter src/stack.c src/error.c src/interpreter.c src/alloc.c tests/test_interpreter.c -ljson-c

all: interpreter test
default: interpreter
