#include <stdio.h>

int add_2_ints(int a, int b) {
    return a + b;
}

int add_3_ints(int a, int b, int c) {
    return a + b + c;
}

float add_2_floats(float a, float b) {
    printf("(%f, %f)\n", a, b);
    return a + b;
}

float add_3_floats(float a, float b, float c) {
    return a + b + c;
}

double add_2_doubles(double a, double b) {
    return a + b;
}

double add_3_doubles(double a, double b, double c) {
    printf("(%f, %f, %f)\n", a, b, c);
    return a + b + c;
}

double add_int_float_double(int a, float b, double c) {
    printf("(%d, %f, %f)\n", a, b, c);
    return (double)a + (double)b + c;
}
