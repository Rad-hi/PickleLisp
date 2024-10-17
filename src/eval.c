#include <stdio.h>
#include <stdbool.h>
#include "eval.h"

static void lval_expr_print(Lval_t* v, char open, char close);

static Lval_t* lval_sym(char* symbol) {
  Lval_t* v = malloc(sizeof(Lval_t));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(symbol) + 1);
  strcpy(v->sym, symbol);
  return v;
}

static Lval_t* lval_err(char* msg) {
  Lval_t* v = malloc(sizeof(Lval_t));
  v->type = LVAL_ERR;
  v->err = malloc(strlen(msg) + 1);
  strcpy(v->err, msg);
  return v;
}

static Lval_t* lval_long(long x) {
  Lval_t* v = malloc(sizeof(Lval_t));
  v->type = LVAL_INTEGER;
  v->num.li = x;
  return v;
}

static Lval_t* lval_double(double x) {
  Lval_t* v = malloc(sizeof(Lval_t));
  v->type = LVAL_DECIMAL;
  v->num.f = x;
  return v;
}

static Lval_t* lval_sexpr(void) {
  Lval_t* v = malloc(sizeof(Lval_t));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

static Lval_t* lval_read_long(mpc_ast_t* ast) {
  errno = 0;
  long x = strtol(ast->contents, NULL, 10);
  return errno != ERANGE
    ? lval_long(x)
    : lval_err("invalid number");
}

static Lval_t* lval_read_double(mpc_ast_t* ast) {
  errno = 0;
  double x = strtof(ast->contents, NULL);
  return errno != ERANGE
    ? lval_double(x)
    : lval_err("invalid number");
}

static Lval_t* lval_add(Lval_t* v, Lval_t* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(Lval_t*) * v->count);
  v->cell[v->count - 1] = x;
  return v;
}

Lval_t* lval_read(mpc_ast_t* ast) {
  if (strstr(ast->tag, "integer")) return lval_read_long(ast);
  if (strstr(ast->tag, "decimal")) return lval_read_double(ast);
  if (strstr(ast->tag, "symbol")) return lval_sym(ast->contents);

  Lval_t* x = NULL;
  if (strcmp(ast->tag, ">") == 0) x = lval_sexpr();
  if (strstr(ast->tag, "sexpr")) x = lval_sexpr();

  for (int i = 0; i < ast->children_num; ++i) {
    if (strcmp(ast->children[i]->contents, "(") == 0) continue;
    if (strcmp(ast->children[i]->contents, ")") == 0) continue;
    if (strcmp(ast->children[i]->tag,  "regex") == 0) continue;
    x = lval_add(x, lval_read(ast->children[i]));
  }

  return x;
}

void lval_del(Lval_t* v) {
  switch (v->type) {
    case LVAL_INTEGER:
    case LVAL_DECIMAL: break;

    case LVAL_ERR: free(v->err); break;
    case LVAL_SYM: free(v->sym); break;
    
    case LVAL_SEXPR: {
      for (int i = 0; i < v->count; ++i) {
        lval_del(v->cell[i]);
      }
      free(v->cell);
      break;
    }
  }
  free(v);
}

void lval_print(Lval_t* v) {
  switch (v->type) {
    case LVAL_INTEGER: printf("%li", v->num.li); break;
    case LVAL_DECIMAL: printf("%f", v->num.f); break;
    case LVAL_ERR:     printf("[ERROR] %s", v->err); break;
    case LVAL_SYM:     printf("%s", v->sym); break;
    case LVAL_SEXPR:   lval_expr_print(v, '(', ')'); break;
  }
}

void lval_println(Lval_t* v) {
  lval_print(v); printf("\n"); 
}

static void lval_expr_print(Lval_t* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; ++i) {
    lval_print(v->cell[i]);

    if (i != (v->count - 1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

// Lval_t eval_op(Lval_t x, char* op, Lval_t y) {
//   if (x.type == LVAL_ERR) return x;
//   if (y.type == LVAL_ERR) return y;  

//   // if one of them is a decimal, we do decimal ops
//   if (x.type == LVAL_DECIMAL || y.type == LVAL_DECIMAL) {
    
//     // since the `num` field is a union, only one value could exist at a time
//     // and accessing the other is UB (weird values), we need to transfer
//     // the data when we're trying to do `integer op decimal` operations
//     if (x.type == LVAL_DECIMAL && y.type != LVAL_DECIMAL) {
//       y.num.f = (double)y.num.li;
//     } else if (x.type != LVAL_DECIMAL && y.type == LVAL_DECIMAL) {
//       x.num.f = (double)x.num.li;
//     }

//     if (strcmp(op, "+") == 0) return lval_double(x.num.f + y.num.f);
//     if (strcmp(op, "-") == 0) return lval_double(x.num.f - y.num.f);
//     if (strcmp(op, "*") == 0) return lval_double(x.num.f * y.num.f);
//     if (strcmp(op, "^") == 0) return lval_double(pow(x.num.f, y.num.f));
//     if (strcmp(op, "min") == 0) return lval_double(fmin(x.num.f, y.num.f));
//     if (strcmp(op, "max") == 0) return lval_double(fmax(x.num.f, y.num.f));
//     if (strcmp(op, "%") == 0) {
//       return y.num.f == 0.0
//         ? lval_err(LERR_BAD_NUM)
//         : lval_double(fmod(x.num.f, y.num.f));
//     }
//     if (strcmp(op, "/") == 0) {
//       return y.num.f == 0.0
//         ? lval_err(LERR_DIV_ZERO)
//         : lval_double(x.num.f / y.num.f);
//     }
//   } else {
//     if (strcmp(op, "+") == 0) return lval_long(x.num.li + y.num.li);
//     if (strcmp(op, "-") == 0) return lval_long(x.num.li - y.num.li);
//     if (strcmp(op, "*") == 0) return lval_long(x.num.li * y.num.li);
//     if (strcmp(op, "^") == 0) return lval_long((long)pow(x.num.li, y.num.li));
//     if (strcmp(op, "min") == 0) return lval_long(min(x.num.li, y.num.li));
//     if (strcmp(op, "max") == 0) return lval_long(max(x.num.li, y.num.li));
//     if (strcmp(op, "%") == 0) {
//       return y.num.li == 0
//         ? lval_err(LERR_BAD_NUM)
//         : lval_long(x.num.li % y.num.li);
//     }
//     if (strcmp(op, "/") == 0) {
//       return y.num.li == 0
//         ? lval_err(LERR_DIV_ZERO)
//         : lval_long(x.num.li / y.num.li);
//     }
//   }

//   return lval_err(LERR_BAD_OP);
// }

// Lval_t eval_ast(mpc_ast_t *ast) {
//   if (strstr(ast->tag, "integer")) {
//     errno = 0;
//     long x = strtol(ast->contents, NULL, 10);
//     return errno != ERANGE ? lval_long(x) : lval_err(LERR_BAD_NUM);
//   } else if (strstr(ast->tag, "decimal")) {
//     errno = 0;
//     double x = strtof(ast->contents, NULL);
//     return errno != ERANGE ? lval_double(x) : lval_err(LERR_BAD_NUM);
//   }

//   // children[0] == '('
//   // children[1] is always the operator
//   char* op = ast->children[1]->contents;

//   Lval_t x = eval_ast(ast->children[2]);

//   // Here we are guaranteed to have at least a closing parentheses ')'
//   // --> children[3] == ')' or another expression
//   int i = 3;
//   while (strstr(ast->children[i]->tag, "expr")) {
//     x = eval_op(x, op, eval_ast(ast->children[i]));
//     i++;
//   }

//   return x;
// }