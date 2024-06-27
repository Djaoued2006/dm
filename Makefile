main: ./src/main.c
	cc ./src/main.c -o main -g

clean:
	rm -f main