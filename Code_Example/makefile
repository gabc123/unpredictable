IDIR =/Library/Frameworks/SDL2.framework/Header
FDIR =/Library/Frameworks
CC=gcc
CFLAGS=-I$(IDIR) -F$(FDIR)

objects = main.o up_shader_module.o up_vertex.o up_filereader.o

gltestapp : $(objects)
	gcc -o glTestapp $(CFLAGS) -framework SDL2 -framework opengl $(objects)