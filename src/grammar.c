#include "grammar.h"

static mpc_parser_t* parsers[NUM_PARSERS];

mpc_parser_t* create_lang() {
    int i = 0;
    parsers[i++] = mpc_new("integer");
    parsers[i++] = mpc_new("decimal");
    parsers[i++] = mpc_new("number");
    parsers[i++] = mpc_new("symbol");
    parsers[i++] = mpc_new("sexpr");
    parsers[i++] = mpc_new("qexpr");
    parsers[i++] = mpc_new("expr");
    parsers[i++] = mpc_new("lang");

    assert(i == NUM_PARSERS && "It seems you added a new rule"
                                "without updating `NUM_PARSERS`");

    mpca_lang(MPCA_LANG_DEFAULT,
        "integer : /-?[0-9]+/ ;"
        "decimal : /-?[0-9]*['.'][0-9]*[fF]?/ ;"
        "number  : <decimal> | <integer> ;"
        "symbol  : '+' | '-' | '*' | '/' | '%' | '^' | "
        "         \"min\" | \"max\" | \"list\" | \"head\" | "
        "         \"tail\" | \"join\" | \"eval\" ;"
        "sexpr   : '(' <expr>* ')' ;"
        "qexpr   : '{' <expr>* '}' ;"
        "expr    : <number> | <symbol> | <sexpr> | <qexpr> ;"
        "lang    : /^/ <expr>* /$/ ;",
        parsers[0], parsers[1], parsers[2], parsers[3],
        parsers[4], parsers[5], parsers[6], parsers[7]
    );

    return parsers[NUM_PARSERS - 1];
}

void cleanup() {
    for (int i = 0; i < NUM_PARSERS; ++i) {
        mpc_cleanup(1, parsers[i]);
    }
}