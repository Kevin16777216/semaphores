ifeq ($(DEBUG), yes)
	CC = gcc -g
else
	CC = gcc
endif

all: control.o write.o
	$(CC) -o control control.o
	$(CC) -o write write.o

write.o: write.c
	$(CC) -c write.c

control.o: control.c
	$(CC) -c control.c

runw:
	./write

runc:
	./control -c

clean:
	rm *.o
	rm *~

memtest:
	valgrind --leak-check=yes ./write
