CC = gcc
CFLAGS = -std=gnu11 -Wall -c
LFLAGS = -std=gnu11 -Wall -o

OBJS =  list.o smalllib.o main.o

smallsh.o: $(OBJS)
	@echo '[BUILDING] ... smallsh'
	@$(CC) $(LFLAGS) smallsh $(OBJS)
	@echo '[DONE] ... Program output as: smallsh'

main.o: main.c
	@echo '[BUILDING] ... main'
	@$(CC) $(CFLAGS) main.c

smalllib.o: smalllib.c
	@echo '[BUILDING] ... smalllib'
	@$(CC) $(CFLAGS) smalllib.c

list.o: list.c
	@echo '[BUILDING] ... list'
	@$(CC) $(CFLAGS) list.c

clean:
	@echo '[CLEANING] ... removing .o files'
	@rm *.o

.PHONY: valgrind

valgrind:
	valgrind --tool=memcheck game