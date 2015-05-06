IDIR_SDL2 =/Library/Frameworks/SDL2.framework/Headers
IDIR_IMAGE =/Library/Frameworks/SDL2_image.framework/Headers
IDIR_NET =/Library/Frameworks/SDL2_net.framework/Headers
FDIR =/Library/Frameworks
CC=gcc
ifeq ($(OS),Windows_NT)
	CFLAGS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lglew32 -lglew32s
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CFLAGS= -lSDL2 -lGL -lSDL2_image -lm -lSDL_net -lpthread
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS=-I$(IDIR_SDL2) -I$(IDIR_IMAGE) -I$(IDIR_NET) -F$(FDIR) -framework SDL2 -framework SDL2_image -framework SDL2_net -framework opengl
	endif
endif

objects = main.o up_updateObjectMatrix.o up_menu.o testmodels.o up_camera_module.o up_shader_module.o up_vertex.o up_filereader.o up_texture_module.o up_matrixTransforms.o up_utilities.o up_objectReader.o up_initGraphics.o up_ship.o up_network_module.o up_thread_utilities.o up_assets.o up_modelRepresentation.o up_star_system.o up_render_engine.o gl_core_3_3.o up_font_module.o

gltestapp : $(objects)
	gcc -o gltest.out $(CFLAGS) $(objects)
