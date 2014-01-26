nfsserver:
	gcc -g -Wall -std=c99 -o bin/nfsserver src/thread-server.c lib/*.c -lpthread

basic-server:
	gcc -g -Wall -std=c99 -o bin/basic-server src/basic-server.c lib/*.c

prefork-server:
	gcc -g -Wall -std=c99 -o bin/prefork-server src/prefork-server.c lib/*.c

serve: nfsserver
	./bin/nfsserver 9001

debug-serve: nfsserver
	gdb bin/nfsserver

clean:
	rm bin/*

