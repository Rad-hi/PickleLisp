#include "lang.h"

#define NUM_PARSERS 10
static mpc_parser_t* parsers[NUM_PARSERS];

mpc_parser_t* pickle_lisp;

// defined in eval.c
extern void _del_builtin_names();


static mpc_parser_t* create_lang(void);
static void load_std_library(Lenv_t* e);

/*
    Creates the language instance and the storage environment for it
*/
void create_vm(Lenv_t** e, mpc_parser_t** lang) {
    *lang = create_lang();
    *e = lenv_new();
    lenv_add_builtins(*e);
    load_std_library(*e);
    _register_builtin_names_from_env(*e);
}

void cleanup(void) {
    for (int i = 0; i < NUM_PARSERS; ++i) {
        mpc_cleanup(1, parsers[i]);
    }
    _del_builtin_names();
}


static mpc_parser_t* create_lang(void) {
    int i = 0;
    parsers[i++] = mpc_new("integer");
    parsers[i++] = mpc_new("decimal");
    parsers[i++] = mpc_new("number");
    parsers[i++] = mpc_new("string");
    parsers[i++] = mpc_new("comment");
    parsers[i++] = mpc_new("symbol");
    parsers[i++] = mpc_new("sexpr");
    parsers[i++] = mpc_new("qexpr");
    parsers[i++] = mpc_new("expr");
    parsers[i++] = mpc_new("lang");

    assert(i == NUM_PARSERS && "It seems you added a new rule"
                                "without updating `NUM_PARSERS`");

    mpca_lang(MPCA_LANG_DEFAULT,
        "integer : /-?[0-9]+/ ;"
        "decimal : /-?[0-9]*[.][0-9]*[fF]?/ ;"
        "number  : <decimal> | <integer> ;"
        "string  : /\"(\\\\.|[^\"])*\"/ ;"
        "comment : /;[^\\r\\n]*/ ;"
        "symbol  : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&^%|]+/ ;"
        "sexpr   : '(' <expr>* ')' ;"
        "qexpr   : '{' <expr>* '}' ;"
        "expr    : <number> | <symbol> | <sexpr> "
                "| <qexpr>  | <string> | <comment> ;"
        "lang    : /^/ <expr>* /$/ ;",
        parsers[0], parsers[1], parsers[2], parsers[3], parsers[4],
        parsers[5], parsers[6], parsers[7], parsers[8], parsers[9]
    );

    pickle_lisp = parsers[NUM_PARSERS - 1];
    return pickle_lisp;
}

static void load_std_library(Lenv_t* e) {
    Lval_t* arg = lval_add(lval_create_sexpr(), lval_create_str(STD_LIB_PATH));
    Lval_t* res = builtin_load(e, arg);
    if (res->type == LVAL_ERR) {
        lval_println(res);
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            strncpy(&cwd[0], "./", 3);
        }
        fprintf(stderr, "Cannot load the standard library. Did you change its location ?"
                        "Expected location: %s/%s\n", cwd, STD_LIB_PATH);
        exit(69);
    }
}

