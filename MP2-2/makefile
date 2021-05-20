all: s c

c: client.c common.c
	gcc -Wall -o c client.c common.c -pthread

s: server.c queue.c lib.c delay.c common.c
	gcc -Wall -o s server.c queue.c lib.c delay.c common.c -pthread

clean:
	rm -f s c
