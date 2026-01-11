#pragma once
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>

#include "config.h"
#include "core.h"
#include "mpc.h"

#include <linux/limits.h>

void create_vm(Lenv_t** e, mpc_parser_t** lang);
void cleanup(void);
