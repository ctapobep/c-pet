typedef struct Point { // forward declared for encapsulation
    int x, y;
    void (*say_hi)(struct Point* this);
} Point;

typedef struct MyClass {
    int some_field;
} MyClass;

Point* Point__create(int x, int y);  // equivalent to "new Point(x, y)"
void Point__destroy(Point* this);  // equivalent to "delete point"
