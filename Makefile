# -----------------------------------------------------------------
# SOURCE: https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html#creating
# -----------------------------------------------------------------

# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

MIMALLOC_DIR := ./thirdparty/mimalloc
MIMALLOC_BUILD := $(MIMALLOC_DIR)/build
LIBFFI_DIR := ./thirdparty/libffi-3.4.6
MPC_DIR := ./thirdparty/mpc

CC = gcc

CFLAGS = -std=c99
CFLAGS += -Wall -Wextra -Wfloat-equal
CFLAGS += -Wno-unused-parameter -Wno-unused-function -Wno-sign-conversion
# CFLAGS +=  -Werror

# extension is defined by the config of the language
STD_LIB_PATH := $(CURDIR)/stdlib/std
CFLAGS += -DSTD_LIB_PATH=\"$(STD_LIB_PATH)\"

# CFLAGS += -ggdb
CFLAGS += -O

## flags for tests
CFLAGS += -DEXIT_ON_FAIL  # for tests to exit on fail
# CFLAGS += -DVERBOSE_ADD_  # for the add library to print its input


# Whether to use mimalloc allocator, or default one
# Ref: https://microsoft.github.io/mimalloc/index.html
# Ref: https://github.com/microsoft/mimalloc
# Comment this if you don't want the language to use mimalloc
#
CFLAGS += -DUSE_MIMALLOC

LFLAGS = -ledit -lm -ldl -lffi -L $(MIMALLOC_BUILD) -l:libmimalloc.a -lpthread

INCLUDES = -I $(MPC_DIR) -I $(LIBFFI_DIR)/include/ -I $(MIMALLOC_DIR)/include
SRCS = $(MPC_DIR)/mpc.c ./src/core.c ./src/lang.c ./src/ctypes.c

OBJS = $(SRCS:.c=.o)

MAIN = pickle
TEST = test_picklelisp

.PHONY: clean, all

all: $(MAIN) $(TEST)

mimalloc:
	mkdir -p $(MIMALLOC_BUILD)
	cmake -S $(MIMALLOC_DIR) -B $(MIMALLOC_BUILD)
	$(MAKE) -C $(MIMALLOC_BUILD) -j$(shell nproc)

$(MAIN): $(OBJS) mimalloc
	$(CC) $(CFLAGS) $(INCLUDES) ./src/pickle_lisp.c -o $(MAIN) $(OBJS) $(LFLAGS)

$(TEST): addlib $(OBJS) mimalloc
	$(CC) $(CFLAGS) $(INCLUDES) ./tests/test.c -o $(TEST) $(OBJS) $(LFLAGS) -L./tests/ -l add

addlib:
	$(CC) $(CFLAGS) ./tests/add.c -c -fPIC -o ./tests/add.o
	$(CC) $(CFLAGS) ./tests/add.o -shared -o ./tests/libadd.so

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) ./src/*.o ./tests/*.o *~ $(MAIN) $(TEST) addlib
