

INCLUDE := ../include

SRC :=	cJSON.c
OBJ := $(patsubst %.c,%.o,$(SRC))

CC := gcc
CCFLAGS := -I$(INCLUDE) -lm 

all: 
	$(CC) $(CCFLAGS) -c $(SRC)
	ar rvs ../lib/cjson.a $(OBJ)

clean:
	rm -f *.o ../lib/*.a *~


