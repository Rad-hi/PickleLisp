#include <stdio.h>
#include <stdbool.h>
#include "eval.h"

static Lval_t lval_err(LERR_t x) {
  return (Lval_t){
    .type = LVAL_ERR,
    .err = x
  };
}

static Lval_t lval_long(long x) {
  return (Lval_t){
    .type = LVAL_INTEGER,
    .num.li_num = x
  };
}

static Lval_t lval_double(double x) {
  return (Lval_t){
    .type = LVAL_DECIMAL,
    .num.f_num = x
  };
}

void lval_print(Lval_t v) {
  switch (v.type) {
    case LVAL_INTEGER: printf("%li", v.num.li_num); break;
    case LVAL_DECIMAL: printf("%f", v.num.f_num); break;
    case LVAL_ERR:
      switch (v.err) {
        case LERR_BAD_OP: printf("ERROR: Unknown operator!"); break;
        case LERR_BAD_NUM: printf("ERROR: Invalid input number!"); break;
        case LERR_DIV_ZERO: printf("ERROR: Division by Zero!"); break;
      }
      break;
  }
}

void lval_println(Lval_t v) {
  lval_print(v); printf("\n"); 
}

Lval_t eval_op(Lval_t x, char* op, Lval_t y) {
  if (x.type == LVAL_ERR) return x;
  if (y.type == LVAL_ERR) return y;  

  // if one of them is a decimal, we do decimal ops
  if (x.type == LVAL_DECIMAL || y.type == LVAL_DECIMAL) {
    
    // since the `num` field is a union, only one value could exist at a time
    // and accessing the other is UB (weird values), we need to transfer
    // the data when we're trying to do `integer op decimal` operations
    if (x.type == LVAL_DECIMAL && y.type != LVAL_DECIMAL) {
      y.num.f_num = (double)y.num.li_num;
    } else if (x.type != LVAL_DECIMAL && y.type == LVAL_DECIMAL) {
      x.num.f_num = (double)x.num.li_num;
    }

    if (strcmp(op, "+") == 0) return lval_double(x.num.f_num + y.num.f_num);
    if (strcmp(op, "-") == 0) return lval_double(x.num.f_num - y.num.f_num);
    if (strcmp(op, "*") == 0) return lval_double(x.num.f_num * y.num.f_num);
    if (strcmp(op, "^") == 0) return lval_double(pow(x.num.f_num, y.num.f_num));
    if (strcmp(op, "min") == 0) return lval_double(fmin(x.num.f_num, y.num.f_num));
    if (strcmp(op, "max") == 0) return lval_double(fmax(x.num.f_num, y.num.f_num));
    if (strcmp(op, "%") == 0) {
      return y.num.f_num == 0.0
        ? lval_err(LERR_BAD_NUM)
        : lval_double(fmod(x.num.f_num, y.num.f_num));
    }
    if (strcmp(op, "/") == 0) {
      return y.num.f_num == 0.0
        ? lval_err(LERR_DIV_ZERO)
        : lval_double(x.num.f_num / y.num.f_num);
    }
  } else {
    if (strcmp(op, "+") == 0) return lval_long(x.num.li_num + y.num.li_num);
    if (strcmp(op, "-") == 0) return lval_long(x.num.li_num - y.num.li_num);
    if (strcmp(op, "*") == 0) return lval_long(x.num.li_num * y.num.li_num);
    if (strcmp(op, "^") == 0) return lval_long((long)pow(x.num.li_num, y.num.li_num));
    if (strcmp(op, "min") == 0) return lval_long(min(x.num.li_num, y.num.li_num));
    if (strcmp(op, "max") == 0) return lval_long(max(x.num.li_num, y.num.li_num));
    if (strcmp(op, "%") == 0) {
      return y.num.li_num == 0
        ? lval_err(LERR_BAD_NUM)
        : lval_long(x.num.li_num % y.num.li_num);
    }
    if (strcmp(op, "/") == 0) {
      return y.num.li_num == 0
        ? lval_err(LERR_DIV_ZERO)
        : lval_long(x.num.li_num / y.num.li_num);
    }
  }

  return lval_err(LERR_BAD_OP);
}

Lval_t eval_ast(mpc_ast_t *ast) {
  if (strstr(ast->tag, "integer")) {
    errno = 0;
    long x = strtol(ast->contents, NULL, 10);
    return errno != ERANGE ? lval_long(x) : lval_err(LERR_BAD_NUM);
  } else if (strstr(ast->tag, "decimal")) {
    errno = 0;
    double x = strtof(ast->contents, NULL);
    return errno != ERANGE ? lval_double(x) : lval_err(LERR_BAD_NUM);
  }

  // children[0] == '('
  // children[1] is always the operator
  char* op = ast->children[1]->contents;

  Lval_t x = eval_ast(ast->children[2]);

  // Here we are guaranteed to have at least a closing parentheses ')'
  // --> children[3] == ')' or another expression
  int i = 3;
  while (strstr(ast->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval_ast(ast->children[i]));
    i++;
  }

  return x;
}