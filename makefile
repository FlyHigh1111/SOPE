objects = xmod
all: $(objects) 
xmod: xmod.c xmod.h
	cc -Wall -o xmod xmod.c
.PHONY: clean
clean:
	rm $(objects)