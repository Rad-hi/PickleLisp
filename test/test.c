#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../src/lang.h"
#include "../src/core.h"

#define EUPSILON                    0.000001
#define ALMOST_EQ(a, b)             fabs((a) - (b)) <= (EUPSILON)
#define PRINT_VERDICT(cond, name)   printf("[%s] [Test %s]\n", (cond) ? "PASSED" : "FAILED", (name));

typedef struct {
    char* name;
    char* statement;
    Lval_t expected;
    char* fn;
} test_statement_t;


static void assert_equal(Lval_t* val, Lval_t expected, char* test_name) {
    switch (expected.type) {
        case LVAL_ERR: {
            bool cond = val->type == LVAL_ERR;
            PRINT_VERDICT(cond, test_name);
#ifdef EXIT_ON_FAIL
            if (!cond) exit(-1);
#endif
            break;
        }
        case LVAL_BOOL:
        case LVAL_INTEGER: {
            bool cond = val->num.li == expected.num.li;
            PRINT_VERDICT(cond, test_name);
#ifdef EXIT_ON_FAIL
            if (!cond) exit(-1);
#endif
            break;
        }
        case LVAL_DECIMAL: {
            bool cond = ALMOST_EQ(val->num.f, expected.num.f); 
            PRINT_VERDICT(cond, test_name);
#ifdef EXIT_ON_FAIL
            if (!cond) exit(-1);
#endif
            break;
        }
        case LVAL_STR: {
            bool cond = strncmp(val->str, expected.str, strlen(expected.str)) == 0;
            PRINT_VERDICT(cond, test_name);
#ifdef EXIT_ON_FAIL
            if (!cond) exit(-1);
#endif
            break;
        }
        case LVAL_OK: {
            bool cond = val->type == LVAL_OK;
            PRINT_VERDICT(cond, test_name);
#ifdef EXIT_ON_FAIL
            if (!cond) exit(-1);
#endif
            break;
        }
        case LVAL_SEXPR:
        case LVAL_QEXPR:
        case LVAL_SYM:
        case LVAL_FN:
        case LVAL_EXIT:
            break;
  }
}

static Lval_t get_lval_err(char* msg) {
    return (Lval_t){
        .type = LVAL_ERR,
        .err = msg
    };
}

static Lval_t get_lval_double(double value) {
    return (Lval_t){
        .type = LVAL_DECIMAL,
        .num.f = value
    };
}

static Lval_t get_lval_long(long value) {
    return (Lval_t){
        .type = LVAL_INTEGER,
        .num.li = value
    };
}

static Lval_t get_lval_bool(bool value) {
    return (Lval_t){
        .type = LVAL_BOOL,
        .num.li = value
    };
}

static Lval_t get_lval_str(char* s) {
    return (Lval_t){
        .type = LVAL_STR,
        .str = s
    };
}

static Lval_t get_lval_ok(void) {
    return (Lval_t){
        .type = LVAL_OK,
        .num.li = 1,
    };
}

/*------------------*/
/* TESTS START HERE */
/*------------------*/

static void test_integer_addition(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t t = {
        .name = "integer addition",
        .statement = "+ 1 1",
        .expected = get_lval_long(2),
    };

    mpc_result_t r;
    if (mpc_parse("test", t.statement, language, &r)) {
        Lval_t* res = lval_eval(e, lval_read(r.output));
        assert_equal(res, t.expected, t.name);
        mpc_ast_delete(r.output);
    } else {
        printf("[FAILED] Parsing error\n");
        PRINT_VERDICT(false, t.name);
#ifdef EXIT_ON_FAIL
        exit(1);
#endif
    }
}

static void test_decimal_addition(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t t = {
        .name = "decimal addition",
        .statement = "+ 1. 3.5",
        .expected = get_lval_double(4.5),
    };

    mpc_result_t r;
    if (mpc_parse("test", t.statement, language, &r)) {
        Lval_t* res = lval_eval(e, lval_read(r.output));
        assert_equal(res, t.expected, t.name);
        mpc_ast_delete(r.output);
    } else {
        printf("[FAILED] Parsing error\n");
        PRINT_VERDICT(false, t.name);
#ifdef EXIT_ON_FAIL
        exit(1);
#endif
    }
}

static void test_heterogenous_addition(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t t = {
        .name = "heterogenous addition",
        .statement = "+ 32 33. 2.4 .6 1",
        .expected = get_lval_double(69.),
    };

    mpc_result_t r;
    if (mpc_parse("test", t.statement, language, &r)) {
        Lval_t* res = lval_eval(e, lval_read(r.output));
        assert_equal(res, t.expected, t.name);
        mpc_ast_delete(r.output);
    } else {
        printf("[FAILED] Parsing error\n");
        PRINT_VERDICT(false, t.name);
#ifdef EXIT_ON_FAIL
        exit(1);
#endif
    }
}

static void test_all_operators(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t t = {
        .name = "all operators",
        .statement = "+ 1 (* 1 2) (/ 4 2) (% 15 2) (- 0 6) (^ 3 2)",
        .expected = get_lval_long(9),
    };

    mpc_result_t r;
    if (mpc_parse("test", t.statement, language, &r)) {
        Lval_t* res = lval_eval(e, lval_read(r.output));
        assert_equal(res, t.expected, t.name);
        mpc_ast_delete(r.output);
    } else {
        printf("[FAILED] Parsing error\n");
        PRINT_VERDICT(false, t.name);
#ifdef EXIT_ON_FAIL
        exit(1);
#endif
    }
}

static void test_min_max(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t t = {
        .name = "min max",
        .statement = "min 69 1200 (max 666 420 2334)",
        .expected = get_lval_long(69),
    };

    mpc_result_t r;
    if (mpc_parse("test", t.statement, language, &r)) {
        Lval_t* res = lval_eval(e, lval_read(r.output));
        assert_equal(res, t.expected, t.name);
        mpc_ast_delete(r.output);
    } else {
        printf("[FAILED] Parsing error\n");
        PRINT_VERDICT(false, t.name);
#ifdef EXIT_ON_FAIL
        exit(1);
#endif
    }
}

static void test_QExpressions(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t tests[] = {
        {
            .name = "QExpressions head-tail long int",
            .statement = "eval (head (tail (tail {5 6 7 8 3.5})))",
            .expected = get_lval_long(7)
        },
        {
            .name = "QExpressions head-tail double",
            .statement = "eval (head (tail (tail (tail {5 6 7 6.9 3.5}))))",
            .expected = get_lval_double(6.9)
        },
        {
            .name = "QExpressions list-head",
            // NOTE: 2 evals becaue first eval returns a Q-Expr, second returns an int
            .statement = "eval (eval {head (list 1 2)})",
            .expected = get_lval_long(1)
        },
        {
            .name = "QExpressions head-arithmetics",
            .statement = "eval (head {(+ 1 2) (+ 10 20)})",
            .expected = get_lval_long(3)
        },

        // keep this at the end
        {.statement = "end"},
    };

    int i = 0;
    while (strncmp(tests[i].statement, "end", 3) != 0) {
        mpc_result_t r;
        if (mpc_parse("test", tests[i].statement, language, &r)) {
            Lval_t* res = lval_eval(e, lval_read(r.output));
            assert_equal(res, tests[i].expected, tests[i].name);
            mpc_ast_delete(r.output);
        } else {
            PRINT_VERDICT(false, tests[i].name);
#ifdef EXIT_ON_FAIL
            exit(1);
#endif
        }
        i++;
    }
}

static void test_QExpressions_Strings(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t tests[] = {
        {
            .name = "QExpressions_Strings head-tail",
            .statement = "head (tail (tail \"56783.5\"))",
            .expected = get_lval_str("7")
        },
        {
            .name = "QExpressions_Strings head-tail err",
            .statement = "head (tail (tail (tail \"5676.93.5\" {})))",
            .expected = get_lval_err("")
        },
        {
            .name = "QExpressions_Strings join",
            .statement = "join \"1\" \"1\" \"34\" \"1\"",
            .expected = get_lval_str("11341")
        },
        {
            .name = "QExpressions_Strings join err",
            .statement = "join \"1\" \"1\" {69} \"34\" \"1\"",
            .expected = get_lval_err("")
        },

        // keep this at the end
        {.statement = "end"},
    };

    int i = 0;
    while (strncmp(tests[i].statement, "end", 3) != 0) {
        mpc_result_t r;
        if (mpc_parse("test", tests[i].statement, language, &r)) {
            Lval_t* res = lval_eval(e, lval_read(r.output));
            assert_equal(res, tests[i].expected, tests[i].name);
            mpc_ast_delete(r.output);
        } else {
            PRINT_VERDICT(false, tests[i].name);
#ifdef EXIT_ON_FAIL
            exit(1);
#endif
        }
        i++;
    }
}

static void test_Boolean(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t tests[] = {
        {
            .name = "Boolean `||`",
            .statement = "|| 1 0",
            .expected = get_lval_bool(true)
        },
        {
            .name = "Boolean `&&`",
            .statement = "&& 1 0",
            .expected = get_lval_bool(false)
        },
        {
            .name = "Boolean expressions",
            .statement = "|| (&& 0 false) (! true)",
            .expected = get_lval_bool(false)
        },

        // keep this at the end
        {.statement = "end"},
    };

    int i = 0;
    while (strncmp(tests[i].statement, "end", 3) != 0) {
        mpc_result_t r;
        if (mpc_parse("test", tests[i].statement, language, &r)) {
            Lval_t* res = lval_eval(e, lval_read(r.output));
            assert_equal(res, tests[i].expected, tests[i].name);
            mpc_ast_delete(r.output);
        } else {
            PRINT_VERDICT(false, tests[i].name);
#ifdef EXIT_ON_FAIL
            exit(1);
#endif
        }
        i++;
    }
}

static void test_Builtin_Symbol_Redefinition_Error(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t tests[] = {
        {
            .name = "Builtin_Symbol_Redefinition_Error `tail`",
            .statement = "def {tail} 69",
            .expected = get_lval_err("")
        },
        {
            .name = "Builtin_Symbol_Redefinition_Error `nil`",
            .statement = "def {nil} {}",
            .expected = get_lval_err("")
        },
        {
            .name = "Builtin_Symbol_Redefinition_Error `fn`",
            .statement = "def {fn} true",
            .expected = get_lval_err("")
        },

        // keep this at the end
        {.statement = "end"},
    };

    int i = 0;
    while (strncmp(tests[i].statement, "end", 3) != 0) {
        mpc_result_t r;
        if (mpc_parse("test", tests[i].statement, language, &r)) {
            Lval_t* res = lval_eval(e, lval_read(r.output));
            assert_equal(res, tests[i].expected, tests[i].name);
            mpc_ast_delete(r.output);
        } else {
            PRINT_VERDICT(false, tests[i].name);
#ifdef EXIT_ON_FAIL
            exit(1);
#endif
        }
        i++;
    }
}

static void test_StdLib(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t tests[] = {
        {
            .name = "StdLib `sum`",
            .statement = "sum (list 4 30 30 5)",
            .expected = get_lval_long(69)
        },
        {
            .name = "StdLib `mul`",
            .statement = "mul (list .5 2 9 3)",
            .expected = get_lval_double(27)
        },
        {
            .name = "StdLib `in`",
            .statement = "in 4 (list 1 2 3 4 5 6)",
            .expected = get_lval_bool(true)
        },
        {
            .name = "StdLib `swap`",
            .statement = "swap - 100 50",
            .expected = get_lval_long(-50)
        },
        {
            .name = "StdLib `filter`",
            .statement = "eval (filter (\\ {x} {> x 2}) {5 2 .3 -7 1.69 1})",
            .expected = get_lval_long(5)
        },
        {
            .name = "StdLib `nd/map`",
            .statement = "nd (map - {.4 5 6 7 8})",
            .expected = get_lval_long(-5)
        },

        // keep this at the end
        {.statement = "end"},
    };

    int i = 0;
    while (strncmp(tests[i].statement, "end", 3) != 0) {
        mpc_result_t r;
        if (mpc_parse("test", tests[i].statement, language, &r)) {
            Lval_t* res = lval_eval(e, lval_read(r.output));
            assert_equal(res, tests[i].expected, tests[i].name);
            mpc_ast_delete(r.output);
        } else {
            PRINT_VERDICT(false, tests[i].name);
#ifdef EXIT_ON_FAIL
            exit(1);
#endif
        }
        i++;
    }
}


static void test_Type_Inference(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t tests[] = {
        {
            .name = "Type_Inference `tail`",
            .statement = "type {tail}",
            .expected = get_lval_str("Function")
        },
        {
            .name = "Type_Inference `nil`",
            .statement = "type {nil}",
            .expected = get_lval_str("Q-Expression")
        },
        {
            .name = "Type_Inference `Float`",
            .statement = "type {3.}",
            .expected = get_lval_str("Float")
        },
        {
            .name = "Type_Inference `Int`",
            .statement = "type {69}",
            .expected = get_lval_str("Int")
        },

        // keep this at the end
        {.statement = "end"},
    };

    int i = 0;
    while (strncmp(tests[i].statement, "end", 3) != 0) {
        mpc_result_t r;
        if (mpc_parse("test", tests[i].statement, language, &r)) {
            Lval_t* res = lval_eval(e, lval_read(r.output));
            assert_equal(res, tests[i].expected, tests[i].name);
            mpc_ast_delete(r.output);
        } else {
            PRINT_VERDICT(false, tests[i].name);
#ifdef EXIT_ON_FAIL
            exit(1);
#endif
        }
        i++;
    }
}

static void test_Conditional(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t tests[] = {
        {
            .name = "Conditional simple if",
            .statement = "if true {+ 209 211} {- 1 1}",
            .expected = get_lval_long(420)
        },
        {
            .name = "Conditional simple if 2.0",
            .statement = "if false {+ 1 1} {+ 34 35}",
            .expected = get_lval_long(69)
        },
        {
            .name = "Conditional expressions",
            .statement = "if (== exit true) {+ 666 0} {* 1. .5}",
            .expected = get_lval_double(0.5)
        },

        // keep this at the end
        {.statement = "end"},
    };

    int i = 0;
    while (strncmp(tests[i].statement, "end", 3) != 0) {
        mpc_result_t r;
        if (mpc_parse("test", tests[i].statement, language, &r)) {
            Lval_t* res = lval_eval(e, lval_read(r.output));
            assert_equal(res, tests[i].expected, tests[i].name);
            mpc_ast_delete(r.output);
        } else {
            PRINT_VERDICT(false, tests[i].name);
#ifdef EXIT_ON_FAIL
            exit(1);
#endif
        }
        i++;
    }
}

/*
    NOTE: This test registers functions into the global environment
    be careful of using the same language instance after this test
*/
static void test_fn(mpc_parser_t* language, Lenv_t* e) {
    /*
        STEP ZERO: Define the unit test for the `sum` function
    */
    
    test_statement_t tests[] = {
        {
            .name = "fn fib",
            .statement = "fib 8",
            .expected = get_lval_long(21),
            .fn = "fn {fib n} { select { (== n 0) 0 } { (== n 1) 1 } { otherwise (+ (fib (- n 1)) (fib (- n 2))) } }"
        },
        {
            .name = "fn add",
            .statement = "add 2 2",
            .expected = get_lval_long(4),
            .fn = "fn {add a b} {+ a b}"
        },
        // keep this at the end
        {.statement = "end"},
    };

    mpc_result_t r;
    int i = 0;
    while (strncmp(tests[i].statement, "end", 3) != 0) {
        /*
            STEP ONE: Register the `.fn` custom function
        */
        if (mpc_parse("test", tests[i].fn, language, &r)) {
            Lval_t* res = lval_eval(e, lval_read(r.output));
            assert_equal(res, get_lval_ok(), "fn registration");
            mpc_ast_delete(r.output);
        } else {
            printf("[FAILED] Parsing error\n");
            PRINT_VERDICT(false, tests[i].name);
#ifdef EXIT_ON_FAIL
            exit(1);
#endif
        }

        /*
            STEP TWO: call the `.fn` custom function and test it
        */
        if (mpc_parse("test", tests[i].statement, language, &r)) {
            Lval_t* res = lval_eval(e, lval_read(r.output));
            assert_equal(res, tests[i].expected, tests[i].name);
            mpc_ast_delete(r.output);
        } else {
            PRINT_VERDICT(false, tests[i].name);
#ifdef EXIT_ON_FAIL
            exit(1);
#endif
        }
        i++;
    }
}

static void test_DivByZero_err(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t t = {
        .name = "DivByZero",
        .statement = "/ 1. 0",
        .expected = get_lval_err(""),
    };

    mpc_result_t r;
    if (mpc_parse("test", t.statement, language, &r)) {
        Lval_t* res = lval_eval(e, lval_read(r.output));
        assert_equal(res, t.expected, t.name);
        mpc_ast_delete(r.output);
    } else {
        printf("[FAILED] Parsing error\n");
        PRINT_VERDICT(false, t.name);
#ifdef EXIT_ON_FAIL
        exit(1);
#endif
    }
}

static void test_BadInput_err(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t t = {
        .name = "BadInput",
        .statement = "% 1. 0.",
        .expected = get_lval_err(""),
    };

    mpc_result_t r;
    if (mpc_parse("test", t.statement, language, &r)) {
        Lval_t* res = lval_eval(e, lval_read(r.output));
        assert_equal(res, t.expected, t.name);
        mpc_ast_delete(r.output);
    } else {
        printf("[FAILED] Parsing error\n");
        PRINT_VERDICT(false, t.name);
#ifdef EXIT_ON_FAIL
        exit(1);
#endif
    }
}

static void test_NonNumber_err(mpc_parser_t* language, Lenv_t* e) {
    test_statement_t t = {
        .name = "NonNumber",
        .statement = "(/ ())",
        .expected = get_lval_err(""),
    };

    mpc_result_t r;
    if (mpc_parse("test", t.statement, language, &r)) {
        Lval_t* res = lval_eval(e, lval_read(r.output));
        assert_equal(res, t.expected, t.name);
        mpc_ast_delete(r.output);
    } else {
        printf("[FAILED] Parsing error\n");
        PRINT_VERDICT(false, t.name);
#ifdef EXIT_ON_FAIL
        exit(1);
#endif
    }
}

static void test_syntax_err(mpc_parser_t* language, Lenv_t* e) {

    char* test_name = "syntax (testing mpc)";
    char* test_statement = "$";

    mpc_result_t r;
    if (mpc_parse("test", test_statement, language, &r)) {
        PRINT_VERDICT(false, test_name);
#ifdef EXIT_ON_FAIL
        exit(1);
#endif
    } else {
        PRINT_VERDICT(true, test_name);
    }
}

int main(int argc, char** argv) {

    mpc_parser_t* language = NULL;
    Lenv_t* e = NULL;
    create_vm(&e, &language);

    test_integer_addition(language, e);
    test_decimal_addition(language, e);
    test_heterogenous_addition(language, e);
    test_all_operators(language, e);
    test_min_max(language, e);
    test_DivByZero_err(language, e);
    test_BadInput_err(language, e);
    test_syntax_err(language, e);
    test_NonNumber_err(language, e);
    test_QExpressions(language, e);
    test_Boolean(language, e);
    test_Conditional(language, e);
    test_QExpressions_Strings(language, e);
    test_Builtin_Symbol_Redefinition_Error(language, e);
    test_Type_Inference(language, e);
    test_StdLib(language, e);
    test_fn(language, e);

    cleanup();
    lenv_del(e);
}