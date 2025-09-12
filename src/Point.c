#include <stdlib.h>
#include <printf.h>
#include "Point.h"

void say_hi(Point* this) {
    printf("Hi, I am %d/%d", this->x, this->y);
}

Point* Point__create(int x, int y) {
    Point* result = (Point*)malloc(sizeof(Point));
    result->x = x;
    result->y = y;
    result->say_hi = say_hi;
    return result;
}


char* Point___itoa(int x) {// test this
    int length = snprintf( NULL, 0, "%d", x);
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%d", x);
    return str;
}

char* Point__to_str(Point *self) {
    return Point___itoa(self->x);
}


void Point__destroy(Point* this) {
    if(this)
        free(this);
}
