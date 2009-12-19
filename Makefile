all: main

clean:
	rm -f main

main:
	g++ main.cpp -o main -I. -ggdb
