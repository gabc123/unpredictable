IDIR_SDL2 =/Library/Frameworks/SDL2.framework/Headers
IDIR_net =/Library/Frameworks/SDL2_net.framework/Headers
IDIR_IMAGE =/Library/Frameworks/SDL2_image.framework/Headers
FDIR =/Library/Frameworks
CC=gcc
ifeq ($(OS),Windows_NT)
	CFLAGS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lglew32 -lglew32s
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CFLAGS= -lSDL2 -lGLEW -lGL -lSDL2_image -lm
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS=-I$(IDIR_SDL2) -I$(IDIR_IMAGE) -I$(IDIR_net) -F$(FDIR) -framework SDL2 -framework SDL2_image -framework opengl -framework SDL2_net
	endif
endif

objects = server.o serverfunk.o 

gltestapp : $(objects)
	gcc -o gltest.out $(CFLAGS) $(objects)