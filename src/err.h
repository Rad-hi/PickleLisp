#pragma once

typedef enum { LVAL_NUM, LVAL_ERR } LVAL_e;
typedef enum { LERR_DIV_ZERO, LERR_BAD_NUM, LERR_BAD_OP } LERR_t;

typedef struct {
  LVAL_e type;
  long num;
  LERR_t err;
} lval_t;

lval_t lval_eval(long x, LVAL_e type);
void lval_print(lval_t v);
void lval_println(lval_t v);