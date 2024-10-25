#pragma once
#include <assert.h>
#include <stdbool.h>

#include "config.h"
#include "mpc.h"

mpc_parser_t* create_lang();
void cleanup();