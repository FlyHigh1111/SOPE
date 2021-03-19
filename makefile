EXEC = xmod
objects = xmod.o 
all: $(EXEC) 
xmod: xmod.h xmod.c 
	cc -Wall -o xmod xmod.c
.PHONY: clean
clean:
	rm $(objects)