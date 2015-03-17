

INCLUDE := ../include

SRC :=	calendar.cpp capifac.cpp capifut.cpp capiran.cpp estatus.cpp event.cpp \
	facility.cpp flist.cpp future.cpp heap.cpp leftist.cpp linked.cpp      \
	service.cpp token.cpp util.cpp vlist.cpp
OBJ := $(patsubst %.cpp,%.o,$(SRC))

CPP := g++
CPPFLAGS := -I$(INCLUDE) -Wno-deprecated

all: 
	$(CPP) $(CPPFLAGS) -c $(SRC)
	ar rvs ../lib/simpack.a $(OBJ)

clean:
	rm -f *.o ../lib/*.a *~


