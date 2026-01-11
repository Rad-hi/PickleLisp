#pragma once

#define LANG_NAME       "PickleLisp"

#define READ_BUF_LEN    64              // initial read buffer length (dynamically doubles when exhausted)
#define ERR_BUF_LEN     512             // maximum allowed length of error message
#define REPL_IN         "8=> "
#define EXTENSION       ".pkl"          // pickle scripts extension
#define EUPSILON        1e-6            // precision of the equality assertion between doubles

// This macro is defined in the Makefile
#ifdef USE_MIMALLOC
    #include <mimalloc.h>

    #define PKL_MALLOC  mi_malloc
    #define PKL_FREE    mi_free
    #define PKL_REALLOC mi_realloc
#else    
    #define PKL_MALLOC  malloc
    #define PKL_FREE    free
    #define PKL_REALLOC realloc
#endif  // USE_MIMALLOC
