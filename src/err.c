#include <stdio.h>
#include <stdbool.h>
#include "err.h"

lval_t lval_eval(double x, LVAL_e type) {
  lval_t v;
  switch (type) {
    case LVAL_ERR: 
      v.err = x;
      v.type = LVAL_ERR;
      break;
    case LVAL_NUM:
      v.num = x;
      v.type = LVAL_NUM;
      break;
  }
  return v;
}

void lval_print(lval_t v) {
  switch (v.type) {
    case LVAL_NUM: printf("%f", v.num); break;
    case LVAL_ERR:
      switch (v.err) {
        case LERR_BAD_OP: printf("ERROR: Invalid operator!"); break;
        case LERR_BAD_NUM: printf("ERROR: Invalid number!"); break;
        case LERR_DIV_ZERO: printf("ERROR: Division by Zero!"); break;
      }
      break;
  }
}

void lval_println(lval_t v) { lval_print(v); printf("\n"); }