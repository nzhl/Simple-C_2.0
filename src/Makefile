CFLAGS=-Wall -std=gnu11 -g -I. -O0

ncc: ncc.o
	cc -o $@ ncc.o


run: ncc
	./ncc





cleanAll: clean
	rm -f ncc
clean: 
	rm -f ncc.o
