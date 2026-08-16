all:
	$(CC) ussh.c -o ussh -Wall -Wextra -pedantic

clean:
	rm -f *.o
install: all
	rm -f /bin/ussh 
	cp ussh /bin
