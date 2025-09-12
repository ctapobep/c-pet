#include "vector.h"

#include <math.h>


//Vector Vector__new(double components[], size_t dims) {
//    Vector v = {.dims = dims, .components = components};// = malloc(sizeof(Vector));
//    return v;
//}

double dot(Vector *a, Vector *b) {
	double result = 0;
	for (size_t i = 0; i < a->dims; ++i)
		result += a->components[i] * b->components[i];
	return result;
}

double norm2(Vector *a) {
	double result = 0;
	for (size_t i = 0; i < a->dims; i++)
		result += a->components[i] * a->components[i];
	return sqrt(result);
}