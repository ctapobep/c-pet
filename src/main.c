#include <stdio.h>
#include "Point.h"

int main1(void) {
    Point* p = Point__create(12, 1);
    p->say_hi(p);
    Point__destroy(p);
    return 0;
}
