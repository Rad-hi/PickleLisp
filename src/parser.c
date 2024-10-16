#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "eval.h"
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


int main(int argc, char** argv) {

  mpc_parser_t *integer = mpc_new("integer");
  mpc_parser_t *decimal = mpc_new("decimal");
  mpc_parser_t *number = mpc_new("number");
  mpc_parser_t *operator = mpc_new("operator");
  mpc_parser_t *expr = mpc_new("expr");
  mpc_parser_t *language = mpc_new("language");

  mpca_lang(MPCA_LANG_DEFAULT,
    "integer  : /-?[0-9]+/ ;"
    "decimal  : /-?[0-9]*['.'][0-9]*[fF]?/ ;"
    "number   : <decimal> | <integer> ;"
    "operator : '+' | '-' | '*' | '/' | '%' | '^' | \"min\" | \"max\" ;"
    "expr     : <number> | '(' <operator> <expr>+ ')' ;"
    "language : /^/ <operator> <expr>+ /$/ ;",
    integer, decimal, number, operator, expr, language
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
      Lval_t res = eval_ast(r.output);
      lval_println(res);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(buf);
  }

  mpc_cleanup(6, integer, decimal, number, operator, expr, language);

  return 0;
}
