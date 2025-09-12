#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

typedef struct {
	size_t dims;
	double *components;
} Vector;

double dot(Vector *a, Vector *b);
double norm2(Vector *a);

#endif