#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../util/common.h"
#include "../util/strings.h"

typedef struct matrix_type {
	size_t cols, rows;
	double *data;
} Matrix;

double Matrix_get(const Matrix *m, size_t row, size_t col) {
	return m->data[row * m->cols + col];
}

double Matrix_set(const Matrix *m, size_t row, size_t col, double val) {
	return m->data[row * m->cols + col] = val;
}

size_t Matrix_cols(const Matrix *m) {
	return m->cols;
}
size_t Matrix_rows(const Matrix *m) {
	return m->rows;
}

static void* allocateMatrix(void) {
	void *m = malloc(sizeof(Matrix));
	if (m == NULL) {
		perror("Couldn't allocate memory for a Matrix");
		exit(COULD_NOT_ALLOCATE_ERROR);
	}
	return m;
}
static double* allocateZeroArray(size_t cols, size_t rows) {
	double* array = calloc(sizeof(double), rows * cols);
	if (array == NULL) {
		fprintf(stderr, "Couldn't allocate memory for a Matrix %lux%lu\n", rows, cols);
		exit(COULD_NOT_ALLOCATE_ERROR);
	}
	return array;
}

Matrix *Matrix_copy(size_t cols, size_t rows, double *data) {
	Matrix *m = Matrix_zeros(cols, rows);
	memcpy((void*)m->data, (void*)data, rows*cols*sizeof(double));
	return m;
}
Matrix *Matrix_create(size_t cols, size_t rows, double *data) {
	Matrix *m = allocateMatrix();
	m->rows = rows;
	m->cols = cols;
	m->data = data;
	return m;
}

Matrix *Matrix_zeros(size_t cols, size_t rows) {
	return Matrix_create(cols, rows, allocateZeroArray(cols, rows));
}
// Matrix *Matrix_diag(size_t rows, size_t cols, double val) {
// 	return NULL;
// }

void Matrix_fill(Matrix *m, double value) {
	for (size_t r = 0; r < m->rows; r++)
		for (size_t c = 0; c < m->cols; c++)
			MATRIXP_VAL(m, r, c) = value;
}

char* Matrix_toString(const Matrix *m) {
	char *result = calloc(1, 1);
	size_t len = 0;
	for (size_t r = 0; r < m->rows; r++) {
		for (size_t c = 0; c < m->cols; c++) {
			// size_t beforeDot = 6, afterDot = 2;
			// char symbol = 'f';
			// if (fabs(v) < 1e-9) {
			// 	beforeDot = 1;
			// 	afterDot = 0;
			// } else if (fabs(v - round(v) / v) < 1e3) {
			// 	 afterDot = 0;
			// }
			// char pattern[20];
			// int pos = 0;
			// pattern[pos++] = '%';
			// pos += sprintf(pattern+pos, "%lu", beforeDot);
			// pattern[pos++] = '.';
			// sprintf(pattern + pos++, "%lu", afterDot);
			// pattern[pos++] = symbol;
			// pattern[pos++] = '\0';
			// printf("Pattern: %s\n", pattern);
			double v = Matrix_get(m, r, c);
			char buffer[20];
			size_t appendLen;
			if (fabs(nearbyint(v) - v) < 1e-4)
				appendLen = sprintf(buffer, "%ld\t", lround(v));
			else
				appendLen = sprintf(buffer, "%.2f\t", v);
			result = strconcat(result, buffer, len, appendLen);
			len += appendLen;
		}
		char c = '\n';
		result = strconcat(result, &c, len++, 1);
	}
	return result;
}

void Matrix_destroy(Matrix *m) {
	free(m->data);
	free(m);
}
