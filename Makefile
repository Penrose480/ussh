all:
	$(CC) ussh.c -o ussh -Wall -Wextra -pedantic

clean:
	rm -f *.o
install: all
	rm -f /usr/local/bin/ussh 
	cp ussh /usr/local/bin
