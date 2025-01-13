#include "ctypes.h"

// NOTE: keep the same order as the definition in `CTypes_e`
char* CTYPE_2_NAME[N_TYPES] = {
    "C_VOID",
    "C_INT",
    "C_DOUBLE",
    "C_STRING",
    "C_COLOR",
};

// Ref: https://eli.thegreenplace.net/2013/03/04/flexible-runtime-interface-to-shared-libraries-with-libffi
ffi_type* color_elements[5] = {&ffi_type_schar, &ffi_type_schar, &ffi_type_schar, &ffi_type_schar, NULL};
ffi_type ffi_color_type = {.size=0, .alignment=0, .type=FFI_TYPE_STRUCT, .elements=color_elements};


ffi_type* ctype_2_ffi_type(CTypes_e c_type) {
    switch (c_type) {
        case C_VOID:   return &ffi_type_void;
        case C_INT:    return &ffi_type_slong;
        case C_DOUBLE: return &ffi_type_double;
        case C_STRING: return &ffi_type_pointer;
        case C_COLOR:  return &ffi_color_type;
        default:
            fprintf(stderr, "You're asking for an FFI equivalent to a ctype that's not handled %s!\n", __func__);
            assert(false);
    }
}

char* ffi_type_2_str(ffi_type* t) {
    if (t == &ffi_type_void)    return "ffi_type_void";
    if (t == &ffi_type_slong)   return "ffi_type_slong";
    if (t == &ffi_type_double)  return "ffi_type_double";
    if (t == &ffi_type_pointer) return "ffi_type_pointer";
    if (t == &ffi_color_type)   return "ffi_color_type";
    return "unknown";
}
