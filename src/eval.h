#pragma once
#include "mpc.h"

#define min(a, b)  ((a) > (b) ? (b) : (a))
#define max(a, b)  ((a) > (b) ? (a) : (b))

typedef enum {
  LVAL_INTEGER,
  LVAL_DECIMAL,
  LVAL_ERR
} LVAL_e;

typedef enum {
  LERR_DIV_ZERO,
  LERR_BAD_NUM,
  LERR_BAD_OP
} LERR_t;

typedef union {
  long li_num;
  double f_num;
} Numeric_u;

typedef struct {
  LVAL_e type;
  Numeric_u num;
  LERR_t err;
} Lval_t;

void lval_print(Lval_t v);
void lval_println(Lval_t v);
Lval_t eval_op(Lval_t x, char* op, Lval_t y);
Lval_t eval_ast(mpc_ast_t *ast);
