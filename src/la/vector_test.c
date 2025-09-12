#include "vector.h"
#include "../log.h"
#include "../util/test_util.h"

#include "../util/asserts.h"
#include <math.h>
#include "../util/signals.c"

void test_dot(void) {
	Vector v = {.dims = 3, .components = (double[]){1, 2, 3}};
	assertEqualsDouble(14, dot(&v, &v), 0);
	assertEqualsDouble(-14, dot(&v, &(Vector){.dims = 5, .components = (double[]){-1, -2, -3}}), 0);
	assertEqualsDouble(0, dot(&v, &(Vector){.dims = 3, .components = (double[]){0, 0, 0}}), 0);
}

void test_norm(void) {
	assertEqualsDouble(0, norm2(&(Vector){.dims = 0, .components = (double[0]){}}), 0);
	assertEqualsDouble(0, norm2(&(Vector){.dims = 1, .components = (double[]){0}}), 0);
	assertEqualsDouble(0, norm2(&(Vector){.dims = 2, .components = (double[]){0, 0}}), 0);

	logInfo("  Testing norm2=1 for a unit vector");
	assertEqualsDouble(1, norm2(&(Vector){.dims = 1, .components = (double[]){1}}), 1e-9);
	assertEqualsDouble(1, norm2(&(Vector){.dims = 2, .components = (double[]){1/sqrt(2), -1/sqrt(2)}}), 1e-9);

	logInfo("  Testing norm2>=1 for a larger vector");
	assertEqualsDouble(2, norm2(&(Vector){.dims = 1, .components = (double[]){2}}), 1e-9);
	assertEqualsDouble(sqrt(2), norm2(&(Vector){.dims = 2, .components = (double[]){1, -1, 0}}), 1e-9);
}

int main(void) {
	register_signals();
	logInfo("[INFO] Testing Vector..");
	RUN_TEST(test_dot);
	RUN_TEST(test_norm);
}
