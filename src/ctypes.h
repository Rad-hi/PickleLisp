# pragma once
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ffi.h>

typedef enum {
    C_VOID,
    C_CHAR,
    C_INT,
    C_DOUBLE,
    C_STRING,
    C_STRUCT,

    N_TYPES,
} CTypes_e;

char* CTYPE_2_NAME[N_TYPES];

ffi_type* ctype_2_ffi_type(CTypes_e c_type);
char* ffi_type_2_str(ffi_type* t);
ffi_type* ffi_type_from_user_defined(CTypes_e* ctypes, int count);
size_t sizeof_ctype(CTypes_e ctype);
