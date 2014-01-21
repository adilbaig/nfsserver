nfsserver:
	gcc -g -Wall -std=c99 -o bin/nfsserver src/*.c -lpthread

serve: nfsserver
	./bin/nfsserver 9001

debug-serve: nfsserver
	gdb bin/nfsserver

clean:
	rm bin/*

