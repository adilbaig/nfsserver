nfsserver:
	gcc -g -Wall -o bin/nfsserver src/*.c

serve: nfsserver
	./bin/nfsserver 9001


clean:
	rm bin/*

