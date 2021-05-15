all: s c

c: client.c
	gcc -Wall -o c client.c -pthread

s: server.c queue.c lib.c delay.c
	gcc -Wall -o s server.c queue.c lib.c delay.c -pthread

clean:
	rm -f s c
