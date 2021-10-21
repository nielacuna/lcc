
OBJS=alloc.o string.o main.o

CFLAGS=-Wall -Werror -I.

lcc: $(OBJS)
	gcc -ggdb $(CFLAGS) -o $@ $^

%.o:%.c
	gcc -ggdb $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf *.o lcc
