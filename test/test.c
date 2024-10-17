#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../src/grammar.h"
#include "../src/eval.h"

#define EUPSILON                    0.000001
#define ALMOST_EQ(a, b)             fabs((a) - (b)) <= (EUPSILON)
#define PRINT_VERDICT(cond, name)   printf("[%s] [Test %s]\n", (cond) ? "PASSED" : "FAILED", (name));


static void assert_equal(Lval_t* val, Lval_t expected, char* test_name) {
  switch (expected.type) {
    case LVAL_ERR: {
      bool cond = (bool)strstr(val->err, expected.err);
      PRINT_VERDICT(cond, test_name);
#ifdef EXIT_ON_FAIL
      if (!cond) exit(-1);
#endif
      break;
    }

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

    case LVAL_SEXPR:
      // TODO: handle this case
    case LVAL_SYM:
      // TODO: handle this case
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

/*------------------*/
/* TESTS START HERE */
/*------------------*/

static void test_integer_addition(mpc_parser_t* language) {

  char* test_name = "integer addition";
  char* test_statement = "+ 1 1";
  Lval_t expected = get_lval_long(2);

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t* res = eval_ast(r.output);
    assert_equal(res, expected, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_decimal_addition(mpc_parser_t* language) {

  char* test_name = "decimal addition";
  char* test_statement = "+ 1. 3.5";
  Lval_t expected = get_lval_double(4.5);

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t* res = eval_ast(r.output);
    assert_equal(res, expected, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_heterogenous_addition(mpc_parser_t* language) {

  char* test_name = "heterogenous addition";
  char* test_statement = "+ 32 33. 2.4 .6 1";
  Lval_t expected = get_lval_double(69.);

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t* res = eval_ast(r.output);
    assert_equal(res, expected, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_all_operators(mpc_parser_t* language) {

  char* test_name = "all operators";
  char* test_statement = "+ 1 (* 1 2) (/ 4 2) (% 15 2) (- 0 6) (^ 3 2)";
  Lval_t expected = get_lval_long(9);

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t* res = eval_ast(r.output);
    assert_equal(res, expected, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_min_max(mpc_parser_t* language) {

  char* test_name = "min max";
  char* test_statement = "min 69 (max 666 420)";
  Lval_t expected = get_lval_long(69);

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t* res = eval_ast(r.output);
    assert_equal(res, expected, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_DivByZero_err(mpc_parser_t* language) {

  char* test_name = "DivByZero";
  char* test_statement = "/ 1. 0";
  Lval_t expected = get_lval_err("By Zero");

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t* res = eval_ast(r.output);
    assert_equal(res, expected, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_BadInput_err(mpc_parser_t* language) {

  char* test_name = "BadInput";
  char* test_statement = "% 1. 0.0";
  Lval_t expected = get_lval_err("cannot be 0");

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t* res = eval_ast(r.output);
    assert_equal(res, expected, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_NonNumber_err(mpc_parser_t* language) {

  char* test_name = "NonNumber";
  char* test_statement = "(/ ())";
  Lval_t expected = get_lval_err("non-number");

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t* res = eval_ast(r.output);
    assert_equal(res, expected, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_syntax_err(mpc_parser_t* language) {

  char* test_name = "syntax";
  char* test_statement = "$";

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    PRINT_VERDICT(false, test_name);
    exit(1);
  } else {
    PRINT_VERDICT(true, test_name);
  }
}

int main(int argc, char** argv) {

    mpc_parser_t* language = create_lang();

    test_integer_addition(language);
    test_decimal_addition(language);
    test_heterogenous_addition(language);
    test_all_operators(language);
    test_min_max(language);
    test_DivByZero_err(language);
    test_BadInput_err(language);
    test_syntax_err(language);
    test_NonNumber_err(language);

    cleanup();

}