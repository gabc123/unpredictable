IDIR =/Library/Frameworks/SDL2.framework/Header
FDIR =/Library/Frameworks
CC=gcc
CFLAGS=-I$(IDIR) -F$(FDIR)

objects = main.o

gltestapp : $(objects)
	gcc -o glTestapp $(CFLAGS) -framework SDL2 -framework opengl $(objects)