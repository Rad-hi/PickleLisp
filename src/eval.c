#include "eval.h"

static Lval_t* builtin_op(Lenv_t* e, Lval_t* a, char* op);
static Lval_t* builtin_add(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_sub(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_mul(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_div(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_mod(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_pow(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_min(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_max(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_head(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_tail(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_list(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_eval(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_join(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_exit(Lenv_t* e, Lval_t* a);

static Lval_t* builtin_lambda(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_fn(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_if(Lenv_t* e, Lval_t* a);

static Lval_t* builtin_var(Lenv_t* e, Lval_t* a, char* fn);
static Lval_t* builtin_def(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_put(Lenv_t* e, Lval_t* a);

static Lval_t* builtin_cmp(Lenv_t* e, Lval_t* a, char* op);
static Lval_t* builtin_eq(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_ne(Lenv_t* e, Lval_t* a);

static Lval_t* builtin_ord(Lenv_t* e, Lval_t* a, char* op);
static Lval_t* builtin_gt(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_lt(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_ge(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_le(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_and(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_or(Lenv_t* e, Lval_t* a);

/* lenv manipulators */
static void    lenv_add_builtin(Lenv_t* e, char* name, Lbuiltin_t fn);
static void    lenv_put(Lenv_t* e, Lval_t* k, Lval_t* v);
static void    lenv_def(Lenv_t* e, Lval_t* k, Lval_t* v);
static Lval_t* lenv_get(Lenv_t* e, Lval_t* k);
static Lenv_t* lenv_copy(Lenv_t* e);

/* lval manipulators */
static Lval_t* lval_join(Lval_t* x, Lval_t* y);
static Lval_t* lval_take(Lval_t* v, int i);
static Lval_t* lval_pop(Lval_t* v, int i);
static Lval_t* lval_add(Lval_t* v, Lval_t* x);
static Lval_t* lval_call(Lenv_t* e, Lval_t* f, Lval_t* a);
static Lval_t* lval_copy(Lval_t* v);
static int     lval_eq(Lval_t* x, Lval_t* y);

/* Parsers */
static Lval_t* lval_read_double(mpc_ast_t* ast);
static Lval_t* lval_read_long(mpc_ast_t* ast);
static Lval_t* lval_eval_sexpr(Lenv_t* e, Lval_t* v);

/* memory allocators */
static Lval_t* lval_create_double(double x);
static Lval_t* lval_create_long(long x);
static Lval_t* lval_create_sexpr(void);
static Lval_t* lval_create_qexpr(void);
static Lval_t* lval_create_err(char* fmt, ...);
static Lval_t* lval_create_sym(char* symbol);
static Lval_t* lval_create_fn(Lbuiltin_t fn);
static Lval_t* lval_create_lambda(Lval_t* formals, Lval_t* body);

/* Misc */
static void lval_expr_print(Lval_t* v, char open, char close);
static char* ltype_name(LVAL_e t);

/*
    Keep a record of all builtin names that exist in the language,
    to prohibit the user from overriding any of them
*/
static void _register_builtin_name(char* name);
static bool _lookup_builtin_name(char* name);
void _del_builtin_names(void);
typedef struct {
    char** names;
    int count;
} Builtins_record_t;

static Builtins_record_t __builtins__ = {
    .names = NULL,
    .count = 0
};


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
    if (strstr(ast->tag, "qexpr")) x = lval_create_qexpr();

    for (int i = 0; i < ast->children_num; ++i) {
        if (strcmp(ast->children[i]->contents, "(") == 0) continue;
        if (strcmp(ast->children[i]->contents, ")") == 0) continue;
        if (strcmp(ast->children[i]->contents, "{") == 0) continue;
        if (strcmp(ast->children[i]->contents, "}") == 0) continue;
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
        case LVAL_FN: {
            bool user_defined_fn = v->builtin == NULL;
            if (user_defined_fn) {
                lenv_del(v->env);
                lval_del(v->formals);
                lval_del(v->body);
            }
            break;
        }
        case LVAL_INTEGER:
        case LVAL_EXIT__:
        case LVAL_DECIMAL: break;

        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;

        case LVAL_QEXPR:
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
        case LVAL_QEXPR:   lval_expr_print(v, '{', '}'); break;
        case LVAL_EXIT__:  printf("exiting"); break;
        case LVAL_FN: {
            if (v->builtin != NULL) {
                printf("<builtin>");
            } else {
                printf("(\\ ");
                lval_print(v->formals);
                putchar(' ');
                lval_print(v->body); putchar(')');
            }
            break;
        }
    }
}

void lval_println(Lval_t* v) { lval_print(v); printf("\n"); }

Lenv_t* lenv_new(void) {
    Lenv_t* e = malloc(sizeof(Lenv_t));
    e->parent = NULL;
    e->count = 0;
    e->vals = NULL;
    e->syms = NULL;
    return e;
}

void lenv_del(Lenv_t* e) {
    for (int i = 0; i < e->count; ++i) {
        free(e->syms[i]);
        lval_del(e->vals[i]);
    }
    free(e->syms);
    free(e->vals);
    free(e);
}

/*
    Register all builtins in the environment
*/
void lenv_add_builtins(Lenv_t* e) {
    /* list functions */
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "join", builtin_join);

    /* mathematical functions */
    lenv_add_builtin(e, "min", builtin_min);
    lenv_add_builtin(e, "max", builtin_max);
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);
    lenv_add_builtin(e, "%", builtin_mod);
    lenv_add_builtin(e, "^", builtin_pow);

    /* comparisons */
    lenv_add_builtin(e, "if", builtin_if);
    lenv_add_builtin(e, "==", builtin_eq);
    lenv_add_builtin(e, "!=", builtin_ne);
    lenv_add_builtin(e, ">",  builtin_gt);
    lenv_add_builtin(e, "<",  builtin_lt);
    lenv_add_builtin(e, ">=", builtin_ge);
    lenv_add_builtin(e, "<=", builtin_le);
    lenv_add_builtin(e, "&&", builtin_and);
    lenv_add_builtin(e, "||", builtin_or);

    /* variable functions */
    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "=", builtin_put);
    lenv_add_builtin(e, "\\", builtin_lambda);
    lenv_add_builtin(e, "fn", builtin_fn);

    /* behavioral methods */
    lenv_add_builtin(e, "exit", builtin_exit);
}

void _del_builtin_names(void) {
    for (int i = 0; i < __builtins__.count; ++i) {
        free(__builtins__.names[i]);
    }
}

/*
  Recursively creates the list of symbolic expressions by
  calling `lval_eval_sexpr` which itself calls lval_eval
*/
Lval_t* lval_eval(Lenv_t* e, Lval_t* v) {
    if (v->type == LVAL_SYM) {
        Lval_t* x = lenv_get(e, v);
        lval_del(v);
        return x;
    }
    if (v->type == LVAL_SEXPR) return lval_eval_sexpr(e, v);
    return v;
}

static Lval_t* lval_eval_sexpr(Lenv_t* e, Lval_t* v) {
    for (int i = 0; i < v->count; ++i) {
        v->cell[i] = lval_eval(e, v->cell[i]);
    }

    for (int i = 0; i < v->count; ++i) {
        if (v->cell[i]->type == LVAL_ERR) return lval_take(v, i);
    }

    if (v->count == 0) return v;
    if (v->count == 1) return lval_take(v, 0);

    Lval_t* fn = lval_pop(v, 0);
    if (fn->type != LVAL_FN) {
        Lval_t* err = lval_create_err(
            "S-Expression must start with [%s] type. Got [%s]!",
            ltype_name(LVAL_FN), ltype_name(fn->type));
        lval_del(fn);
        lval_del(v);
        return err;
    }

    Lval_t* res = lval_call(e, fn, v);
    lval_del(fn);
    return res;
}

static Lval_t* lval_create_lambda(Lval_t* formals, Lval_t* body) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_FN;
    v->builtin = NULL;
    v->env = lenv_new();
    v->formals = formals;
    v->body = body;
    return v;
}

static Lval_t* lval_create_fn(Lbuiltin_t fn) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_FN;
    v->builtin = fn;
    return v;
}

static Lval_t* lval_create_sym(char* symbol) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_SYM;
    v->sym = malloc(strlen(symbol) + 1);
    strcpy(v->sym, symbol);
    return v;
}

static Lval_t* lval_create_err(char* fmt, ...) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_ERR;

    va_list va;
    va_start(va, fmt);

    v->err = malloc(ERR_BUF_LEN);
    vsnprintf(v->err, ERR_BUF_LEN - 1, fmt, va);
    v->err = realloc(v->err, strlen(v->err) + 1);

    va_end(va);

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

static Lval_t* lval_create_qexpr(void) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

static Lval_t* lval_read_long(mpc_ast_t* ast) {
    errno = 0;
    long x = strtol(ast->contents, NULL, 10);
    return errno != ERANGE
        ? lval_create_long(x)
        : lval_create_err("Number is out of range for long `%s`", ast->contents);
}

static Lval_t* lval_read_double(mpc_ast_t* ast) {
    errno = 0;
    double x = strtof(ast->contents, NULL);
    return errno != ERANGE
        ? lval_create_double(x)
        : lval_create_err("Number is out of range for double `%s`", ast->contents);
}

static Lval_t* lval_add(Lval_t* v, Lval_t* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(Lval_t*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

/*
    Dispatches function calls based on whether it's a builtin or a user-defined one
*/
static Lval_t* lval_call(Lenv_t* e, Lval_t* fn, Lval_t* a) {
    if (fn->builtin != NULL) return fn->builtin(e, a);

    int given = a->count;
    int total = fn->formals->count;

    while (a->count) {
        if (fn->formals->count == 0) {
            lval_del(a);
            return lval_create_err(
                "Function `user-defined` expects [%i] args, got [%i].", total, given);
        }

        Lval_t* sym = lval_pop(fn->formals, 0);

        /* Special case where we have a variable number of arguments, syntax `sym & syms` */
        if (strcmp(sym->sym, "&") == 0) {
            if (fn->formals->count != 1) {
                lval_del(a);
                return lval_create_err("Invalid format; "
                    "symbol `&` must be followed by a single symbol");
            }
            Lval_t* sym_list = lval_pop(fn->formals, 0);
            lenv_put(fn->env, sym_list, builtin_list(e, a));
            lval_del(sym);
            lval_del(sym_list);
            break;
        }


        Lval_t* val = lval_pop(a, 0);
        lenv_put(fn->env, sym, val);  // register into the func "local" environment
        lval_del(sym);
        lval_del(val);
    }

    lval_del(a);

    /*
        Done with user input (only supplied named args, no va_args), but fn formals
        still have the `&` in them -> change to empty list
    */
    if (fn->formals->count > 0 && strcmp(fn->formals->cell[0]->sym, "&") == 0) {
        if (fn->formals->count != 2) {
            return lval_create_err("Invalid format; symbol `&` must "
                                    "be followed by a single symbol");
        }
        lval_del(lval_pop(fn->formals, 0));  // pop and delete the `&` symbol
        Lval_t* sym = lval_pop(fn->formals, 0);  // va_args symbol
        Lval_t* val = lval_create_qexpr();  // empty list
        lenv_put(fn->env, sym, val);  // register these into the "local" env
        lval_del(sym);
        lval_del(val);
    }

    if (fn->formals->count == 0) { // all formals were bound -> evaluate function
        fn->env->parent = e;
        return builtin_eval(fn->env, lval_add(lval_create_sexpr(), lval_copy(fn->body)));
    } else { // return partially evaluated function
        return lval_copy(fn);
    }
}

static void lval_expr_print(Lval_t* v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; ++i) {
        lval_print(v->cell[i]);
        if (i != (v->count - 1)) { putchar(' '); }
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

static Lval_t* builtin_add(Lenv_t* e, Lval_t* a) { return builtin_op(e, a, "+"); }
static Lval_t* builtin_sub(Lenv_t* e, Lval_t* a) { return builtin_op(e, a, "-"); }
static Lval_t* builtin_mul(Lenv_t* e, Lval_t* a) { return builtin_op(e, a, "*"); }
static Lval_t* builtin_div(Lenv_t* e, Lval_t* a) { return builtin_op(e, a, "/"); }
static Lval_t* builtin_mod(Lenv_t* e, Lval_t* a) { return builtin_op(e, a, "%"); }
static Lval_t* builtin_pow(Lenv_t* e, Lval_t* a) { return builtin_op(e, a, "^"); }

static Lval_t* builtin_op(Lenv_t* e, Lval_t* a, char* op) {
    for (int i = 0; i < a->count; ++i) {
        if (a->cell[i]->type != LVAL_INTEGER && a->cell[i]->type != LVAL_DECIMAL) {
            char* type = ltype_name(a->cell[i]->type);
            lval_del(a);
            return lval_create_err("Operator `%s` cannot operate on non-numbers; "
                                   "Arg [%i] is of type [%s]", op, i + 1, type);
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
            if (y->type != LVAL_DECIMAL) {
                y->type = LVAL_DECIMAL; 
                y->num.f = (double)y->num.li;
            } else if (x->type != LVAL_DECIMAL) {
                x->type = LVAL_DECIMAL;
                x->num.f = (double)x->num.li;
            }

            if (strcmp(op, "+") == 0) x->num.f += y->num.f;
            if (strcmp(op, "-") == 0) x->num.f -= y->num.f;
            if (strcmp(op, "*") == 0) x->num.f *= y->num.f;
            if (strcmp(op, "^") == 0) x->num.f = pow(x->num.f, y->num.f);
            if (strcmp(op, "%") == 0) {
                if (y->num.f == 0.0) {
                    lval_del(x);
                    lval_del(y);
                    x = lval_create_err("Right-hand operand of '%s' cannot be 0!", op);
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
            if (strcmp(op, "%") == 0) {
                if (y->num.li == 0) {
                    lval_del(x);
                    lval_del(y);
                    x = lval_create_err("Right-hand operand of '%s' cannot be 0!", op);
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

static Lval_t* builtin_min(Lenv_t* e, Lval_t* a) {
    LASSERT(a, a->count >= 2, "Function `min` expects at least 2 arguments, got [%i]", a->count);
    for (int i = 0; i < a->count; ++i) {
        bool cond = a->cell[i]->type == LVAL_INTEGER || a->cell[i]->type == LVAL_DECIMAL;
        LASSERT(a, cond, "Function `min` expects arguments of type Number,"
                         " but arg [%i] is of type [%s]",
                         i + 1, ltype_name(a->cell[i]->type));
    }

    Lval_t* x = lval_pop(a, 0);
    while (a->count) {
        Lval_t* y = lval_pop(a, 0);

        if (x->type == LVAL_DECIMAL || y->type == LVAL_DECIMAL) {
            if (y->type != LVAL_DECIMAL) {
                y->type = LVAL_DECIMAL;
                y->num.f = (double)y->num.li;
            } else if (x->type != LVAL_DECIMAL) {
                x->type = LVAL_DECIMAL;
                x->num.f = (double)x->num.li;
            }
            x->num.f = fmin(x->num.f, y->num.f);  
        } else {
            x->num.li = min(x->num.li, y->num.li);  
        }
        lval_del(y);
    }

    lval_del(a);
    return x;
}

static Lval_t* builtin_max(Lenv_t* e, Lval_t* a) {
    LASSERT(a, a->count >= 2, "Function `max` expects at least 2 arguments, got [%i]", a->count);
    for (int i = 0; i < a->count; ++i) {
        bool cond = a->cell[i]->type == LVAL_INTEGER || a->cell[i]->type == LVAL_DECIMAL;
        LASSERT(a, cond, "Function `max` expects arguments of type Number,"
                         " but arg [%i] is of type [%s]",
                         i + 1, ltype_name(a->cell[i]->type));
    }

    Lval_t* x = lval_pop(a, 0);
    while (a->count) {
        Lval_t* y = lval_pop(a, 0);

        if (x->type == LVAL_DECIMAL || y->type == LVAL_DECIMAL) {
            if (x->type == LVAL_DECIMAL && y->type != LVAL_DECIMAL) {
                y->type = LVAL_DECIMAL; y->num.f = (double)y->num.li;
            } else if (x->type != LVAL_DECIMAL && y->type == LVAL_DECIMAL) {
                x->type = LVAL_DECIMAL; x->num.f = (double)x->num.li;
            }
            x->num.f = fmax(x->num.f, y->num.f);  
        } else {
            x->num.li = max(x->num.li, y->num.li);  
        }
        lval_del(y);
    }

    lval_del(a);
    return x;
}

static Lval_t* builtin_gt(Lenv_t* e, Lval_t* a)  { return builtin_ord(e, a, ">"); }
static Lval_t* builtin_lt(Lenv_t* e, Lval_t* a)  { return builtin_ord(e, a, "<"); }
static Lval_t* builtin_ge(Lenv_t* e, Lval_t* a)  { return builtin_ord(e, a, ">="); }
static Lval_t* builtin_le(Lenv_t* e, Lval_t* a)  { return builtin_ord(e, a, "<="); }
static Lval_t* builtin_and(Lenv_t* e, Lval_t* a) { return builtin_ord(e, a, "&&"); }
static Lval_t* builtin_or(Lenv_t* e, Lval_t* a)  { return builtin_ord(e, a, "||"); }

static Lval_t* builtin_ord(Lenv_t* e, Lval_t* a, char* op) {
    LASSERT(a, a->count == 2, "Operator `%s` expects 2 arguments, got [%i]", op, a->count);
    for (int i = 0; i < a->count; ++i) {
        bool cond = a->cell[i]->type == LVAL_INTEGER || a->cell[i]->type == LVAL_DECIMAL;
        LASSERT(a, cond, "Operator `%s` expects arguments of type Number,"
                         " but arg [%i] is of type [%s]",
                         op, i + 1, ltype_name(a->cell[i]->type));
    }

    Lval_t* x = lval_pop(a, 0);
    Lval_t* y = lval_pop(a, 0);

    if (x->type == LVAL_DECIMAL || y->type == LVAL_DECIMAL) {
        if (y->type != LVAL_DECIMAL) {
            y->type = LVAL_DECIMAL;
            y->num.f = (double)y->num.li;
        } else if (x->type != LVAL_DECIMAL) {
            x->type = LVAL_DECIMAL;
            x->num.f = (double)x->num.li;
        }
        if (strcmp(op, ">") == 0) x->num.f = x->num.f > y->num.f;
        if (strcmp(op, "<") == 0) x->num.f = x->num.f < y->num.f;
        if (strcmp(op, ">=") == 0) x->num.f = x->num.f >= y->num.f;
        if (strcmp(op, "<=") == 0) x->num.f = x->num.f <= y->num.f;
        if (strcmp(op, "&&") == 0) x->num.f = x->num.f && y->num.f;
        if (strcmp(op, "||") == 0) x->num.f = x->num.f || y->num.f;
    } else {
        if (strcmp(op, ">") == 0) x->num.li = x->num.li > y->num.li;
        if (strcmp(op, "<") == 0) x->num.li = x->num.li < y->num.li;
        if (strcmp(op, ">=") == 0) x->num.li = x->num.li >= y->num.li;
        if (strcmp(op, "<=") == 0) x->num.li = x->num.li <= y->num.li;
        if (strcmp(op, "&&") == 0) x->num.li = x->num.li && y->num.li;
        if (strcmp(op, "||") == 0) x->num.li = x->num.li || y->num.li;
    }

    lval_del(y);
    lval_del(a);
    return x;
}

/*
    evaluate the equality of two Lvals
*/
static int lval_eq(Lval_t* x, Lval_t* y) {
    if (x->type != y->type) return 0;

    switch (x->type) {
        case LVAL_INTEGER: return x->num.li == y->num.li;
        case LVAL_DECIMAL: return x->num.f == y->num.f;

        case LVAL_ERR: return strcmp(x->err, y->err) == 0;
        case LVAL_SYM: return strcmp(x->sym, y->sym) == 0;
        
        case LVAL_FN: {
            if (x->builtin || y->builtin) return x->builtin == y->builtin;
            else return lval_eq(x->formals, y->formals) && lval_eq(x->body, y->body);
        }

        case LVAL_SEXPR:
        case LVAL_QEXPR: {
            if (x->count != y->count) return 0;
            for (int i = 0; i < x->count; ++i) {
                if (!lval_eq(x->cell[i], y->cell[i])) return 0;
            }
            return 1;
        }

        case LVAL_EXIT__: return 1;
    }
    return 0;
}

static Lval_t* builtin_eq(Lenv_t* e, Lval_t* a) { return builtin_cmp(e, a, "=="); }
static Lval_t* builtin_ne(Lenv_t* e, Lval_t* a) { return builtin_cmp(e, a, "!="); }

static Lval_t* builtin_cmp(Lenv_t* e, Lval_t* a, char* op) {
    LASSERT_NUM(op, a, 2);

    int res;
    if (strcmp(op, "==") == 0) res = lval_eq(a->cell[0], a->cell[1]);
    if (strcmp(op, "!=") == 0) res = !lval_eq(a->cell[0], a->cell[1]);

    lval_del(a);
    return lval_create_long(res);
}

static Lval_t* builtin_if(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM("if", a, 3);
    
    if (a->cell[0]->type == LVAL_DECIMAL) {
        a->cell[0]->type = LVAL_INTEGER;
        a->cell[0]->num.li = (long)a->cell[0]->num.f;
    }
    LASSERT_TYPE("if", a, 0, LVAL_INTEGER);
    LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
    LASSERT_TYPE("if", a, 2, LVAL_QEXPR);

    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;

    Lval_t* x = a->cell[0]->num.li ? lval_eval(e, lval_pop(a, 1))
                                   : lval_eval(e, lval_pop(a, 2));
    lval_del(a);
    return x;
}

static Lval_t* builtin_head(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM("head", a, 1);
    LASSERT_TYPE("head", a, 0, LVAL_QEXPR);
    LASSERT(a, a->cell[0]->count != 0,
        "Function `head` expects a non-empty [%s], got {} as input!", ltype_name(LVAL_QEXPR));

    Lval_t* v = lval_take(a, 0);
    while (v->count > 1) { lval_del(lval_pop(v, 1)); }
    return v;
}

static Lval_t* builtin_tail(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM("tail", a, 1);
    LASSERT_TYPE("tail", a, 0, LVAL_QEXPR);
    LASSERT(a, a->cell[0]->count != 0,
        "Function `tail` expects a non-empty [%s], got {} as input!", ltype_name(LVAL_QEXPR));

    Lval_t* v = lval_take(a, 0);
    lval_del(lval_pop(v, 0));
    return v;
}

static Lval_t* builtin_list(Lenv_t* e, Lval_t* a) {
    a->type = LVAL_QEXPR;
    return a;
}

static Lval_t* builtin_eval(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM("eval", a, 1);
    LASSERT_TYPE("eval", a, 0, LVAL_QEXPR);

    Lval_t* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

static Lval_t* builtin_join(Lenv_t* e, Lval_t* a) {
    for (int i = 0; i < a->count; ++i) {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR,
        "Function `join` expects arguments of type [%s], "
        "but arg [%i] is of type [%s]",
        ltype_name(LVAL_QEXPR), i + 1, ltype_name(a->cell[0]->type));
    }

    Lval_t* x = lval_pop(a, 0);
    while (a->count) { x = lval_join(x, lval_pop(a, 0)); }

    lval_del(a);
    return x;
}

static Lval_t* builtin_def(Lenv_t* e, Lval_t* a) {
    return builtin_var(e, a, "def");
}

static Lval_t* builtin_put(Lenv_t* e, Lval_t* a) {
    return builtin_var(e, a, "=");
}

static Lval_t* builtin_var(Lenv_t* e, Lval_t* a, char* fn) {
    LASSERT_TYPE(fn, a, 0, LVAL_QEXPR);

    Lval_t* syms = a->cell[0];

    for (int i = 0; i < syms->count; ++i) {
        LASSERT(a, syms->cell[i]->type == LVAL_SYM,
            "Function `%s` cannot define a non-symbol; arg number [%i] "
            "is of type [%s]", fn, i + 1, ltype_name(syms->cell[i]->type));
    }

    LASSERT(a, syms->count == a->count - 1,
        "Function `%s` expects #symbols == #values; "
        "we got [%i] symbols, and [%i] values",
        fn, syms->count, a->count - 1);

    for (int i = 0; i < syms->count; ++i) {
        if (_lookup_builtin_name(syms->cell[i]->sym)) {
            LASSERT(a, false, "Function `%s` cannot define arg number [%i]"
                              " named '%s'; builtin keyword!",
                              fn, i + 1, syms->cell[i]->sym);
        }
    }

    for (int i = 0; i < syms->count; ++i) {
        if (strcmp(fn, "def") == 0) lenv_def(e, syms->cell[i], a->cell[i + 1]);
        if (strcmp(fn, "=") == 0)   lenv_put(e, syms->cell[i], a->cell[i + 1]);
    }

    lval_del(a);
    return lval_create_sexpr();
}

static Lval_t* builtin_exit(Lenv_t* e, Lval_t* a) {
    a->type = LVAL_EXIT__;
    return a;
}

static Lval_t* builtin_fn(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM("fn", a, 2);
    LASSERT_TYPE("fn", a, 0, LVAL_QEXPR);
    LASSERT_TYPE("fn", a, 1, LVAL_QEXPR);

    Lval_t* symbols = a->cell[0];

    /* First Q-expr must only contain symbols */
    for (int i = 0; i < symbols->count; ++i) {
        LASSERT(a, (symbols->cell[i]->type == LVAL_SYM),
            "Function `fn` cannot define arg [%i] of type [%s], expected [%s]",
            i, ltype_name(symbols->cell[i]->type), ltype_name(LVAL_SYM));
    }

    Lval_t* fn_name = lval_pop(symbols, 0);
    Lval_t* formals = lval_pop(a, 0);
    Lval_t* body = lval_pop(a, 0);
    lenv_def(e, fn_name, lval_create_lambda(formals, body));
    lval_del(a);
    return lval_create_sexpr();
}

static Lval_t* builtin_lambda(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM("\\", a, 2);
    LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
    LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

    /* First Q-expr must only contain symbols */
    for (int i = 0; i < a->cell[0]->count; ++i) {
        LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
            "Function `\\` cannot define arg [%i] of type [%s]. Expected type [%s]",
            i + 1, ltype_name(a->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
    }

    Lval_t* formals = lval_pop(a, 0);
    Lval_t* body = lval_pop(a, 0);
    lval_del(a);

    return lval_create_lambda(formals, body);
}

static Lval_t* lval_join(Lval_t* x, Lval_t* y) {
    while (y->count) { x = lval_add(x, lval_pop(y, 0)); }

    lval_del(y);
    return x;
}

static Lval_t* lval_copy(Lval_t* v) {
    Lval_t* x = malloc(sizeof(Lval_t));
    x->type = v->type;

    switch (v->type) {
        case LVAL_FN: {
            if (v->builtin != NULL) {
                x->builtin = v->builtin;
            } else {
                x->builtin = NULL;
                x->env = lenv_copy(v->env);
                x->formals = lval_copy(v->formals);
                x->body = lval_copy(v->body);
            }
            break;
        }
        case LVAL_DECIMAL: x->num.li = v->num.li; break;
        case LVAL_INTEGER: x->num.f = v->num.f; break;

        case LVAL_ERR: {
            x->err = malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err);
            break;
        }
        case LVAL_SYM: {
            x->sym = malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym);
            break;
        }

        case LVAL_QEXPR:
        case LVAL_SEXPR: {
            x->count = v->count;
            x->cell = malloc(sizeof(Lval_t*) * x->count);
            for (int i = 0; i < x->count; ++i) {
                x->cell[i] = lval_copy(v->cell[i]);
            }
            break;
        }
        case LVAL_EXIT__: break;
    }
    return x;
}

static void lenv_add_builtin(Lenv_t* e, char* name, Lbuiltin_t fn) {
    _register_builtin_name(name);

    Lval_t* k = lval_create_sym(name);
    Lval_t* v = lval_create_fn(fn);
    lenv_put(e, k, v);
    lval_del(k);
    lval_del(v);
}

static Lval_t* lenv_get(Lenv_t* e, Lval_t* k) {
    for (int i = 0; i < e->count; ++i) {
        if (strcmp(e->syms[i], k->sym) == 0) return lval_copy(e->vals[i]);
    }
    if (e->parent != NULL) return lenv_get(e->parent, k);
    return lval_create_err("Unbound symbol `%s`", k->sym);
}

/*
    puts a newly defined symbol into the global environment [syntax is `def`]
*/
static void lenv_def(Lenv_t* e, Lval_t* k, Lval_t* v) {
    while (e->parent != NULL) { e = e->parent; }
    lenv_put(e, k, v);
}

/*
    puts a newly defined symbol into a local environment [syntax is `=`]
*/
static void lenv_put(Lenv_t* e, Lval_t* k, Lval_t* v) {
    // lookup `k` in the env `e`
    for (int i = 0; i < e->count; ++i) {
        // found --> override existing symbol
        if (strcmp(e->syms[i], k->sym) == 0) {
            lval_del(e->vals[i]);
            e->vals[i] = lval_copy(v);
            return;
        }
    }

    e->count++;
    e->vals = realloc(e->vals, sizeof(Lval_t*) * e->count);
    e->syms = realloc(e->syms, sizeof(char*) * e->count);

    e->vals[e->count - 1] = lval_copy(v);
    e->syms[e->count - 1] = malloc(strlen(k->sym) + 1);
    strcpy(e->syms[e->count - 1], k->sym);
}

static Lenv_t* lenv_copy(Lenv_t* e) {
    Lenv_t* cpy = malloc(sizeof(Lenv_t));
    cpy->parent = e->parent;
    cpy->count = e->count;
    cpy->syms = malloc(sizeof(char*) * cpy->count);
    cpy->vals = malloc(sizeof(Lval_t*) * cpy->count);

    for (int i = 0; i < e->count; ++i) {
        cpy->syms[i] = malloc(strlen(e->syms[i]) + 1);
        strcpy(cpy->syms[i], e->syms[i]);
        cpy->vals[i] = lval_copy(e->vals[i]);
    }
    return cpy;
}

static void _register_builtin_name(char* name) {
    __builtins__.count++;
    __builtins__.names = realloc(__builtins__.names, sizeof(char*) * __builtins__.count);
    __builtins__.names[__builtins__.count - 1] = malloc(strlen(name) + 1);
    strcpy(__builtins__.names[__builtins__.count - 1], name);
}

static bool _lookup_builtin_name(char* name) {
    for (int i = 0; i < __builtins__.count; ++i) {
        if (strcmp(__builtins__.names[i], name) == 0) return true;
    }
    return false;
}

static char* ltype_name(LVAL_e t) {
    switch (t) {
        case LVAL_INTEGER:
        case LVAL_DECIMAL:  return "Number";
        case LVAL_ERR:      return "Error";
        case LVAL_SYM:      return "Symbol";
        case LVAL_FN:       return "Function";
        case LVAL_SEXPR:    return "S-Expression";
        case LVAL_QEXPR:    return "Q-Expression";
        case LVAL_EXIT__:   return "Exit";
        default: return "Your language is falling apart!";
    }
}