objects = xmod.o
all: $(objects) 
xmod.o: xmod.c xmod.h
	cc -Wall -o xmod.o xmod.c
.PHONY: clean
clean:
	rm $(objects)