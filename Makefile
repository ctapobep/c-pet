#CFLAGS = -Wall -g
CFLAGS := -Wall -Werror -Wextra -g -O0

DBGDIR := debug
DBGCFLAGS := -g -O0 -DDEBUG

RELDIR := release
RELCFLAGS := -O3 -DNDEBUG

ALL_OBJECTS = $(wildcard build/*.o)

all: vector_test
test: strings_test vector_test matrix_test
	build/strings_test
	build/vector_test
	build/matrix_test
clean:
	rm build/*
vector_test: vector_test.o asserts.o vector.o
	$(CC) $(CFLAGS) -o build/vector_test $(filter-out $(wildcard build/*_test.o), $(ALL_OBJECTS)) build/vector_test.o
vector_test.o: src/util/asserts.h src/la/vector.h src/la/vector_test.c src/util/test_util.h
	$(CC) $(CFLAGS) -c src/la/vector_test.c -o build/vector_test.o

matrix_test: matrix.o asserts.o matrix_test.o
	$(CC) $(CFLAGS) -o build/matrix_test $(filter-out $(wildcard build/*_test.o), $(ALL_OBJECTS)) build/matrix_test.o
strings_test: strings_test.o
	$(CC) $(CFLGAGS) -o build/strings_test $(filter-out $(wildcard build/*_test.o), $(ALL_OBJECTS)) build/strings_test.o
matrix_test.o: src/util/asserts.h src/util/test_util.h src/la/matrix.h src/la/matrix_test.c
	$(CC) $(CFLAGS) -c src/la/matrix_test.c -o build/matrix_test.o
strings_test.o: src/util/strings_test.c strings.o asserts.o
	$(CC) $(CFLAGS) -c src/util/strings_test.c -o build/strings_test.o
matrix.o: src/la/vector.h src/la/matrix.c src/la/matrix.h
	$(CC) $(CFLAGS) -c src/la/matrix.c -o build/matrix.o
vector.o: src/la/vector.c src/la/vector.h
	$(CC) $(CFLAGS) -c src/la/vector.c -o build/vector.o
asserts.o: src/util/asserts.c src/util/asserts.h src/util/xterm.h log.o
	$(CC) $(CFLAGS) -c src/util/asserts.c -o build/asserts.o
strings.o: src/util/strings_test.c src/util/strings.h
	$(CC) $(CFLAGS) -c src/util/strings.c -o build/strings.o
log.o: src/log.c src/log.h
	$(CC) $(CFLAGS) -c src/log.c -o build/log.o

prep:
	@mkdir -p $(DBGDIR) $(RELDIR)