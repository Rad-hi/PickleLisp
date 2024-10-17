#include "eval.h"

static Lval_t* builtin_op(Lval_t* a, char* op);
static Lval_t* lval_take(Lval_t* v, int i);
static Lval_t* lval_pop(Lval_t* v, int i);
static Lval_t* lval_add(Lval_t* v, Lval_t* x);
static Lval_t* lval_read_double(mpc_ast_t* ast);
static Lval_t* lval_create_double(double x);
static Lval_t* lval_read_long(mpc_ast_t* ast);
static Lval_t* lval_create_long(long x);
static Lval_t* lval_eval_sexpr(Lval_t* v);
static Lval_t* lval_create_sexpr(void);
static Lval_t* lval_create_err(char* msg);
static Lval_t* lval_create_sym(char* symbol);
static void lval_expr_print(Lval_t* v, char open, char close);


/*
  Entry point
*/
Lval_t* eval_ast(mpc_ast_t *ast) {
  Lval_t* res = lval_eval(lval_read(ast));
  return res;
}

/*
  Recursively creates the list of symbolic expressions by
  calling `lval_eval_sexpr` which itself calls lval_eval
*/
Lval_t* lval_eval(Lval_t* v) {
  if (v->type == LVAL_SEXPR) return lval_eval_sexpr(v);
  return v;
}

/*
  Recursively constructs the list of values (lval)
  based on theirs tags which are defined in grammar.h
*/
Lval_t* lval_read(mpc_ast_t* ast) {
  if (strstr(ast->tag, "integer")) return lval_read_long(ast);
  if (strstr(ast->tag, "decimal")) return lval_read_double(ast);
  if (strstr(ast->tag, "symbol")) return lval_create_sym(ast->contents);

  Lval_t* x = NULL;
  if (strcmp(ast->tag, ">") == 0) x = lval_create_sexpr();
  if (strstr(ast->tag, "sexpr")) x = lval_create_sexpr();

  for (int i = 0; i < ast->children_num; ++i) {
    if (strcmp(ast->children[i]->contents, "(") == 0) continue;
    if (strcmp(ast->children[i]->contents, ")") == 0) continue;
    if (strcmp(ast->children[i]->tag,  "regex") == 0) continue;
    x = lval_add(x, lval_read(ast->children[i]));
  }

  return x;
}

/*
  Recursively deletes the list-value pointer as well as
  any children that were allocated on the heap
*/
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

void lval_println(Lval_t* v) { lval_print(v); printf("\n"); }


static Lval_t* lval_eval_sexpr(Lval_t* v) {
  for (int i = 0; i < v->count; ++i) {
    v->cell[i] = lval_eval(v->cell[i]);
  }

  for (int i = 0; i < v->count; ++i) {
    if (v->cell[i]->type == LVAL_ERR) return lval_take(v, i);
  }

  if (v->count == 0) return v;
  if (v->count == 1) return lval_take(v, 0);

  Lval_t* start = lval_pop(v, 0);
  if (start->type != LVAL_SYM) {
    lval_del(start);
    lval_del(v);
    return lval_create_err("S-expression must start with a symbol!");
  }

  Lval_t* res = builtin_op(v, start->sym);
  lval_del(start);
  return res;

}

static Lval_t* lval_create_sym(char* symbol) {
  Lval_t* v = malloc(sizeof(Lval_t));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(symbol) + 1);
  strcpy(v->sym, symbol);
  return v;
}

static Lval_t* lval_create_err(char* msg) {
  Lval_t* v = malloc(sizeof(Lval_t));
  v->type = LVAL_ERR;
  v->err = malloc(strlen(msg) + 1);
  strcpy(v->err, msg);
  return v;
}

static Lval_t* lval_create_long(long x) {
  Lval_t* v = malloc(sizeof(Lval_t));
  v->type = LVAL_INTEGER;
  v->num.li = x;
  return v;
}

static Lval_t* lval_create_double(double x) {
  Lval_t* v = malloc(sizeof(Lval_t));
  v->type = LVAL_DECIMAL;
  v->num.f = x;
  return v;
}

static Lval_t* lval_create_sexpr(void) {
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
    ? lval_create_long(x)
    : lval_create_err("invalid number");
}

static Lval_t* lval_read_double(mpc_ast_t* ast) {
  errno = 0;
  double x = strtof(ast->contents, NULL);
  return errno != ERANGE
    ? lval_create_double(x)
    : lval_create_err("invalid number");
}

static Lval_t* lval_add(Lval_t* v, Lval_t* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(Lval_t*) * v->count);
  v->cell[v->count - 1] = x;
  return v;
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


static Lval_t* lval_pop(Lval_t* v, int i) {
  assert(i < v->count && "Index provided to `pop` is out of bound");
  Lval_t* x = v->cell[i];
  memmove(&v->cell[i], &v->cell[i+1], sizeof(Lval_t*) * (v->count - i - 1));
  v->count--;
  v->cell = realloc(v->cell, sizeof(Lval_t*) * v->count);
  return x;
}

static Lval_t* lval_take(Lval_t* v, int i) {
  Lval_t* x = lval_pop(v, i);
  lval_del(v);
  return x;
}
static Lval_t* builtin_op(Lval_t* a, char* op) {
  for (int i = 0; i < a->count; ++i) {
    if (a->cell[i]->type != LVAL_INTEGER && a->cell[i]->type != LVAL_DECIMAL) {
      lval_del(a);
      return lval_create_err("Cannot operate on non-number!");
    }
  }

  Lval_t* x = lval_pop(a, 0);

  // no arguments provided and `op` is `-` then perform negation
  if ((strcmp(op, "-") == 0) && a->count == 0) {
    if (x->type == LVAL_INTEGER) x->num.li = -x->num.li;
    else if (x->type == LVAL_DECIMAL) x->num.f = -x->num.f;
  }

  while (a->count > 0) {
    Lval_t* y = lval_pop(a, 0);

    // if any one of inuts is a decimal, output will be decimal
    if (x->type == LVAL_DECIMAL || y->type == LVAL_DECIMAL) {
      // since the `num` field is a union, only one value could exist at a time
      // and accessing the other is UB (weird values), we need to transfer
      // the data when we're trying to do `integer op decimal` operations
      if (x->type == LVAL_DECIMAL && y->type != LVAL_DECIMAL) {
        y->type = LVAL_DECIMAL;
        y->num.f = (double)y->num.li;
      } else if (x->type != LVAL_DECIMAL && y->type == LVAL_DECIMAL) {
        x->type = LVAL_DECIMAL;
        x->num.f = (double)x->num.li;
      }

      if (strcmp(op, "+") == 0) x->num.f += y->num.f;
      if (strcmp(op, "-") == 0) x->num.f -= y->num.f;
      if (strcmp(op, "*") == 0) x->num.f *= y->num.f;
      if (strcmp(op, "^") == 0) x->num.f = pow(x->num.f, y->num.f);
      if (strcmp(op, "min") == 0) x->num.f = fmin(x->num.f, y->num.f);
      if (strcmp(op, "max") == 0) x->num.f = fmax(x->num.f, y->num.f);
      if (strcmp(op, "%") == 0) {
        if (y->num.f == 0.0) {
          lval_del(x);
          lval_del(y);
          x = lval_create_err("Right-hand operand of \% cannot be 0!");
          break;
        }
        x->num.f = fmod(x->num.f, y->num.f);
      }
      if (strcmp(op, "/") == 0) {
        if (y->num.f == 0.0) {
          lval_del(x);
          lval_del(y);
          x = lval_create_err("Division By Zero!");
          break;
        }
        x->num.f /= y->num.f;
      }
    } else {
      if (strcmp(op, "+") == 0) x->num.li += y->num.li;
      if (strcmp(op, "-") == 0) x->num.li -= y->num.li;
      if (strcmp(op, "*") == 0) x->num.li *= y->num.li;
      if (strcmp(op, "^") == 0) x->num.li = (long)pow(x->num.li, y->num.li);
      if (strcmp(op, "min") == 0) x->num.li = min(x->num.li, y->num.li);
      if (strcmp(op, "max") == 0) x->num.li = max(x->num.li, y->num.li);
      if (strcmp(op, "%") == 0) {
        if (y->num.li == 0) {
          lval_del(x);
          lval_del(y);
          x = lval_create_err("Right-hand operand of \% cannot be 0!");
          break;
        }
        x->num.li %= y->num.li;
      }
      if (strcmp(op, "/") == 0) {
        if (y->num.li == 0) {
          lval_del(x);
          lval_del(y);
          x = lval_create_err("Division By Zero!");
          break;
        }
        x->num.li /= y->num.li;
      }
    }

    lval_del(y);
  }

  lval_del(a);
  return x;
}
