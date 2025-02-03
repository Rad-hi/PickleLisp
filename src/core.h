#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <dlfcn.h>
#include <ffi.h>

#include "config.h"
#include "ctypes.h"
#include "mpc.h"

#define min(a, b) ((a) > (b) ? (b) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define almost_eq(a, b) (fabs((a) - (b)) <= (EUPSILON))

#define IS_NUM(a, idx)                      \
   ((a)->cell[(idx)]->type == LVAL_INTEGER  \
 || (a)->cell[(idx)]->type == LVAL_DECIMAL  \
 || (a)->cell[(idx)]->type == LVAL_BOOL)

#define IS_ITERABLE(a, idx)                \
   ((a)->cell[(idx)]->type == LVAL_QEXPR   \
 || (a)->cell[(idx)]->type == LVAL_STR)

#define LASSERT(arg, cond, fmt, ...) do{                    \
    if (!(cond)) {                                          \
        Lval_t* err = lval_create_err(fmt, ##__VA_ARGS__);  \
        lval_del(arg);                                      \
        return  err;                                        \
    }                                                       \
} while(0)

#define LASSERT_TYPE(fn, arg, idx, expect)                                   \
  LASSERT(arg, (arg)->cell[(idx)]->type == expect,                           \
    "Function `%s` expects arg of type %s. Arg [%i] is of type %s.",         \
    fn, ltype_name(expect), (idx) + 1, ltype_name((arg)->cell[(idx)]->type))

#define LASSERT_NUM(fn, arg, num)                                                \
  LASSERT((arg), (arg)->count == (num),                                          \
    "Function `%s` expects [%i] arguments, got [%i]", (fn), (num), (arg)->count)

/* the language defined in lang.h */
extern mpc_parser_t* pickle_lisp;

struct Lval_t;
struct Lenv_t;
typedef struct Lval_t Lval_t;
typedef struct Lenv_t Lenv_t;

typedef Lval_t* (*Lbuiltin_t)(Lenv_t*, Lval_t*);

typedef enum {
    LVAL_INTEGER,
    LVAL_DECIMAL,
    LVAL_BOOL,
    LVAL_STR,
    LVAL_ERR,
    LVAL_SYM,
    LVAL_FN,
    LVAL_SEXPR,
    LVAL_QEXPR,
    LVAL_DLL,
    LVAL_TYPE,
    LVAL_EXIT,
    LVAL_OK,
    LVAL_USER_TYPE,
} LVAL_e;

typedef union {
    long li;
    double f;
} Numeric_u;

// NOTE: might be better to use a hashmap here, gotta implement it though
struct Lenv_t {
    Lenv_t* parent;
    int count;
    char** syms;
    Lval_t** vals;
};

struct Lval_t {
    // char* name;  // TODO: add the name `symbol` of anything registered
    LVAL_e type;
    CTypes_e c_type;

    /* Lval_t can only represent one at a time */
    union {
        Numeric_u num;
        char* str;
        char* err;
        char* sym;
        Lbuiltin_t builtin;
        void* dll;
        ffi_type* ud_ffi_t;  // describes a user-defined ffi_type [a struct]
    };

    /* Functions' stuff (along with builtin) */
    Lenv_t* env;
    Lval_t* formals;  // used to define a function's input variables (fn), and signature (extern)
    Lval_t* body;  // used to contain the function's body (fn), and return type (extern)

    /* libffi and extern function linking stuff (along with dll) */
    ffi_cif* cif;
    ffi_type** atypes;
    bool is_extern;
    void* extern_ptr;

    size_t ud_ffi_sz;  // the size of the entire user-defined type

    /* Expression */
    int count;
    struct Lval_t** cell;
};


Lval_t* lval_eval(Lenv_t* e, Lval_t* v);
Lval_t* lval_read(mpc_ast_t* ast);
Lenv_t* lenv_new(void);
Lval_t* lval_add(Lval_t* v, Lval_t* x);
Lval_t* lval_create_sexpr(void);
Lval_t* lval_create_qexpr(void);
Lval_t* lval_create_str(char* s);
Lval_t* builtin_load(Lenv_t* e, Lval_t* a);
void    lval_del(Lval_t* v);
void    lval_print(Lval_t* v);
void    lval_println(Lval_t* v);
void    lenv_del(Lenv_t* e);
void    lenv_add_builtins(Lenv_t* e);
void    _register_builtin_names_from_env(Lenv_t* e);
void    _del_builtin_names(void);
