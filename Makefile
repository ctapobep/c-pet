# Docs on Make:
#
# - If target name corresponds to a file, it won't be run if the file exists and the dependencies haven't changed
# - $@ is the target name
# - $? is all the prerequisites newer than the target
# - $^ all prerequisites
# - $< the 1st prerequisite

DSTDIR := build
#CFLAGS = -Wall -g
CFLAGS := -Wall -Werror -Wextra -g -O0
LIBS := -lm

DBGDIR := debug
DBGCFLAGS := -g -O0 -DDEBUG

RELDIR := release
RELCFLAGS := -O3 -DNDEBUG

UTILS = strings.o asserts.o log.o
UTILS_TESTS = strings_test.o
LA_TESTS = vector_test.o matrix_test.o
LA = vector.o matrix.o
ALL_OBJECTS = $(UTILS) $(UTILS_TESTS) $(LA)

all:

test: strings_test vector_test matrix_test

clean:
	rm -f build/*
vector_test: vector_test.o $(LA) $(UTILS)
	$(CC) $(CFLAGS) -o build/$@ $(foreach obj, $(UTILS) $(LA), build/$(obj)) build/$@.o $(LIBS)
	build/vector_test
matrix_test: matrix_test.o $(LA) $(UTILS)
	$(CC) $(CFLAGS) -o build/$@ $(foreach obj, $(UTILS) $(LA), build/$(obj)) build/$@.o $(LIBS)
	build/matrix_test
strings_test: strings_test.o $(UTILS)
	$(CC) $(CFLAGS) -o build/$@ $(foreach obj, $(UTILS), build/$(obj)) build/$@.o $(LIBS)
	build/strings_test

#matrix_test.o: src/util/asserts.h src/util/test_util.h src/la/matrix.h src/la/matrix_test.c
#	$(CC) $(CFLAGS) -c src/la/matrix_test.c -o build/matrix_test.o
#strings_test.o: src/util/strings_test.c strings.o asserts.o
	#$(CC) $(CFLAGS) -c src/util/strings_test.c -o build/strings_test.o
#matrix.o: src/la/vector.h src/la/matrix.c src/la/matrix.h
	#$(CC) $(CFLAGS) -c src/la/matrix.c -o build/matrix.o
#vector.o: src/la/vector.c src/la/vector.h
	#$(CC) $(CFLAGS) -c src/la/vector.c -o build/vector.o

#log.o: src/log.c src/log.h
#     $< is the 1st prerequisite (src/log.c), $@ is the taget name (log.o)
#	$(CC) $(CFLAGS) -c $< -o $(DSTDIR)/$@

$(LA): %.o: src/la/%.c src/la/*.h src/util/xterm.h
	$(CC) -c $< -o build/$@
$(LA_TESTS): %.o: src/la/%.c src/la/*.h src/util/xterm.h
	$(CC) -c $< -o build/$@
# For each .o in UTILS it generates a target:
# target.o: src/util/target.c src/util/xterm.h log.o
# 	cc -c src/util/target.c -o target.o
$(UTILS): %.o: src/util/%.c src/util/*.h
	$(CC) -c $< -o build/$@
$(UTILS_TESTS): %.o: src/util/%.c src/util/*.h
	$(CC) -c $< -o build/$@
#asserts.o: src/util/asserts.c src/util/asserts.h src/util/xterm.h log.o
#	$(CC) $(CFLAGS) -c src/util/asserts.c -o build/asserts.o
#strings.o: src/util/strings_test.c src/util/strings.h
#	$(CC) $(CFLAGS) -c src/util/strings.c -o build/strings.o

prep:
	@mkdir -p $(DBGDIR) $(RELDIR)