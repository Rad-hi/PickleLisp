# -----------------------------------------------------------------
# SOURCE: https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html#creating
# -----------------------------------------------------------------

# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

CC = cc

CFLAGS = -Wall -g
LFLAGS = -ledit -lm

INCLUDES = -I../mpc
SRCS = parser.c ../mpc/mpc.c err.c

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
MAIN = parser



# -----------------------------------------------------------------
# GENERIC PART (You can leave it as it is)
# -----------------------------------------------------------------
#
.PHONY: clean

all:    $(MAIN)
	@echo  Compiled \--\> \./$(MAIN)

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
#
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)
