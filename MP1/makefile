objects = xmod.o 
all: $(objects) 
xmod.o: xmod.h xmod.c 
	cc -Wall -o xmod.o xmod.c
.PHONY: clean
clean:
	rm $(objects)