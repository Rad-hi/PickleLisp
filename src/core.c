#include "core.h"

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
static Lval_t* builtin_not(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_and(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_or(Lenv_t* e, Lval_t* a);

static Lval_t* builtin_error(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_print(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_read(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_type(Lenv_t* e, Lval_t* a);

static Lval_t* builtin_dll(Lenv_t* e, Lval_t* a);
static Lval_t* builtin_extern(Lenv_t* e, Lval_t* a);

static void    lenv_add_builtin_const(Lenv_t* e, char* name, Lval_t* val);
static void    lenv_add_builtin(Lenv_t* e, char* name, Lbuiltin_t fn);
static void    lenv_put(Lenv_t* e, Lval_t* k, Lval_t* v);
static void    lenv_def(Lenv_t* e, Lval_t* k, Lval_t* v);
static Lval_t* lenv_get(Lenv_t* e, Lval_t* k);
static Lenv_t* lenv_copy(Lenv_t* e);

static Lval_t* lval_join(Lval_t* x, Lval_t* y);
static Lval_t* lval_take(Lval_t* v, int i);
static Lval_t* lval_pop(Lval_t* v, int i);
static Lval_t* lval_call(Lenv_t* e, Lval_t* f, Lval_t* a);
static Lval_t* lval_copy(Lval_t* v);
static int     lval_eq(Lval_t* x, Lval_t* y);

static Lval_t* lval_read_double(mpc_ast_t* ast);
static Lval_t* lval_read_long(mpc_ast_t* ast);
static Lval_t* lval_read_str(mpc_ast_t* ast);
static Lval_t* lval_eval_sexpr(Lenv_t* e, Lval_t* v);

/* memory allocators */
static Lval_t* lval_create_ok(void);
static Lval_t* lval_create_exit(void);
static Lval_t* lval_create_bool(bool x);
static Lval_t* lval_create_double(double x);
static Lval_t* lval_create_long(long x);
static Lval_t* lval_create_qexpr(void);
static Lval_t* lval_create_err(char* fmt, ...);
static Lval_t* lval_create_sym(char* symbol);
static Lval_t* lval_create_fn(Lbuiltin_t fn);
static Lval_t* lval_create_lambda(Lval_t* formals, Lval_t* body);
static Lval_t* lval_create_dll(void* dll);
static Lval_t* lval_create_color_type(void);
static Lval_t* lval_create_str_type(void);
static Lval_t* lval_create_double_type(void);
static Lval_t* lval_create_int_type(void);
static Lval_t* lval_create_void_type(void);

static void    lval_expr_print(Lval_t* v, char open, char close);
static char*   ltype_name(LVAL_e t);
static void    lval_print_str(Lval_t* v);
static char*   freadline(FILE* fp, size_t size);
static Color_t color_from_list(Lval_t* l);

/*
    Keep a record of all builtin names that exist in the language,
    to prohibit the user from overriding any of them
*/
static void _register_builtin_name(char* name);
static bool _lookup_builtin_name(char* name);
typedef struct {
    char** names;
    int* lengths;
    int count;
} Builtins_record_t;

// TODO: Change to a hash-map, so that runtime lookup is faster
//       (though this being a contiguous array, maybe lookup is faster
//       this way as long as the number of builtins is small)
static Builtins_record_t __builtins__ = {
    .names = NULL,
    .lengths = NULL,
    .count = 0
};

// NOTE: keep the same order as the definition
static char* TYPE_NAME_LUT[N_TYPES] = {
    "C_VOID",
    "C_INT",
    "C_DOUBLE",
    "C_STRING",
    "C_COLOR",
};

// Ref: https://eli.thegreenplace.net/2013/03/04/flexible-runtime-interface-to-shared-libraries-with-libffi
ffi_type* color_elements[5] = {&ffi_type_schar, &ffi_type_schar, &ffi_type_schar, &ffi_type_schar, NULL};
ffi_type ffi_color_type = {.size=0, .alignment=0, .type=FFI_TYPE_STRUCT, .elements=color_elements};

/*
  Recursively constructs the list of values (lval)
  based on theirs tags which are defined in lang.h
*/
Lval_t* lval_read(mpc_ast_t* ast) {
    if      (strstr(ast->tag, "integer")) return lval_read_long(ast);
    else if (strstr(ast->tag, "decimal")) return lval_read_double(ast);
    else if (strstr(ast->tag, "string"))  return lval_read_str(ast);
    else if (strstr(ast->tag, "symbol"))  return lval_create_sym(ast->contents);

    Lval_t* x = NULL;
    if      (strncmp(ast->tag, ">", 2) == 0) x = lval_create_sexpr();
    else if (strstr(ast->tag, "sexpr")) x = lval_create_sexpr();
    else if (strstr(ast->tag, "qexpr")) x = lval_create_qexpr();

    for (int i = 0; i < ast->children_num; ++i) {
        if      (strncmp(ast->children[i]->contents, "(", 2) == 0) continue;
        else if (strncmp(ast->children[i]->contents, ")", 2) == 0) continue;
        else if (strncmp(ast->children[i]->contents, "{", 2) == 0) continue;
        else if (strncmp(ast->children[i]->contents, "}", 2) == 0) continue;
        else if (strncmp(ast->children[i]->tag,  "regex", 6) == 0) continue;
        else if (strstr(ast->children[i]->tag, "comment"))         continue;
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
                free(v->cif);
                free(v->atypes);
            }
            break;
        }

        case LVAL_OK:
        case LVAL_EXIT:
        case LVAL_BOOL:
        case LVAL_TYPE:
        case LVAL_INTEGER:
        case LVAL_DECIMAL: break;

        case LVAL_STR: free(v->str); break;
        case LVAL_ERR: free(v->err); break;
        case LVAL_SYM: free(v->sym); break;

        case LVAL_DLL: dlclose(v->dll); break;

        case LVAL_QEXPR:
        case LVAL_SEXPR: {
            for (int i = 0; i < v->count; ++i) {
                lval_del(v->cell[i]);
            }
            free(v->cell);
            break;
        }
        default: 
            fprintf(stderr, "You added a new type, but forgot to add it to %s!", __func__);
            assert(false);
    }
    free(v);
}

void lval_print(Lval_t* v) {
    switch (v->type) {
        case LVAL_INTEGER: printf("%li", v->num.li); break;
        case LVAL_DECIMAL: printf("%f", v->num.f); break;
        case LVAL_BOOL:    printf("%s", v->num.li ? "true" : "false"); break;
        case LVAL_ERR:     printf("[ERROR] %s", v->err); break;
        case LVAL_STR:     lval_print_str(v); break;
        case LVAL_SYM:     printf("%s", v->sym); break;
        case LVAL_SEXPR:   lval_expr_print(v, '(', ')'); break;
        case LVAL_QEXPR:   lval_expr_print(v, '{', '}'); break;
        case LVAL_EXIT:    printf("Exiting"); break;
        case LVAL_DLL:     printf("Dynamic library"); break;
        case LVAL_TYPE:    printf("%s", TYPE_NAME_LUT[v->c_type]); break;
        case LVAL_OK:      break;
        case LVAL_FN: {
            if (v->builtin != NULL) {
                printf("<builtin>");
            } else {
                printf("(\\ ");
                lval_print(v->formals);
                putchar(' ');
                lval_print(v->body);
                putchar(')');
            }
            break;
        }
        default:
            fprintf(stderr, "You added a new type, but forgot to add it to %s!", __func__);
            assert(false);
    }
}

void lval_println(Lval_t* v) {
    if (v->type == LVAL_OK) return;
    lval_print(v); printf("\n");
}

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
    lenv_add_builtin(e, "list",  builtin_list);
    lenv_add_builtin(e, "head",  builtin_head);
    lenv_add_builtin(e, "tail",  builtin_tail);
    lenv_add_builtin(e, "eval",  builtin_eval);
    lenv_add_builtin(e, "join",  builtin_join);
    lenv_add_builtin(e, "min",   builtin_min);
    lenv_add_builtin(e, "max",   builtin_max);
    lenv_add_builtin(e, "+",     builtin_add);
    lenv_add_builtin(e, "-",     builtin_sub);
    lenv_add_builtin(e, "*",     builtin_mul);
    lenv_add_builtin(e, "/",     builtin_div);
    lenv_add_builtin(e, "%",     builtin_mod);
    lenv_add_builtin(e, "^",     builtin_pow);
    lenv_add_builtin(e, "if",    builtin_if);
    lenv_add_builtin(e, "==",    builtin_eq);
    lenv_add_builtin(e, "!=",    builtin_ne);
    lenv_add_builtin(e, ">",     builtin_gt);
    lenv_add_builtin(e, "<",     builtin_lt);
    lenv_add_builtin(e, ">=",    builtin_ge);
    lenv_add_builtin(e, "<=",    builtin_le);
    lenv_add_builtin(e, "!",     builtin_not);
    lenv_add_builtin(e, "&&",    builtin_and);
    lenv_add_builtin(e, "||",    builtin_or);
    lenv_add_builtin(e, "load",  builtin_load);
    lenv_add_builtin(e, "print", builtin_print);
    lenv_add_builtin(e, "read",  builtin_read);
    lenv_add_builtin(e, "error", builtin_error);
    lenv_add_builtin(e, "type",  builtin_type);
    lenv_add_builtin(e, "def",   builtin_def);
    lenv_add_builtin(e, "=",     builtin_put);
    lenv_add_builtin(e, "\\",    builtin_lambda);
    lenv_add_builtin(e, "fn",    builtin_fn);

    lenv_add_builtin(e, "dll",    builtin_dll);
    lenv_add_builtin(e, "extern", builtin_extern);

    /* atoms */
    lenv_add_builtin_const(e, "ok",    lval_create_ok());
    lenv_add_builtin_const(e, "nil",   lval_create_qexpr());
    lenv_add_builtin_const(e, "true",  lval_create_bool(true));
    lenv_add_builtin_const(e, "false", lval_create_bool(false));
    lenv_add_builtin_const(e, "exit",  lval_create_exit());

    /* types */
    lenv_add_builtin_const(e, "Void",   lval_create_void_type());
    lenv_add_builtin_const(e, "Int",    lval_create_int_type());
    lenv_add_builtin_const(e, "Double", lval_create_double_type());
    lenv_add_builtin_const(e, "String", lval_create_str_type());
    lenv_add_builtin_const(e, "Color",  lval_create_color_type());
}

/*
    registers all symbols in an environment as builtin names
*/
void _register_builtin_names_from_env(Lenv_t* e) {
    for (int i = 0; i < e->count; ++i) {
        _register_builtin_name(e->syms[i]);
    }
}

void _del_builtin_names(void) {
    for (int i = 0; i < __builtins__.count; ++i) {
        free(__builtins__.names[i]);
    }
    free(__builtins__.names);
    free(__builtins__.lengths);
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

Lval_t* lval_create_sexpr(void) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

Lval_t* lval_create_str(char* s) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_STR;
    v->str = malloc(strlen(s) + 1);
    strcpy(v->str, s);
    return v;
}

Lval_t* lval_add(Lval_t* v, Lval_t* x) {
    v->count++;
    v->cell = realloc(v->cell, sizeof(Lval_t*) * v->count);
    v->cell[v->count - 1] = x;
    return v;
}

Lval_t* builtin_load(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPE(__func__, a, 0, LVAL_STR);

    char* filename = a->cell[0]->str;
    size_t base_name_len = strlen(filename) - strlen(EXTENSION);
    char* end = filename + (base_name_len > 0 ? base_name_len : 0);
    LASSERT(a, strncmp(end , EXTENSION, strlen(EXTENSION)) == 0,
            "Function `%s` expects a file with the extension [%s], got [%s]",
            __func__, EXTENSION, filename);

    mpc_result_t r;
    if (mpc_parse_contents(filename, pickle_lisp, &r)) {
        Lval_t* expr = lval_read(r.output);
        mpc_ast_delete(r.output);

        while (expr->count) {
            Lval_t* x = lval_eval(e, lval_pop(expr, 0));
            if (x->type == LVAL_ERR) lval_println(x);
            lval_del(x);
        }

        lval_del(expr);
        lval_del(a);
        return lval_create_ok();
    } else {
        char* err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);

        Lval_t* err = lval_create_err("Could not load library [%s]", err_msg);
        free(err_msg);
        lval_del(a);

        return err;
    }
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
    v->cif = malloc(sizeof(ffi_cif));
    v->atypes = malloc(formals->count * sizeof(ffi_type*));
    v->extern_ptr = NULL;
    v->is_extern = false;
    return v;
}

static Lval_t* lval_create_fn(Lbuiltin_t fn) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_FN;
    v->is_extern = false;
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

static Lval_t* lval_create_ok(void) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_OK;
    return v;
}

static Lval_t* lval_create_void_type(void) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_TYPE;
    v->c_type = C_VOID;
    return v;
}

static Lval_t* lval_create_int_type(void) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_TYPE;
    v->c_type = C_INT;
    return v;
}

static Lval_t* lval_create_double_type(void) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_TYPE;
    v->c_type = C_DOUBLE;
    return v;
}

static Lval_t* lval_create_str_type(void) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_TYPE;
    v->c_type = C_STRING;
    return v;
}

static Lval_t* lval_create_color_type(void) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_TYPE;
    v->c_type = C_COLOR;
    return v;
}

static Lval_t* lval_create_exit(void) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_EXIT;
    return v;
}

static Lval_t* lval_create_bool(bool x) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_BOOL;
    v->num.li = x;
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

static Lval_t* lval_read_str(mpc_ast_t* ast) {
    ast->contents[strlen(ast->contents) - 1] = '\0';

    // copy the string without the first quotation mark
    char* unescaped = malloc(strlen(ast->contents + 1) + 1);
    strcpy(unescaped, ast->contents + 1);

    unescaped = mpcf_unescape(unescaped);
    Lval_t* str = lval_create_str(unescaped);
    free(unescaped);
    return str;
}

static Color_t color_from_list(Lval_t* l) {
    return (Color_t){
        (char)l->cell[0]->num.li,
        (char)l->cell[1]->num.li,
        (char)l->cell[2]->num.li,
        (char)l->cell[3]->num.li,
    };
}

static Lval_t* color_to_list(Color_t* c) {
    Lval_t* l = lval_create_qexpr();
    lval_add(l, lval_create_long(c->r));
    lval_add(l, lval_create_long(c->g));
    lval_add(l, lval_create_long(c->b));
    lval_add(l, lval_create_long(c->a));
    return l;
}

static bool lval_type_2_ctype(Lval_t* input, CTypes_e* ret) {
    switch (input->type) {
        case LVAL_BOOL:
        case LVAL_INTEGER: {
            *ret = C_INT;
            return true;
        }

        case LVAL_DECIMAL: {
            *ret = C_DOUBLE;
            return true;
        }

        case LVAL_STR: {
            *ret = C_STRING;
            return true;
        }

        case LVAL_QEXPR: {
            if (input->count == 0) {
                *ret = C_VOID;
                return true;
            } else if (input->count == 4) {
                *ret = C_COLOR;
                return true;
            }
        }

        case LVAL_SEXPR: {
            if (input->count == 0) {
                *ret = C_VOID;
                return true;
            }
        }

        default: return false;
    }

    return false;
}

static void ffi_call_extern(Lval_t* fn, CTypes_e* atypes, Lval_t* inputs, void* ret) {
    void *avalues[inputs->count];
    for (int i = 0; i < inputs->count; i++) {
        switch (atypes[i]) {
            case C_VOID: {
                avalues[i] = NULL;
                break;
            }
            case C_INT: {
                avalues[i] = &inputs->cell[i]->num.li;
                break;
            }
            case C_DOUBLE: {
                avalues[i] = &inputs->cell[i]->num.f;
                break;
            }
            case C_STRING: {
                avalues[i] = &inputs->cell[i]->str;
                break;
            }
            case C_COLOR: {
                Color_t c = color_from_list(inputs->cell[i]);
                avalues[i] = &c;
                break;
            }
            default:
                fprintf(stderr, "You added a new C-type, but forgot to add it to %s!", __func__);
                assert(false);
        }
    }

    ffi_call(fn->cif, FFI_FN(fn->extern_ptr), ret, avalues);
}

static Lval_t* lval_call_extern(Lenv_t* e, Lval_t* fn, Lval_t* inputs) {
    int n_given = inputs->count;
    int n_expct = fn->formals->count;

    if (n_given != n_expct) {
        lval_del(inputs);
        return lval_create_err("Extern function expects [%i] args, got [%i].", n_expct, n_given);
    }

    CTypes_e atypes[n_given];
    for (int i = 0; i < n_given; ++i) {
        bool ret = lval_type_2_ctype(inputs->cell[i], &atypes[i]);
        Lval_t* arg_type = lenv_get(e, fn->formals->cell[i]);
        bool okay = ret && arg_type->c_type == atypes[i];
        if (!okay) free(atypes);
        LASSERT(inputs, okay, "Extern func `%s` got input arg [%i] of type [%s], expected [%s]",
                              __func__, i + 1, TYPE_NAME_LUT[atypes[i]], TYPE_NAME_LUT[arg_type->c_type]);
    }

    Lval_t* out = lenv_get(e, fn->body->cell[0]);

    switch (out->c_type) {
        case C_VOID: {
            ffi_call_extern(fn, atypes, inputs, NULL);
            return lval_create_ok();
        }
        case C_INT: {
            long ret = 0;
            ffi_call_extern(fn, atypes, inputs, &ret);
            return lval_create_long(ret);
        }
        case C_DOUBLE: {
            double ret;
            ffi_call_extern(fn, atypes, inputs, &ret);
            return lval_create_double(ret);
        }
        case C_STRING: {
            char *ret;
            ffi_call_extern(fn, atypes, inputs, &ret);
            return lval_create_str(ret);
        }
        case C_COLOR: {
            Color_t *ret;
            ffi_call_extern(fn, atypes, inputs, &ret);
            return color_to_list(ret);
        }
        default:
            fprintf(stderr, "You added a new C-type, but forgot to add it to %s!", __func__);
            assert(false);
    }

    return lval_create_err("UNREACHABLE");
}

/*
    Dispatches function calls based on whether it's a builtin, externally linked one, or user-defined
*/
static Lval_t* lval_call(Lenv_t* e, Lval_t* fn, Lval_t* a) {
    if (fn->builtin != NULL) return fn->builtin(e, a);
    if (fn->is_extern) return lval_call_extern(e, fn, a);

    int n_given = a->count;
    while (a->count) {
        if (fn->formals->count == 0) {
            lval_del(a);
            return lval_create_err(
                // TODO: figure out how to provide the name of the function
                "Function `user-defined` expects [%i] args, got [%i].", fn->formals->count, n_given);
        }

        Lval_t* sym = lval_pop(fn->formals, 0);

        /* Special case where we have a variable number of arguments, syntax `sym & syms` */
        if (strncmp(sym->sym, "&", 2) == 0) {
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
    if (fn->formals->count > 0 && strncmp(fn->formals->cell[0]->sym, "&", 2) == 0) {
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
        if (!IS_NUM(a, i)) {
            char* type = ltype_name(a->cell[i]->type);
            lval_del(a);
            return lval_create_err("Operator `%s` cannot operate on non-numbers; "
                                   "Arg [%i] is of type [%s]", op, i + 1, type);
        }
    }

    Lval_t* x = lval_pop(a, 0);
    if (x->type == LVAL_BOOL) x->type = LVAL_INTEGER;

    // no arguments provided and `op` is `-` then perform negation
    if ((strncmp(op, "-", 2) == 0) && a->count == 0) {
        if (x->type == LVAL_INTEGER) x->num.li = -x->num.li;
        else if (x->type == LVAL_DECIMAL) x->num.f = -x->num.f;
    }

    while (a->count > 0) {
        Lval_t* y = lval_pop(a, 0);
        if (y->type == LVAL_BOOL) y->type = LVAL_INTEGER;

        // if any one of inuts is a decimal, output will be decimal
        if (x->type == LVAL_DECIMAL || y->type == LVAL_DECIMAL) {
            if (y->type != LVAL_DECIMAL) {
                y->type = LVAL_DECIMAL; 
                y->num.f = (double)y->num.li;
            } else if (x->type != LVAL_DECIMAL) {
                x->type = LVAL_DECIMAL;
                x->num.f = (double)x->num.li;
            }

            if      (strncmp(op, "+", 2) == 0) x->num.f += y->num.f;
            else if (strncmp(op, "-", 2) == 0) x->num.f -= y->num.f;
            else if (strncmp(op, "*", 2) == 0) x->num.f *= y->num.f;
            else if (strncmp(op, "^", 2) == 0) x->num.f = pow(x->num.f, y->num.f);
            else if (strncmp(op, "%", 2) == 0) {
                if (y->num.f == 0.0) {
                    lval_del(x);
                    lval_del(y);
                    x = lval_create_err("Right-hand operand of '%s' cannot be 0!", op);
                    break;
                }
                x->num.f = fmod(x->num.f, y->num.f);
            }
            else if (strncmp(op, "/", 2) == 0) {
                if (y->num.f == 0.0) {
                    lval_del(x);
                    lval_del(y);
                    x = lval_create_err("Division By Zero!");
                    break;
                }
                x->num.f /= y->num.f;
            }
        } else {
            if      (strncmp(op, "+", 2) == 0) x->num.li += y->num.li;
            else if (strncmp(op, "-", 2) == 0) x->num.li -= y->num.li;
            else if (strncmp(op, "*", 2) == 0) x->num.li *= y->num.li;
            else if (strncmp(op, "^", 2) == 0) x->num.li = (long)pow(x->num.li, y->num.li);
            else if (strncmp(op, "%", 2) == 0) {
                if (y->num.li == 0) {
                    lval_del(x);
                    lval_del(y);
                    x = lval_create_err("Right-hand operand of '%s' cannot be 0!", op);
                    break;
                }
                x->num.li %= y->num.li;
            }
            else if (strncmp(op, "/", 2) == 0) {
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
    LASSERT(a, a->count >= 2, "Function `%s` expects at least 2 arguments, got [%i]", __func__, a->count);
    for (int i = 0; i < a->count; ++i) {
        LASSERT(a, IS_NUM(a, i), "Function `%s` expects arguments of type Number,"
                                 " but arg [%i] is of type [%s]", __func__,
                                 i + 1, ltype_name(a->cell[i]->type));
    }

    Lval_t* x = lval_pop(a, 0);
    for (int i = 0; i < a->count; ++i) {
        Lval_t* y = a->cell[i];

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
    }

    lval_del(a);
    return x;
}

static Lval_t* builtin_max(Lenv_t* e, Lval_t* a) {
    LASSERT(a, a->count >= 2, "Function `%s` expects at least 2 arguments, got [%i]", __func__, a->count);
    for (int i = 0; i < a->count; ++i) {
        LASSERT(a, IS_NUM(a, i), "Function `%s` expects arguments of type Number,"
                                 " but arg [%i] is of type [%s]", __func__,
                                 i + 1, ltype_name(a->cell[i]->type));
    }

    Lval_t* x = lval_pop(a, 0);
    for (int i = 0; i < a->count; ++i) {
        Lval_t* y = a->cell[i];

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

    Lval_t* x = a->cell[0];
    LASSERT(a, IS_NUM(a, 0), "Operator `%s` expects arguments of type Number,"
                             " but arg [%i] is of type [%s]",
                             op, 1, ltype_name(x->type));

    Lval_t* y = a->cell[1];
    LASSERT(a, IS_NUM(a, 1), "Operator `%s` expects arguments of type Number,"
                             " but arg [%i] is of type [%s]",
                             op, 2, ltype_name(y->type));

    Lval_t* res = lval_create_bool(false);
    if (x->type == LVAL_DECIMAL || y->type == LVAL_DECIMAL) {
        if (y->type != LVAL_DECIMAL) {
            y->type = LVAL_DECIMAL;
            y->num.f = (double)y->num.li;
        } else if (x->type != LVAL_DECIMAL) {
            x->type = LVAL_DECIMAL;
            x->num.f = (double)x->num.li;
        }
        if (strncmp(op, ">", 2) == 0)       res->num.li = x->num.f > y->num.f;
        else if (strncmp(op, "<", 2) == 0)  res->num.li = x->num.f < y->num.f;
        else if (strncmp(op, ">=", 3) == 0) res->num.li = x->num.f >= y->num.f;
        else if (strncmp(op, "<=", 3) == 0) res->num.li = x->num.f <= y->num.f;
        else if (strncmp(op, "&&", 3) == 0) res->num.li = x->num.f && y->num.f;
        else if (strncmp(op, "||", 3) == 0) res->num.li = x->num.f || y->num.f;
    } else {
        if (strncmp(op, ">", 2) == 0)       res->num.li = x->num.li > y->num.li;
        else if (strncmp(op, "<", 2) == 0)  res->num.li = x->num.li < y->num.li;
        else if (strncmp(op, ">=", 3) == 0) res->num.li = x->num.li >= y->num.li;
        else if (strncmp(op, "<=", 3) == 0) res->num.li = x->num.li <= y->num.li;
        else if (strncmp(op, "&&", 3) == 0) res->num.li = x->num.li && y->num.li;
        else if (strncmp(op, "||", 3) == 0) res->num.li = x->num.li || y->num.li;
    }

    lval_del(a);
    return res;
}

static Lval_t* builtin_not(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 1);
    if (a->cell[0]->type == LVAL_DECIMAL) {
        a->cell[0]->type = LVAL_BOOL;
        a->cell[0]->num.li = (long)a->cell[0]->num.f;
    } else if (a->cell[0]->type == LVAL_INTEGER) {
        a->cell[0]->type = LVAL_BOOL;
        a->cell[0]->num.li = (long)a->cell[0]->num.li;
    }
    LASSERT_TYPE(__func__, a, 0, LVAL_BOOL);

    bool res = !(bool)(a->cell[0]->num.li);
    lval_del(a);
    return lval_create_bool(res);
}

/*
    evaluate the equality of two Lvals
*/
static int lval_eq(Lval_t* x, Lval_t* y) {
    if (x->type != y->type) return 0;

    switch (x->type) {
        case LVAL_BOOL:
        case LVAL_INTEGER:
            return x->num.li == y->num.li;
        case LVAL_DECIMAL: return x->num.f == y->num.f;

        case LVAL_STR: {
            int l1, l2;
            if ((l1 = strlen(x->str)) != (l2 = strlen(y->str))) return false;
            return strncmp(x->str, y->str, l1) == 0;
        }
        case LVAL_ERR: {
            int l1, l2;
            if ((l1 = strlen(x->err)) != (l2 = strlen(y->err))) return false;
            return strncmp(x->err, y->err, l1) == 0;
        }
        case LVAL_SYM: {
            int l1, l2;
            if ((l1 = strlen(x->sym)) != (l2 = strlen(y->sym))) return false;
            return strncmp(x->sym, y->sym, l1) == 0;
        }

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

        case LVAL_DLL:  return x->dll == y->dll;
        case LVAL_TYPE: return x->c_type == y->c_type;

        case LVAL_EXIT: return 1;
        case LVAL_OK: return 1;
        default:
            fprintf(stderr, "You added a new type, but forgot to add it to %s!", __func__);
            assert(false);
    }
    return 0;
}

static Lval_t* builtin_eq(Lenv_t* e, Lval_t* a) { return builtin_cmp(e, a, "=="); }
static Lval_t* builtin_ne(Lenv_t* e, Lval_t* a) { return builtin_cmp(e, a, "!="); }

static Lval_t* builtin_cmp(Lenv_t* e, Lval_t* a, char* op) {
    LASSERT_NUM(op, a, 2);

    bool res;
    if (strncmp(op, "==", 3) == 0) res = lval_eq(a->cell[0], a->cell[1]);
    if (strncmp(op, "!=", 3) == 0) res = !lval_eq(a->cell[0], a->cell[1]);

    lval_del(a);
    return lval_create_bool(res);
}

static Lval_t* builtin_if(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 3);

    if (a->cell[0]->type == LVAL_DECIMAL) {
        a->cell[0]->type = LVAL_BOOL;
        a->cell[0]->num.li = (long)a->cell[0]->num.f;
    }
    if (a->cell[0]->type == LVAL_INTEGER) a->cell[0]->type = LVAL_BOOL;
 
    LASSERT_TYPE(__func__, a, 0, LVAL_BOOL);
    LASSERT_TYPE(__func__, a, 1, LVAL_QEXPR);
    LASSERT_TYPE(__func__, a, 2, LVAL_QEXPR);

    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;

    Lval_t* x = a->cell[0]->num.li ? lval_eval(e, lval_pop(a, 1))
                                   : lval_eval(e, lval_pop(a, 2));
    lval_del(a);
    return x;
}

static Lval_t* builtin_head(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 1);

    Lval_t* v = a->cell[0];
    LASSERT(a, IS_ITERABLE(a, 0),
            "Function `%s` expects arguments of type [%s, %s], "
            "but arg [1] is of type [%s]", __func__, ltype_name(LVAL_QEXPR),
            ltype_name(LVAL_STR), ltype_name(v->type));
    bool cond = (v->type == LVAL_QEXPR && v->count != 0)
             || (v->type == LVAL_STR && strlen(v->str) > 0);
    LASSERT(a, cond, "Function `%s` expects a non-empty [%s, %s]!",
                     __func__, ltype_name(LVAL_QEXPR), ltype_name(LVAL_STR));

    v = lval_take(a, 0);  // upacks the input Q-expression
    switch (v->type) {
        case LVAL_QEXPR: {
            while (v->count > 1) {
                lval_del(lval_pop(v, 1));
            }
            break;
        }
        case LVAL_STR: {
            v->str = realloc(v->str, 2);
            v->str[1] = '\0';
            break;
        }
        default:
            fprintf(stderr, "Function %s doesn't support this type %s\n",
                            __func__, ltype_name(v->type));
            assert(false);
    }
    return v;
}

static Lval_t* builtin_tail(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 1);

    Lval_t* v = a->cell[0];
    LASSERT(a, IS_ITERABLE(a, 0),
            "Function `%s` expects arguments of type [%s, %s], "
            "but arg [1] is of type [%s]", __func__, ltype_name(LVAL_QEXPR),
            ltype_name(LVAL_STR), ltype_name(v->type));
    bool cond = (v->type == LVAL_QEXPR && v->count != 0)
             || (v->type == LVAL_STR && strlen(v->str) > 0);
    LASSERT(a, cond, "Function `%s` expects a non-empty [%s, %s]!",
                    __func__, ltype_name(LVAL_QEXPR), ltype_name(LVAL_STR));

    v = lval_take(a, 0);  // upacks the input Q-expression
    switch (v->type) {
        case LVAL_QEXPR: {
            lval_del(lval_pop(v, 0));
            break;
        }
        case LVAL_STR: {
            const size_t len = strlen(v->str);
            strncpy(v->str, v->str + 1, len);
            v->str = realloc(v->str, len);
            break;
        }
        default:
            fprintf(stderr, "Function %s doesn't support this type %s\n",
                            __func__, ltype_name(v->type));
            assert(false);
    }
    return v;
}

static Lval_t* builtin_list(Lenv_t* e, Lval_t* a) {
    a->type = LVAL_QEXPR;
    return a;
}

static Lval_t* builtin_eval(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR);

    Lval_t* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

static Lval_t* builtin_join(Lenv_t* e, Lval_t* a) {
    for (int i = 0; i < a->count; ++i) {
        LASSERT(a, IS_ITERABLE(a, i),
        "Function `%s` expects arguments of type [%s, %s], "
        "but arg [%i] is of type [%s]", __func__, ltype_name(LVAL_QEXPR),
        ltype_name(LVAL_STR), i + 1, ltype_name(a->cell[i]->type));
    }

    Lval_t* x = lval_pop(a, 0);
    for (int i = 0; i < a->count; ++i) {
        LASSERT(a, x->type == a->cell[i]->type,
        "Function `%s` expects all arguments to be of the same type, arg [%i] "
        "is of type [%s] which is different than 1st element's type [%s]", __func__,
        i + 2, ltype_name(a->cell[i]->type), ltype_name(x->type));
    }

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

    Lval_t* symbols = a->cell[0];

    for (int i = 0; i < symbols->count; ++i) {
        LASSERT(a, symbols->cell[i]->type == LVAL_SYM,
            "Function `%s` cannot define a non-symbol; arg number [%i] "
            "is of type [%s]", fn, i + 1, ltype_name(symbols->cell[i]->type));
    }

    LASSERT(a, symbols->count == a->count - 1,
        "Function `%s` expects #symbols == #values; "
        "we got [%i] symbols, and [%i] values",
        fn, symbols->count, a->count - 1);

    for (int i = 0; i < symbols->count; ++i) {
        LASSERT(a, !_lookup_builtin_name(symbols->cell[i]->sym), 
            "Function `%s` cannot define arg number [%i] named '%s'; builtin keyword!",
            fn, i + 1, symbols->cell[i]->sym);
    }

    for (int i = 0; i < symbols->count; ++i) {
        if (strncmp(fn, "def", 4) == 0) lenv_def(e, symbols->cell[i], a->cell[i + 1]);
        if (strncmp(fn, "=", 2) == 0)   lenv_put(e, symbols->cell[i], a->cell[i + 1]);
    }

    lval_del(a);
    return lval_create_ok();
}

static Lval_t* builtin_fn(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 2);
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR);
    LASSERT_TYPE(__func__, a, 1, LVAL_QEXPR);

    Lval_t* symbols = a->cell[0];

    /* First Q-expr must only contain symbols */
    for (int i = 0; i < symbols->count; ++i) {
        LASSERT(a, (symbols->cell[i]->type == LVAL_SYM),
            "Function `%s` cannot define arg [%i] of type [%s], expected [%s]",
            __func__, i + 1, ltype_name(symbols->cell[i]->type), ltype_name(LVAL_SYM));
    }

    for (int i = 0; i < symbols->count; ++i) {
        LASSERT(a, !_lookup_builtin_name(symbols->cell[i]->sym), 
            "Function `%s` cannot define arg number [%i] named '%s'; builtin keyword!",
            __func__, i + 1, symbols->cell[i]->sym);
    }

    Lval_t* fn_name = lval_pop(symbols, 0);
    Lval_t* formals = lval_pop(a, 0);
    Lval_t* body = lval_pop(a, 0);
    lenv_def(e, fn_name, lval_create_lambda(formals, body));
    lval_del(a);
    return lval_create_ok();
}

static Lval_t* builtin_lambda(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 2);
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR);
    LASSERT_TYPE(__func__, a, 1, LVAL_QEXPR);

    /* First Q-expr must only contain symbols */
    for (int i = 0; i < a->cell[0]->count; ++i) {
        LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
            "Function `%s` cannot define arg [%i] of type [%s]. Expected type [%s]",
            __func__, i + 1, ltype_name(a->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
    }

    Lval_t* formals = lval_pop(a, 0);
    Lval_t* body = lval_pop(a, 0);
    lval_del(a);

    return lval_create_lambda(formals, body);
}

static Lval_t* lval_join(Lval_t* x, Lval_t* y) {
    switch (x->type) {
        case LVAL_QEXPR: {
            while (y->count) { x = lval_add(x, lval_pop(y, 0)); }
            break;
        }
        case LVAL_STR: {
            const size_t l1 = strlen(x->str);
            const size_t l2 = strlen(y->str);
            x->str = realloc(x->str, l1 + l2 + 1);
            strncpy(x->str + l1, y->str, l2 + 1);
            break;
        }
        default:
            fprintf(stderr, "%s doesn't support the type %s", __func__, ltype_name(x->type));
            assert(false);
    }
    lval_del(y);
    return x;
}

static Lval_t* lval_copy(Lval_t* v) {
    Lval_t* x = malloc(sizeof(Lval_t));
    x->type = v->type;

    switch (v->type) {
        case LVAL_FN: {
            x->is_extern = v->is_extern;

            if (v->builtin != NULL) {
                x->builtin = v->builtin;
            } else {
                x->builtin = NULL;
                x->cif = malloc(sizeof(ffi_cif));
                memcpy(x->cif, v->cif, sizeof(ffi_cif));
                x->atypes = malloc(v->formals->count * sizeof(ffi_type*));
                memcpy(x->atypes, v->atypes, v->formals->count * sizeof(ffi_type*));
                x->extern_ptr = v->extern_ptr;
                x->env = lenv_copy(v->env);
                x->formals = lval_copy(v->formals);
                x->body = lval_copy(v->body);
            }
            break;
        }

        case LVAL_DLL:       x->dll = v->dll; break;
        case LVAL_TYPE:      x->c_type = v->c_type; break;
        case LVAL_DECIMAL:   x->num.f = v->num.f; break;

        case LVAL_BOOL:
        case LVAL_INTEGER:   x->num.li = v->num.li; break;

        case LVAL_STR: {
            x->str = malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str);
            break;
        }
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

        case LVAL_OK:
        case LVAL_EXIT: break;

        default:
            fprintf(stderr, "You added a new type, but forgot to add it to %s!", __func__);
            assert(false);
    }
    return x;
}

static void lenv_add_builtin_const(Lenv_t* e, char* name, Lval_t* val) {
    lenv_def(e, lval_create_sym(name), val);
}

static void lenv_add_builtin(Lenv_t* e, char* name, Lbuiltin_t fn) {
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

/*
    NOTE: this doesn't check if a symbol has been registered already.
*/
static void _register_builtin_name(char* name) {
    size_t name_len = strlen(name);
    __builtins__.count++;
    __builtins__.lengths = realloc(__builtins__.lengths, sizeof(int*) * __builtins__.count);
    __builtins__.lengths[__builtins__.count - 1] = name_len;
    __builtins__.names = realloc(__builtins__.names, sizeof(char*) * __builtins__.count);
    __builtins__.names[__builtins__.count - 1] = malloc(name_len + 1);
    strcpy(__builtins__.names[__builtins__.count - 1], name);
}

static bool _lookup_builtin_name(char* name) {
    const size_t l = strlen(name);
    for (int i = 0; i < __builtins__.count; ++i) {
        if (__builtins__.lengths[i] == l && strncmp(__builtins__.names[i], name, l) == 0) {
            return true;
        }
    }
    return false;
}

static char* ltype_name(LVAL_e t) {
    switch (t) {
        case LVAL_INTEGER:  return "Int";
        case LVAL_DECIMAL:  return "Float";
        case LVAL_BOOL:     return "Boolean";
        case LVAL_ERR:      return "Error";
        case LVAL_STR:      return "String";
        case LVAL_SYM:      return "Symbol";
        case LVAL_FN:       return "Function";
        case LVAL_SEXPR:    return "S-Expression";
        case LVAL_QEXPR:    return "Q-Expression";
        case LVAL_EXIT:     return "Exit";
        case LVAL_OK:       return "OK";
        case LVAL_DLL:      return "DLL";
        case LVAL_TYPE:     return "C_Type";
        default:
            fprintf(stderr, "You added a new type, but forgot to add it to %s!", __func__);
            assert(false);
    }
}

/*
    The goal of this function is to print the input string
    (from the user) after evaluating the escape sequences
*/
static void lval_print_str(Lval_t* v) {
    char* escaped = malloc(strlen(v->str) + 1);
    strcpy(escaped, v->str);

    escaped = mpcf_escape(escaped);
    printf("\"%s\"", escaped);
    free(escaped);
}

static char* freadline(FILE* fp, size_t size) {
    char* str;
    int ch;
    size_t len = 0;

    str = realloc(NULL, sizeof(*str) * size);
    if(!str) return str;

    while(EOF != (ch = fgetc(fp)) && ch != '\n') {
        str[len++] = ch;
        if(len == size){
            str = realloc(str, sizeof(*str) * (size *= 2));
            if(!str) return str;
        }
    }
    str[len++] = '\0';

    return realloc(str, sizeof(*str) * len);
}

static Lval_t* builtin_read(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR);

    Lval_t* v = a->cell[0];
    LASSERT_NUM(__func__, v, 1);
    LASSERT(a, (v->cell[0]->type == LVAL_SYM),
            "Function `%s` cannot define arg [%i] of type [%s], expected [%s]",
            __func__, 1, ltype_name(v->cell[0]->type), ltype_name(LVAL_SYM));

    Lval_t* sym = lval_pop(v, 0);
    lval_del(a);

    if (_lookup_builtin_name(sym->sym)) {
        LASSERT(sym, false, "Function `%s` cannot define arg named '%s'; "
                            "builtin keyword!", __func__, sym->sym);
    }

    char* s = freadline(stdin, READ_BUF_LEN);
    if (s) {
        lenv_put(e, sym, lval_create_str(s));
        free(s);
        return lval_create_ok();
    } else {
        return lval_create_err("Function `%s` coudn't read input string", __func__);
    }
}

static Lval_t* builtin_print(Lenv_t* e, Lval_t* a) {
    for (int i = 0; i < a->count; ++i) {
        lval_print(a->cell[i]);
        putchar(' ');
    }
    putchar('\n');
    lval_del(a);
    return lval_create_ok();
}

static Lval_t* builtin_error(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPE(__func__, a, 0, LVAL_STR);
    Lval_t* err = lval_create_err(a->cell[0]->str);
    lval_del(a);
    return err;
}

static Lval_t* builtin_type(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 1);
    LASSERT_TYPE(__func__, a, 0, LVAL_QEXPR);

    Lval_t* v = a->cell[0];
    LASSERT_NUM(__func__, v, 1);

    Lval_t* val = lval_pop(v, 0);
    lval_del(a);

    switch (val->type) {
        case LVAL_INTEGER:
        case LVAL_DECIMAL:
        case LVAL_STR:
        case LVAL_ERR:
        case LVAL_EXIT:
        case LVAL_OK:
        case LVAL_BOOL:
        case LVAL_SEXPR:
        case LVAL_QEXPR:
        case LVAL_DLL:
        case LVAL_TYPE:
            return lval_create_str(ltype_name(val->type));

        case LVAL_SYM:
        case LVAL_FN: {
            for (int i = 0; i < e->count; ++i) {
                if (strcmp(e->syms[i], val->sym) == 0) {
                    return lval_create_str(ltype_name(e->vals[i]->type));
                }
            }
        }
    }

    LASSERT(val, false, "Function `%s` cannot find arg [%s]", __func__, val->sym);
}

static Lval_t* lval_create_dll(void* dll) {
    Lval_t* v = malloc(sizeof(Lval_t));
    v->type = LVAL_DLL;
    v->dll = dll;
    return v;
}

// Ref: https://linux.die.net/man/3/dlopen
static Lval_t* builtin_dll(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__, a, 2);
    LASSERT_TYPE(__func__, a, 0, LVAL_STR);
    LASSERT_TYPE(__func__, a, 1, LVAL_STR);

    char* name = a->cell[0]->str;
    char* path = a->cell[1]->str;
    void* dll = dlopen(path, RTLD_NOW|RTLD_GLOBAL);
    if (!dll) {
        return lval_create_err("[%s] -- Couldn't load DLL `%s`. ERROR: %s", __func__, name, dlerror());
    }
    dlerror();

    Lval_t* dll_name = lval_create_str(name);
    lenv_def(e, dll_name, lval_create_dll(dll));
    lval_del(a);
    return lval_create_ok();
}

ffi_type* ctype_2_ffi_type(CTypes_e c_type) {
    switch (c_type) {
        case C_VOID:   return &ffi_type_void;
        case C_INT:    return &ffi_type_slong;
        case C_DOUBLE: return &ffi_type_double;
        case C_STRING: return &ffi_type_pointer;
        case C_COLOR:  return &ffi_color_type;
        default:
            fprintf(stderr, "You're asking for an FFI equivalent to a ctype that's not handled %s!", __func__);
            assert(false);
    }
}

char* ffi_type_2_str(ffi_type* t) {
    if (t == &ffi_type_void)    return "ffi_type_void";
    if (t == &ffi_type_slong)   return "ffi_type_slong";
    if (t == &ffi_type_double)  return "ffi_type_double";
    if (t == &ffi_type_pointer) return "ffi_type_pointer";
    if (t == &ffi_color_type)   return "ffi_color_type";
    return "unknown";
}

static Lval_t* builtin_extern(Lenv_t* e, Lval_t* a) {
    LASSERT_NUM(__func__,  a, 4);
    LASSERT_TYPE(__func__, a, 0, LVAL_DLL);
    LASSERT_TYPE(__func__, a, 1, LVAL_STR);
    LASSERT_TYPE(__func__, a, 2, LVAL_QEXPR);
    LASSERT_TYPE(__func__, a, 3, LVAL_QEXPR);

    void* dll = a->cell[0]->dll;
    Lval_t* fn_name = lval_pop(a, 1);
    Lval_t* inputs = lval_pop(a, 1);
    Lval_t* outputs = lval_pop(a, 1);

    Lval_t** input_types = malloc(sizeof(Lval_t*) * inputs->count);
    for (int i = 0; i < inputs->count; ++i) {
        input_types[i] = lenv_get(e, inputs->cell[i]);
        bool okay = input_types[i]->type == LVAL_TYPE;
        if (!okay) free(input_types);
        LASSERT(a, okay, "Extern def of func `%s` got input arg [%i] of type [%s], expected [%s]",
                         fn_name->str, i + 1, ltype_name(inputs->cell[i]->type), ltype_name(LVAL_TYPE));
    }

    LASSERT(a, (outputs->count <= 1), "Extern def of func `%s` got [%i] output args, "
                                      "only 1 is supported at the time", fn_name->str, outputs->count);

    Lval_t** output_types = malloc(sizeof(Lval_t*) * inputs->count);
    for (int i = 0; i < outputs->count; ++i) {
        output_types[i] = lenv_get(e, outputs->cell[i]);
        bool okay = output_types[i]->type == LVAL_TYPE;
        if (!okay) free(output_types);
        LASSERT(a, okay, "Extern def of func `%s` got output arg [%i] of type [%s], expected [%s]",
                         fn_name->str, i + 1, ltype_name(outputs->cell[i]->type), ltype_name(LVAL_TYPE));
    }

    void* ptr = dlsym(dll, fn_name->str);
    if (dlerror() != NULL) {
        return lval_create_err("[%s] -- Couldn't load symbol %s from DLL. ERROR: %s", __func__, fn_name->str, dlerror());
    }
    dlerror();

    ffi_type* rtype = ctype_2_ffi_type(output_types[0]->c_type);
    Lval_t* fn = lval_create_lambda(inputs, outputs);
    int n_args = inputs->count;

    ffi_status status;
    if (n_args == 1 && input_types[0]->c_type == C_VOID) {
        status = ffi_prep_cif(fn->cif, FFI_DEFAULT_ABI, 0, rtype, NULL);
    } else {
        for (int i = 0; i < n_args; ++i) {
            fn->atypes[i] = ctype_2_ffi_type(input_types[i]->c_type);
        }
        status = ffi_prep_cif(fn->cif, FFI_DEFAULT_ABI, n_args, rtype, fn->atypes);
    }
    if (status != FFI_OK) {
        return lval_create_err("[%s] -- Couldn't prep symbol %s through libffi `ffi_prep_cif`", __func__, fn_name->str);
    }

    fn->extern_ptr = ptr;
    fn->is_extern = true;
    lenv_def(e, fn_name, fn);

    free(input_types);
    free(output_types);

    return lval_create_ok();
}
