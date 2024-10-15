#pragma once
#include "mpc.h"

typedef enum { LVAL_NUM, LVAL_ERR } LVAL_e;
typedef enum { LERR_DIV_ZERO, LERR_BAD_NUM, LERR_BAD_OP } LERR_t;

typedef struct {
  LVAL_e type;
  double num;
  LERR_t err;
} Lval_t;


Lval_t lval_eval(double x, LVAL_e type);
void lval_print(Lval_t v);
void lval_println(Lval_t v);
Lval_t eval_op(Lval_t x, char* op, Lval_t y);
Lval_t eval_ast(mpc_ast_t *ast);
