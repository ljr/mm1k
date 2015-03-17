
PROG = identification
SRC = identification.cpp window.cpp config.cpp
HEADERS = window.h config.h
OBJ = $(patsubst %.cpp,%.o,$(SRC))

TEST = test/test_window
SRCT = test/test_window.cpp window.cpp 
OBJT = $(patsubst %.cpp,%.o,$(SRCT))

LIB = ./lib/simpack.a ./lib/cjson.a
INCLUDE = ../include

CPP = g++
CPPFLAGS = -lm -I$(INCLUDE) -Wno-deprecated 

all: $(PROG) $(TEST)

$(PROG): $(OBJ) $(HEADERS)
	$(CPP) $(CPPFLAGS) -o $(PROG) $(SRC) $(LIB)

$(TEST): $(OBJT) $(HEADERS)
	$(CPP) $(CPPFLAGS) -o $(TEST) $(SRCT) $(LIB)

clean: 
	rm -f *.o $(PROG) $(TEST)

