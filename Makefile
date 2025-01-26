# -----------------------------------------------------------------
# SOURCE: https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html#creating
# -----------------------------------------------------------------

# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

CC = gcc

STD_LIB_PATH := $(CURDIR)/stdlib/std.pickle

CFLAGS = -Wall -Wextra -g -DSTD_LIB_PATH=\"$(STD_LIB_PATH)\" #-DEXIT_ON_FAIL=0
# CFLAGS += -O3  # do we really want to optimize ?

LFLAGS = -ledit -lm -ldl -lffi

INCLUDES = -I ./mpc -I ./libffi-3.4.6/include/
SRCS = ./mpc/mpc.c ./src/core.c ./src/lang.c ./src/ctypes.c

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.c=.o)

MAIN = PickleLisp
TEST = test
ADD_LIB = tests/libadd.so


.PHONY: clean

all: $(MAIN) $(TEST)


$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) ./src/pickle_lisp.c -o $(MAIN) $(OBJS) $(LFLAGS)

$(TEST): $(ADD_LIB)
	$(CC) $(CFLAGS) $(INCLUDES) ./tests/test.c -o $(TEST) $(OBJS) $(LFLAGS) -L./tests/ -l add

$(ADD_LIB):
	$(CC) $(CFLAGS) ./tests/add.c -c -fPIC -o ./tests/add.o
	$(CC) $(CFLAGS) ./tests/add.o -shared -o ./$(ADD_LIB)


.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) ./src/*.o ./tests/*.o *~ $(MAIN) $(TEST) $(ADD_LIB)
