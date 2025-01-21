# -----------------------------------------------------------------
# SOURCE: https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html#creating
# -----------------------------------------------------------------

# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

CC = cc

STD_LIB_PATH := $(CURDIR)
STDLIB := /stdlib/std.pickle
STD_LIB_PATH := $(STD_LIB_PATH)$(STDLIB)

CFLAGS = -Wall -g -DSTD_LIB_PATH=\"$(STD_LIB_PATH)\"
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

# define the executable file 
#
MAIN = PickleLisp

TEST = test

# -----------------------------------------------------------------
# GENERIC PART (You can leave it as it is)
# -----------------------------------------------------------------
#
.PHONY: clean

all:    $(MAIN) $(TEST) 
	@echo  Compiled \--\> \./$(MAIN)
	@echo  Compiled \--\> \./$(TEST)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) ./src/pickle_lisp.c -o $(MAIN) $(OBJS) $(LFLAGS)

$(TEST): ./tests/test.o
	$(CC) $(CFLAGS) $(INCLUDES) ./tests/test.c -o $(TEST) $(OBJS) $(LFLAGS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
#
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) ./src/*.o ./tests/*.o *~ $(MAIN)
