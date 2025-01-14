#include "ctypes.h"

PUBLIC char* ctype_2_str(CTypes_e c_type) {
    switch (c_type) {
        case C_VOID: return "C_VOID";
        case C_CHAR: return "C_CHAR";
        case C_INT: return "C_INT";
        case C_FLOAT: return "C_FLOAT";
        case C_DOUBLE: return "C_DOUBLE";
        case C_STRING: return "C_STRING";
        case C_STRUCT: return "C_STRUCT";
    }

    fprintf(stderr, "I shouldn't be here %s!\n", __func__);
    exit(69);
};

PUBLIC ffi_type* ctype_2_ffi_type(CTypes_e c_type) {
    switch (c_type) {
        case C_VOID:   return &ffi_type_void;
        case C_CHAR:   return &ffi_type_schar;
        case C_INT:    return &ffi_type_slong;
        case C_FLOAT: return &ffi_type_float;
        case C_DOUBLE: return &ffi_type_double;
        case C_STRING: return &ffi_type_pointer;
        default:
            fprintf(stderr, "You're asking for an FFI equivalent to a ctype that's not handled, "
                            "Or is user defined. %s!\n", __func__);
            assert(false);
    }
}

PUBLIC char* ffi_type_2_str(ffi_type* t) {
    if (t == &ffi_type_void)        return "ffi_type_void";
    if (t == &ffi_type_schar)       return "ffi_type_schar";
    if (t == &ffi_type_slong)       return "ffi_type_slong";
    if (t == &ffi_type_float)       return "ffi_type_float";
    if (t == &ffi_type_double)      return "ffi_type_double";
    if (t == &ffi_type_pointer)     return "ffi_type_pointer";
    if (t->type == FFI_TYPE_STRUCT) return "ffi_user_defined_type [struct]";
    return "unknown";
}

PUBLIC ffi_type* ffi_type_from_user_defined(CTypes_e* ctypes, int count) {
    // Ref: https://eli.thegreenplace.net/2013/03/04/flexible-runtime-interface-to-shared-libraries-with-libffi

    int n_types = count;
    ffi_type** elements = malloc((n_types + 1) * sizeof(ffi_type*));
    for (int i = 0; i < n_types; ++i) {
        elements[i] = ctype_2_ffi_type(ctypes[i]);
    }
    elements[n_types] = NULL;

    ffi_type *type = malloc(sizeof(ffi_type));
    type->size = 0;
    type->alignment = 0;
    type->type = FFI_TYPE_STRUCT;
    type->elements = elements;

    return type;
}

PUBLIC size_t sizeof_ctype(CTypes_e ctype) {
    switch (ctype) {
        case C_INT: return sizeof(long);
        case C_CHAR: return sizeof(char);
        case C_DOUBLE: return sizeof(double);
        case C_FLOAT: return sizeof(float);
        case C_STRING: return sizeof(char*);
        case C_VOID: fprintf(stderr, "WHAT THE FUCK IS A VOID DOING AS INPUT?"); exit(69);
        default:
            fprintf(stderr, "You added a new C-type [%s], but forgot to add it to %s!\n", ctype_2_str(ctype), __func__);
            assert(false);
    }
}