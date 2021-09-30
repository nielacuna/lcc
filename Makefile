
OBJS=alloc.o

lcc: $(OBJS)
	gcc -ggdb -o $@ $^

%.o:%.c
	gcc -ggdb -c -o $@ $<

.PHONY: clean
clean:
	rm -rf *.o lcc
