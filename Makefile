all:
	$(CC) ussh.c -o ussh -Wall -Wextra -pedantic

clean:
	rm -f *.o
install:
	cp ussh /usr/local/bin
