# pragma once
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ffi.h>

#include "config.h"

typedef enum {
    C_VOID,
    C_CHAR,
    C_INT,
    C_FLOAT,
    C_DOUBLE,
    C_STRING,
    C_STRUCT,
} CTypes_e;

char* ctype_2_str(CTypes_e c_type);
ffi_type* ctype_2_ffi_type(CTypes_e c_type);
char* ffi_type_2_str(ffi_type* t);
ffi_type* ffi_type_from_user_defined(CTypes_e* ctypes, int count);
size_t sizeof_ctype(CTypes_e ctype);
