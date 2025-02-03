# -----------------------------------------------------------------
# SOURCE: https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html#creating
# -----------------------------------------------------------------

# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

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


LFLAGS = -ledit -lm -ldl -lffi

INCLUDES = -I ./mpc -I ./libffi-3.4.6/include/
SRCS = ./mpc/mpc.c ./src/core.c ./src/lang.c ./src/ctypes.c

OBJS = $(SRCS:.c=.o)

MAIN = pickle
TEST = test
ADD_LIB = tests/libadd.so


.PHONY: clean, all

all: $(MAIN) $(TEST)


$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) ./src/pickle_lisp.c -o $(MAIN) $(OBJS) $(LFLAGS)

$(TEST): $(ADD_LIB) $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) ./tests/test.c -o $(TEST) $(OBJS) $(LFLAGS) -L./tests/ -l add

$(ADD_LIB):
	$(CC) $(CFLAGS) ./tests/add.c -c -fPIC -o ./tests/add.o
	$(CC) $(CFLAGS) ./tests/add.o -shared -o ./$(ADD_LIB)


.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) ./src/*.o ./tests/*.o *~ $(MAIN) $(TEST) $(ADD_LIB)
