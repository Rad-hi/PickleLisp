#pragma once
#include <assert.h>
#include <stdbool.h>

#include "mpc.h"

mpc_parser_t* pickle_lisp;

mpc_parser_t* create_lang();
void cleanup();