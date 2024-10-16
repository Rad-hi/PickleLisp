#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "../src/grammar.h"
#include "../src/eval.h"

#define EUPSILON                    0.000001
#define ALMOST_EQ(a, b)             fabs((a) - (b)) <= (EUPSILON)
#define PRINT_VERDICT(cond, name)   printf("[%s] [Test %s]\n", (cond) ? "PASSED" : "FAILED", (name));


static void assert_equal(Lval_t val, Lval_t expected, LVAL_e type, char* test_name) {
  switch (type) {
    case LVAL_ERR: {
      bool cond = val.err == expected.err;
      PRINT_VERDICT(cond, test_name);
#ifdef EXIT_ON_FAIL
      if (!cond) exit(-1);
#endif
      break;
    }

    case LVAL_INTEGER: {
      bool cond = val.num.li_num == expected.num.li_num;
      PRINT_VERDICT(cond, test_name);
#ifdef EXIT_ON_FAIL
      if (!cond) exit(-1);
#endif
      break;
    }

    case LVAL_DECIMAL: {
      bool cond = ALMOST_EQ(val.num.f_num, expected.num.f_num); 
      PRINT_VERDICT(cond, test_name);
#ifdef EXIT_ON_FAIL
      if (!cond) exit(-1);
#endif
      break;
    }
  }
}

/*------------------*/
/* TESTS START HERE */
/*------------------*/

static void test_integer_addition(mpc_parser_t* language) {

  char* test_name = "integer addition";
  char* test_statement = "+ 1 1";
  LVAL_e type = LVAL_INTEGER;

  /////////////////////////////////////////////////////
  // TODO: make this universal and less prone to errors
  /////////////////////////////////////////////////////

  Lval_t expected = {
    .type = type,
    .num.li_num = 2
  };

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t res = eval_ast(r.output);
    assert_equal(res, expected, type, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_decimal_addition(mpc_parser_t* language) {

  char* test_name = "decimal addition";
  char* test_statement = "+ 1. 3.5";
  LVAL_e type = LVAL_DECIMAL;
  Lval_t expected = {
    .type = type,
    .num.f_num = 4.5
  };

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t res = eval_ast(r.output);
    assert_equal(res, expected, type, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_heterogenous_addition(mpc_parser_t* language) {

  char* test_name = "heterogenous addition";
  char* test_statement = "+ 32 33. 2.4 .6 1";
  LVAL_e type = LVAL_DECIMAL;
  Lval_t expected = {
    .type = type,
    .num.f_num = 69.
  };

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t res = eval_ast(r.output);
    assert_equal(res, expected, type, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_all_operators(mpc_parser_t* language) {

  char* test_name = "all operators";
  char* test_statement = "+ 1 (* 1 2) (/ 4 2) (% 15 2) (- 0 6) (^ 3 2)";
  LVAL_e type = LVAL_INTEGER;
  Lval_t expected = {
    .type = type,
    .num.li_num = 9
  };

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t res = eval_ast(r.output);
    assert_equal(res, expected, type, test_name);
    mpc_ast_delete(r.output);
  }
}

static void test_min_max(mpc_parser_t* language) {

  char* test_name = "min max";
  char* test_statement = "min 69 (max 666 420)";
  LVAL_e type = LVAL_INTEGER;
  Lval_t expected = {
    .type = type,
    .num.li_num = 69
  };

  mpc_result_t r;
  if (mpc_parse("test", test_statement, language, &r)) {
    Lval_t res = eval_ast(r.output);
    assert_equal(res, expected, type, test_name);
    mpc_ast_delete(r.output);
  }
}


int main(int argc, char** argv) {

    mpc_parser_t* language = create_lang();

    test_integer_addition(language);
    test_decimal_addition(language);
    test_heterogenous_addition(language);
    test_all_operators(language);
    test_min_max(language);

    cleanup();

}