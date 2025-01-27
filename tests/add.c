/*
    This is a dummy library that we compile to an .so as part of the test procedure
    It has random funtionalities that aim at testing all the internals related to FFI
*/

#include <stdio.h>
#include <stdlib.h>


int add_2_ints(int a, int b) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_2_ints: (%d, %d)\n", a, b);
#endif // VERBOSE_ADD_
    return a + b;
}

int add_3_ints(int a, int b, int c) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_3_ints: (%d, %d, %d)\n", a, b, c);
#endif // VERBOSE_ADD_
    return a + b + c;
}

float add_2_floats(float a, float b) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_2_floats: (%f, %f)\n", a, b);
#endif // VERBOSE_ADD_
    return a + b;
}

float add_3_floats(float a, float b, float c) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_3_floats: (%f, %f, %f)\n", a, b, c);
#endif // VERBOSE_ADD_
    return a + b + c;
}

double add_2_doubles(double a, double b) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_2_doubles: (%f, %f)\n", a, b);
#endif // VERBOSE_ADD_
    return a + b;
}

double add_3_doubles(double a, double b, double c) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_3_doubles: (%f, %f, %f)\n", a, b, c);
#endif // VERBOSE_ADD_
    return a + b + c;
}

double add_int_float_double(int a, float b, double c) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_int_float_double: (%d, ", a);
    printf("%f, ", b);
    printf("%f)\n", c);
#endif // VERBOSE_ADD_
    return a + b + c;
}

typedef struct {
    int sum;
    int mod;
    int div;
} SumModDiv_t;

SumModDiv_t add_mod_div_int_int(int a, int b, int moddiv) {
    int sum_ = a + b;
    int mod_ = sum_ % moddiv;
    int div_ = sum_ / moddiv;
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_mod_div_int_int: (%d, %d, %d)\n", sum_, mod_, div_);
#endif // VERBOSE_ADD_
    return (SumModDiv_t){
        .sum = sum_,
        .mod = mod_,
        .div = div_,
    };
}

long add_2_longs(long a, long b) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_2_longs: (%li, %li)\n", a, b);
#endif // VERBOSE_ADD_
    return a + b;
}

char* add_2_longs_str(long a, long b) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_2_longs_str: (%li, %li)\n", a, b);
#endif // VERBOSE_ADD_
    char* buf = malloc(1024);
    snprintf(buf, 1024, "(%li + %li) = %li", a, b, a + b);
    return buf;
}


typedef struct{
    float x;
    float y;
} Vector2;


char* add_vector2_str(Vector2 v) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_2_longs_str: (%f, %f)\n", v.x, v.y);
#endif // VERBOSE_ADD_
    char* buf = malloc(1024);
    snprintf(buf, 1024, "(%.1f + %.1f) = %.1f", v.x, v.y, v.x + v.y);
    return buf;
}

Vector2 add_const_vector2(Vector2 v, float x) {
#ifdef VERBOSE_ADD_
    printf("[addlib]: add_const_vector2: (%f + %f, %f + %f)\n", v.x, x, v.y, x);
#endif // VERBOSE_ADD_
    return (Vector2){
        .x = v.x + x,
        .y = v.y + x,
    };
}
