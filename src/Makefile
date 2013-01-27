# Makefile para SimPack
#

PROG = identification

INCLUDE = ../event/include
SRC = ../event/src
OBJ = $(SRC)/calendar.o $(SRC)/capifac.o $(SRC)/capifut.o $(SRC)/capiran.o \
      $(SRC)/estatus.o $(SRC)/event.o $(SRC)/facility.o $(SRC)/flist.o \
      $(SRC)/future.o $(SRC)/heap.o $(SRC)/leftist.o $(SRC)/linked.o $(SRC)/service.o \
      $(SRC)/token.o $(SRC)/util.o $(SRC)/vlist.o

CPP = g++
CPPFLAGS = -I$(INCLUDE) -Wno-deprecated

all : $(PROG)

$(PROG) : $(PROG).o $(OBJ) window.o
	$(CPP) $(CPPFLAGS) -o $(PROG) $(PROG).cpp window.cpp $(OBJ) -lm

clean : $(SRC)/*.o $(PROG).o
	rm $(SRC)/*.o $(PROG).o

