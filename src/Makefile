# Makefile para SimPack
#

PROG = identification
SRC = identification.cpp window.cpp window.h
OBJ = $(patsubst %.cpp,%.o,$(SRC))

TEST = test/test
SRCT = test/test.cpp window.cpp window.h
OBJT = $(patsubst %.cpp,%.o,$(SRCT))

LIB = ./lib/simpack.a ./lib/cjson.a
INCLUDE = ../include

CPP = g++
CPPFLAGS = -lm -I$(INCLUDE)

all: $(PROG) $(TEST)

$(PROG): $(OBJ)
	$(CPP) $(CPPFLAGS) -o $(PROG) $(SRC) $(LIB)

$(TEST): $(OBJT)
	$(CPP) $(CPPFLAGS) -o $(TEST) $(SRCT) $(LIB)

clean: 
	rm -f *.o $(PROG) $(TEST)

