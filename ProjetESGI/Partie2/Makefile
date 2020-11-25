# MAKEFILE
CC = gcc
CFLAGS = -Wall -Werror
DEBUG_FLAGS = valgrind
EXEC = XML-DTD-validator-P2.exe
SRC =  $(wildcard definitions/*.c headers/*.h)
OBJ = $(SRC:.c=.o)

All : $(EXEC)

$(EXEC) : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean :
	rm -rf *.o && rm -rf definitions/*.o

start :
	./$(EXEC)

debug :
	$(DEBUG_FLAGS) ./$(EXEC)

cleanAll : clean
	rm -rf $(EXEC)