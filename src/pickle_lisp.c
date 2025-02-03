#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "lang.h"
#include "core.h"
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
    #include <linux/limits.h>
#endif // _WIN32

#define PRINT_AST    0

int main(int argc, char** argv) {
    mpc_parser_t* language = NULL;
    Lenv_t* e = NULL;
    create_vm(&e, &language);

    if (argc >= 2) {
        for (int i = 1; i < argc; ++i) {
            Lval_t* args = lval_add(lval_create_sexpr(), lval_create_str(argv[i]));
            Lval_t* x = builtin_load(e, args);
            if (x->type == LVAL_ERR) lval_println(x);
            lval_del(x);
        }
    } else {

        puts(LANG_NAME" Version 666.69.420");
        puts("`exit` or Ctrl+C to Exit\n");

        mpc_result_t r;
        while(true) {
            char* buf = readline(REPL_IN);
            add_history(buf);

            if (mpc_parse("<stdin>", buf, language, &r)) {
#if PRINT_AST
                mpc_ast_print(r.output);
#endif
                Lval_t* res = lval_eval(e, lval_read(r.output));
                bool exit_REPL = res->type == LVAL_EXIT;
                lval_println(res);

                lval_del(res);
                mpc_ast_delete(r.output);

                if (exit_REPL) break;
            } else {
                mpc_err_print(r.error);
                mpc_err_delete(r.error);
            }
            free(buf);
        }
    }

    cleanup();

    return 0;
}
