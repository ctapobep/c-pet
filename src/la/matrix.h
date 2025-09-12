#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

typedef struct matrix_type Matrix;

#define MATRIX_VAL(matrix, r, c) (matrix).data[(r) * (m).cols + (c)]
#define MATRIXP_VAL(matrix, r, c) (matrix)->data[(r) * (m)->cols + (c)]

double Matrix_get(const Matrix* m, size_t row, size_t col);
double Matrix_set(const Matrix* m, size_t row, size_t col, double val);
size_t Matrix_cols(const Matrix *m);
size_t Matrix_rows(const Matrix *m);

Matrix *Matrix_copy(size_t cols, size_t rows, double *data);
Matrix *Matrix_create(size_t cols, size_t rows, double *data);
Matrix* Matrix_zeros(size_t cols, size_t rows);
Matrix *Matrix_diag(size_t rows, size_t cols, double val);
void Matrix_destroy(Matrix *m);

void Matrix_fill(Matrix *m, double value);
char* Matrix_toString(const Matrix *m);
#endif
