#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "mpc.h"

#define ERR_BUF_LEN     512

#define min(a, b)       ((a) > (b) ? (b) : (a))
#define max(a, b)       ((a) > (b) ? (a) : (b))

#define IS_NUM(a, idx)                    \
    (a->cell[idx]->type == LVAL_INTEGER   \
    || a->cell[idx]->type == LVAL_DECIMAL \
    || a->cell[idx]->type == LVAL_BOOL)

#define IS_ITERABLE(a, idx) (a->cell[idx]->type == LVAL_QEXPR \
                          || a->cell[idx]->type == LVAL_STR)

#define LASSERT(arg, cond, fmt, ...)                        \
    if (!(cond)) {                                          \
        Lval_t* err = lval_create_err(fmt, ##__VA_ARGS__);  \
        lval_del(arg);                                      \
        return  err;                                        \
    }

#define LASSERT_TYPE(fn, arg, idx, expect)                             \
  LASSERT(arg, arg->cell[idx]->type == expect,                         \
    "Function `%s` expects arg of type %s. Arg [%i] is of type %s.",   \
    fn, ltype_name(expect), idx + 1, ltype_name(arg->cell[idx]->type))

#define LASSERT_NUM(fn, arg, num)                                          \
  LASSERT(arg, arg->count == num,                                          \
    "Function `%s` expects [%i] arguments, got [%i]", fn, num, arg->count)

/* the language defined in grammar.h */
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
    LVAL_EXIT,
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
    LVAL_e type;

    /* Lval_t can only represent one at a time */
    union {
        Numeric_u num;
        char* str;
        char* err;
        char* sym;
        Lbuiltin_t builtin;
    };

    /* Functions' stuff (along with builtin) */
    Lenv_t* env;
    Lval_t* formals;
    Lval_t* body;

    /* Expression */
    int count;
    struct Lval_t** cell;
};


Lval_t* lval_eval(Lenv_t* e, Lval_t* v);
Lval_t* lval_read(mpc_ast_t* ast);
Lenv_t* lenv_new(void);
Lval_t* lval_add(Lval_t* v, Lval_t* x);
Lval_t* lval_create_sexpr(void);
Lval_t* lval_create_str(char* s);
Lval_t* builtin_load(Lenv_t* e, Lval_t* a);
void    lval_del(Lval_t* v);
void    lval_print(Lval_t* v);
void    lval_println(Lval_t* v);
void    lenv_del(Lenv_t* e);
void    lenv_add_builtins(Lenv_t* e);
void    _del_builtin_names(void);
