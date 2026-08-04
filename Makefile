all:
	$(CC) base.c -o base -Wall -Wextra -pedantic

clean:
	rm -f *.o
install:
	cp base /usr/local/bin
