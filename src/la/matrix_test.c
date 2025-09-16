#include "matrix.h"

#include "../util/test_util.h"
#include "../util/asserts.h"
#include "../util/log.h"
#include "../util/signals.c"

static void matrix_get_set__allow_getting_n_setting_matrix_elements() {
	Matrix *m = Matrix_copy(2, 2, (double[]){
		                        1, 2,
		                        3, 4
	                        });
	assertEqualsDouble(1, Matrix_get(m, 0, 0), 0);
	assertEqualsDouble(2, Matrix_get(m, 0, 1), 0);
	assertEqualsDouble(3, Matrix_get(m, 1, 0), 0);
	assertEqualsDouble(4, Matrix_get(m, 1, 1), 0);

	Matrix_set(m, 0, 1, 13);
	assertEqualsDouble(13, Matrix_get(m, 0, 1), 0);
	Matrix_destroy(m);
}

static void matrixZeros_fillsItWithZeros() {
	Matrix* m = Matrix_zeros(3, 2);
	assertEqualsDouble(0, Matrix_get(m, 0, 0), 0);
	assertEqualsDouble(0, Matrix_get(m, 1, 0), 0);
	assertEqualsDouble(0, Matrix_get(m, 0, 2), 0);
	Matrix_destroy(m);
}

static void toString() {
	Matrix *m = Matrix_copy(4, 2, (double[]){0, 1, .1, -.1, 1.1234, 1e4, 1e-8, 1e-12});
	char *str = Matrix_toString(m);
	puts(str);
	assertEqualsString("0\t1\t0.10\t-0.10\t\n1.12\t10000\t0\t0\t\n", str);
	free(str);
	Matrix_destroy(m);
}

static void matrixDiag_hasValuesOnlyOnDiagonal() {
	// Matrix *m = Matrix_diag(2, 3, 2);
	// assertEqualsDouble(2, Matrix_get(m, 0, 0), 0);
	// assertEqualsDouble(2, Matrix_get(m, 1, 1), 0);

	// assertEqualsDouble(0, Matrix_get(m, 1, 1), 0);
}


int main(void) {
	logInfo("Matrix Tests");
	register_signals();
	RUN_TEST(matrix_get_set__allow_getting_n_setting_matrix_elements);
	RUN_TEST(matrixZeros_fillsItWithZeros);
	RUN_TEST(toString);
	RUN_TEST(matrixDiag_hasValuesOnlyOnDiagonal);
}
