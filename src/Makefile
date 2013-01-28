# Makefile para SimPack
#

PROG = identification
SRC = identification.cpp window.cpp
OBJ = $(patsubst %.cpp,%.o,$(SRC))

LIB = ./lib/simpack.a
INCLUDE = ../event/include/

CPP = g++
CPPFLAGS = -lm -I$(INCLUDE)

all: $(PROG)

$(PROG):
	$(CPP) $(CPPFLAGS) -o $(PROG) $(SRC) $(LIB)

clean: 
	rm -f *.o $(PROG)

