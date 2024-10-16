#pragma once
#include <assert.h>

#include "mpc.h"

#define NUM_PARSERS  6

mpc_parser_t* create_lang();
void cleanup();