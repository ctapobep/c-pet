#include <assert.h>
#include <stdbool.h>
#include "../log.h"
#include "asserts.h"
#include "../la/matrix.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

void assertEqualsDouble(double expected, double actual, double absError) {
	if (fabs(expected - actual) > absError) {
		logError("Test failed:");
		char errorMsg[1024];
		sprintf(errorMsg, "Expected: %f,\n  Actual: %f\n", expected, actual);
		logError(errorMsg);
		assert(false);
	}
}
void assertEqualsString(const char* expected, const char* actual) {
	if (strcmp(expected, actual) != 0) {
		logError("Test failed:");
		char errorMsg[strlen(expected) + strlen(actual) + 50];
		sprintf(errorMsg, "Strings are not equal:\nExpected (%lu): %s\n  Actual (%lu): %s\n", strlen(expected), expected, strlen(actual), actual);
		logError(errorMsg);
		assert(false);
	}
}
// void assertMatrixEquals(Matrix *expected, Matrix *actual, double absError) {
// 	size_t cols = Matrix_cols(expected);
// 	size_t rows = Matrix_rows(expected);
// 	if (cols != Matrix_cols(actual) || rows != Matrix_rows(actual)) {
// 		logError("Matrix dimensions were different:");
// 		char errorMsg[1024];
// 		sprintf(errorMsg, "Expected: %zux%zu\n  Actual: %zux%zu", cols, rows, Matrix_cols(actual), Matrix_rows(actual));
// 		logError(errorMsg);
// 		assert(false);
// 	}
// }
