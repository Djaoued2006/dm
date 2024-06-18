CFLAGS=-Wall -Wextra -g -pedantic
main: *.c
	$(CC) $(CFLAGS) *.c -o main

clean:
	rm -f main