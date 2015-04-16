IDIR_SDL2 =/Library/Frameworks/SDL2.framework/Headers
IDIR_IMAGE =/Library/Frameworks/SDL2_image.framework/Headers
FDIR =/Library/Frameworks
CC=gcc
ifeq ($(OS),Windows_NT)
	CFLAGS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lglew32 -lglew32s
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CFLAGS= -lSDL2 -lGLEW -lGL -lSDL_image -lm
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS=-I$(IDIR_SDL2) -I$(IDIR_IMAGE) -F$(FDIR) -framework SDL2 -framework SDL2_image -framework opengl
	endif
endif

objects = main.o up_shader_module.o up_vertex.o up_filereader.o up_texture_module.o up_matrixTransforms.o

gltestapp : $(objects)
	gcc -o gltest.out $(CFLAGS) $(objects)
