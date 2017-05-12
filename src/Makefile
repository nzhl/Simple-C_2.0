CFLAGS=-Wall -std=gnu11 -g -I. -O0

TESTSRCS=vector.c map.c

ncc: ncc.o
	cc -o $@ ncc.o

run: ncc
	./ncc

test: $(patsubst %.c, %.o, $(TESTSRCS))
	cd test && cc -c test.c $(patsubst %.c, test_%.c, $(TESTSRCS))
	$(foreach each, $(TESTSRCS), \
		echo "Current Test File :  $(each)"; \
		gcc -o do_test test/test.o $(patsubst %.c, test/test_%.o, $(each)) $(patsubst %.c, %.o, $(each)); \
		./do_test; \
	)


cleanAll: clean
	rm -f ncc
clean:
	rm -f test/*.o *.o  do_test

.PHONY : test cleanAll clean
