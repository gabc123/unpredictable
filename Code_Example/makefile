IDIR =/Library/Frameworks/SDL2.framework/Header
FDIR =/Library/Frameworks
CC=gcc
CFLAGS=-I$(IDIR) -F$(FDIR)

objects = main.o

glTestapp : $(objects)
	gcc -o glTestapp $(CFLAGS) -framework SDL2 -framework opengl $(objects)