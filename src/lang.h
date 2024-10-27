#pragma once
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>

#include "config.h"
#include "core.h"
#include "mpc.h"

#ifdef _WIN32
    // TODO: test on windows and figure out what to include
#else
    #include <linux/limits.h>
#endif // _WIN32

void create_vm(Lenv_t** e, mpc_parser_t** lang);
void cleanup(void);
