# pragma once
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <ffi.h>

typedef enum {
    C_VOID,
    C_INT,
    C_DOUBLE,
    C_STRING,
    C_COLOR,

    N_TYPES,
} CTypes_e;

typedef struct {
    char r;
    char g;
    char b;
    char a;
} Color_t;


char* CTYPE_2_NAME[N_TYPES];

ffi_type* ctype_2_ffi_type(CTypes_e c_type);
char* ffi_type_2_str(ffi_type* t);