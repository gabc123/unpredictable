IDIR =/Library/Frameworks/SDL2.framework/Header
FDIR =/Library/Frameworks
CC=gcc
ifeq ($(OS),Windows_NT)
	CFLAGS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lglew32 -lglew32s
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CFLAGS=-lSDL2 -lGLEW -lGL
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS=-I$(IDIR) -F$(FDIR) -framework SDL2 -framework opengl
	endif
endif

objects = main.o up_shader_module.o up_vertex.o up_filereader.o

gltestapp : $(objects)
	gcc -o gltest.out $(CFLAGS) $(objects)