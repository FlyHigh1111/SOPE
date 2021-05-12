all: server lib client

#s: server.o lib.o delay.c
#	gcc -Wall -DDELAY=100 -o s delay.c lib.o server.o -pthread

client: client.c
	gcc -Wall -o cclient client.c -pthread

server: server.c
	gcc -Wall -c -o server server.c

lib: lib.c
	gcc -Wall -c -o lib lib.c

clean:
	rm -f s c
