interpreter: src/stack.c src/error.c src/interpreter.c src/alloc.c src/main.c
	gcc -o build/interpreter -ljson-c src/stack.c src/error.c src/interpreter.c src/alloc.c src/main.c

test: src/stack.c src/error.c src/interpreter.c src/alloc.c tests/test_interpreter.c
	gcc -g -o build/test_interpreter -ljson-c src/stack.c src/error.c src/interpreter.c src/alloc.c tests/test_interpreter.c

defaul: interpreter
