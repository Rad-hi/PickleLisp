#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "mpc.h"

#define min(a, b)  ((a) > (b) ? (b) : (a))
#define max(a, b)  ((a) > (b) ? (a) : (b))

#define LASSERT(arg, cond, err)  \
    if (!(cond)) { lval_del(arg); return lval_create_err(err); }


typedef enum {
    LVAL_INTEGER,
    LVAL_DECIMAL,
    LVAL_ERR,
    LVAL_SYM,
    LVAL_SEXPR,
    LVAL_QEXPR,
} LVAL_e;

typedef union {
    long li;
    double f;
} Numeric_u;

typedef struct Lval_t {
    LVAL_e type;
    Numeric_u num;
    union {
        char* err;
        char* sym;  // symbol
    };
    int count;
    struct Lval_t** cell;
} Lval_t;


Lval_t* eval_ast(mpc_ast_t *ast);
Lval_t* lval_eval(Lval_t* v);
Lval_t* lval_read(mpc_ast_t* ast);
void lval_del(Lval_t* v);
void lval_print(Lval_t* v);
void lval_println(Lval_t* v);
