#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "mpc.h"

#define ERR_BUF_LEN     512

#define min(a, b)       ((a) > (b) ? (b) : (a))
#define max(a, b)       ((a) > (b) ? (a) : (b))

#define LASSERT(arg, cond, fmt, ...)                        \
    if (!(cond)) {                                          \
        Lval_t* err = lval_create_err(fmt, ##__VA_ARGS__);  \
        lval_del(arg);                                      \
        return  err;                                        \
    }


struct Lval_t;
struct Lenv_t;
typedef struct Lval_t Lval_t;
typedef struct Lenv_t Lenv_t;

typedef Lval_t* (*Lbuiltin_t)(Lenv_t*, Lval_t*);

typedef enum {
    LVAL_INTEGER,
    LVAL_DECIMAL,
    LVAL_ERR,
    LVAL_SYM,
    LVAL_FN,
    LVAL_SEXPR,
    LVAL_QEXPR,
    LVAL_EXIT__,
} LVAL_e;

typedef union {
    long li;
    double f;
} Numeric_u;

// NOTE: might be better to use a hashmap here, gotta implement it though
struct Lenv_t {
    long count;
    char** syms;
    Lval_t** vals;
};

struct Lval_t {
    LVAL_e type;
    union {
        Numeric_u num;
        char* err;
        char* sym;          // symbol
        Lbuiltin_t fn;      // function
    };
    int count;
    struct Lval_t** cell;
};


Lval_t* lval_eval(Lenv_t* e, Lval_t* v);
Lval_t* lval_read(mpc_ast_t* ast);
void lval_del(Lval_t* v);
void lval_print(Lval_t* v);
void lval_println(Lval_t* v);

Lenv_t* lenv_new(void);
void lenv_del(Lenv_t* e);
void lenv_add_builtins(Lenv_t* e);
void _del_builtin_names(void);
