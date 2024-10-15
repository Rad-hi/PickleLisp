#include <stdio.h>
#include <stdbool.h>
#include "err.h"

lval_t lval_eval(long x, bool err) {
  lval_t v;
  if (err) {
    v.err = x;
    v.type = LVAL_ERR;
  } else {
    v.num = x;
    v.type = LVAL_NUM;
  }
  return v;
}

void lval_print(lval_t v) {
  switch (v.type) {
    case LVAL_NUM: printf("%li", v.num); break;
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