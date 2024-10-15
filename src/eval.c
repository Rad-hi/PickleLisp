#include <stdio.h>
#include <stdbool.h>
#include "eval.h"

Lval_t lval_eval(double x, LVAL_e type) {
  Lval_t v;
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

void lval_print(Lval_t v) {
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

void lval_println(Lval_t v) { lval_print(v); printf("\n"); }

Lval_t eval_op(Lval_t x, char* op, Lval_t y) {
  if (x.type == LVAL_ERR) return x;
  if (y.type == LVAL_ERR) return y;

  if (strcmp(op, "+") == 0) return lval_eval(x.num + y.num, LVAL_NUM);
  if (strcmp(op, "-") == 0) return lval_eval(x.num - y.num, LVAL_NUM);
  if (strcmp(op, "*") == 0) return lval_eval(x.num * y.num, LVAL_NUM);
  if (strcmp(op, "%") == 0) return lval_eval(fmod(x.num, y.num), LVAL_NUM);
  if (strcmp(op, "^") == 0) return lval_eval(pow(x.num, y.num), LVAL_NUM);
  if (strcmp(op, "min") == 0) return lval_eval(fmin(x.num, y.num), LVAL_NUM);
  if (strcmp(op, "max") == 0) return lval_eval(fmax(x.num, y.num), LVAL_NUM);
  if (strcmp(op, "/") == 0) {
    return y.num == 0.0
      ? lval_eval(LERR_DIV_ZERO, LVAL_ERR)
      : lval_eval(x.num / y.num, LVAL_NUM);
  }

  return lval_eval(LERR_BAD_OP, LVAL_ERR);
}

Lval_t eval_ast(mpc_ast_t *ast) {
  if (strstr(ast->tag, "number")) {
    errno = 0;
    double x = strtof(ast->contents, NULL);
    return errno != ERANGE ? lval_eval(x, LVAL_NUM) : lval_eval(LERR_BAD_NUM, LVAL_ERR);
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