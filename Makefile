nfsserver:
	gcc -g -Wall -o bin/nfsserver src/*.c -lpthread

serve: nfsserver
	./bin/nfsserver 9001

debug-serve: nfsserver
	gdb bin/nfsserver

clean:
	rm bin/*

