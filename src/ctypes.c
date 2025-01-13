#include "ctypes.h"

// NOTE: keep the same order as the definition in `CTypes_e`
char* CTYPE_2_NAME[N_TYPES] = {
    "C_VOID",
    "C_INT",
    "C_DOUBLE",
    "C_STRING",
    "C_COLOR",  // TODO: remove
    "C_STRUCT",
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
        case C_COLOR:  return &ffi_color_type;  // TODO: remove
        default:
            fprintf(stderr, "You're asking for an FFI equivalent to a ctype that's not handled %s!\n", __func__);
            assert(false);
    }
}

char* ffi_type_2_str(ffi_type* t) {
    if (t == &ffi_type_void)        return "ffi_type_void";
    if (t == &ffi_type_slong)       return "ffi_type_slong";
    if (t == &ffi_type_double)      return "ffi_type_double";
    if (t == &ffi_type_pointer)     return "ffi_type_pointer";
    if (t->type == FFI_TYPE_STRUCT) return "ffi_user_defined_type [struct]";
    return "unknown";
}

ffi_type* ffi_type_from_user_defined(CTypes_e* ctypes, int count) {
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
