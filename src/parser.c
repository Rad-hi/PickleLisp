#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "err.h"
#include "mpc.h"

#ifdef _WIN32
  #define IN_BUF_SZ  2048
  static char buffer[IN_BUF_SZ];
  char* readline(char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, IN_BUF_SZ, stdin);
    char* ret = malloc(strlen(buffer) + 1);
    strcpy(ret, buffer);
    ret[strlen(ret) - 1] = '\0';
    return ret;
  }
  void add_history(char* unused) {}
#else
  #include <editline/readline.h>
  #include <editline/history.h>
#endif // _WIN32

#define REPL_IN    "8=> "
#define LANG_NAME  "PickleLisp"
#define PRINT_AST  0

#define min(a, b)  ((a) > (b) ? (b) : (a))
#define max(a, b)  ((a) > (b) ? (a) : (b))

lval_t eval_op(lval_t x, char* op, lval_t y) {
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

lval_t eval_ast(mpc_ast_t *ast) {
  if (strstr(ast->tag, "number")) {
    errno = 0;
    double x = strtof(ast->contents, NULL);
    return errno != ERANGE ? lval_eval(x, LVAL_NUM) : lval_eval(LERR_BAD_NUM, LVAL_ERR);
  }

  // children[0] == '('
  // children[1] is always the operator
  char* op = ast->children[1]->contents;

  lval_t x = eval_ast(ast->children[2]);

  // Here we are guaranteed to have at least a closing parentheses ')'
  // --> children[3] == ')' or another expression
  int i = 3;
  while (strstr(ast->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval_ast(ast->children[i]));
    i++;
  }

  return x;
}


int main(int argc, char** argv) {

  mpc_parser_t *number = mpc_new("number");
  mpc_parser_t *operator = mpc_new("operator");
  mpc_parser_t *expr = mpc_new("expr");
  mpc_parser_t *language = mpc_new("language");

  mpca_lang(MPCA_LANG_DEFAULT,
    "number   : /-?[0-9]+['.']?[0-9]*/ ;"
    "operator : '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\" ;"
    "expr     : <number> | '(' <operator> <expr>+ ')' ;"
    "language : /^/ <operator> <expr>+ /$/ ;",
    number, operator, expr, language
  );

  puts(LANG_NAME" Version 666.69.420");
  puts("Press Ctrl+C to Exit\n");

  mpc_result_t r;
  while(true) {
    char* buf = readline(REPL_IN);
    add_history(buf);

    if (mpc_parse("<stdin>", buf, language, &r)) {
#if PRINT_AST
      mpc_ast_print(r.output);
#endif
      lval_t res = eval_ast(r.output);
      lval_println(res);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(buf);
  }

  mpc_cleanup(4, number, operator, expr, language);

  return 0;
}
