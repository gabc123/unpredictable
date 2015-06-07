IDIR_SDL2 =/Library/Frameworks/SDL2.framework/Headers
IDIR_IMAGE =/Library/Frameworks/SDL2_image.framework/Headers
IDIR_NET =/Library/Frameworks/SDL2_net.framework/Headers
IDIR_MIXER =/Library/Frameworks/SDL2_mixer.framework/Headers
FDIR =/Library/Frameworks
CC=gcc

objects = main.o sha256.o testmodels.o up_assets.o up_camera_module.o up_collisions.o up_control_events.o up_filereader.o up_font_module.o up_game_tools.o up_graphics_setup.o up_hash.o up_interface.o up_math.o up_math_transformations.o up_menu.o up_mesh.o up_music.o up_network_module.o up_network_packet_utilities.o up_objectReader.o up_object_actions.o up_object_handler.o up_player.o up_render_engine.o up_shader_module.o up_skyBox.o up_star_system.o up_texture_module.o up_thread_utilities.o up_updateObjectMatrix.o up_utilities.o

ifeq ($(OS),Windows_NT)
	CFLAGS=-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lglew32 -lglew32s
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CFLAGS= -lSDL2 -lGL -lSDL2_image -lm -lSDL2_net -lpthread -lSDL2_mixer
		objects += gl_core_3_3.o
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS=-I$(IDIR_SDL2) -I$(IDIR_IMAGE) -I$(IDIR_NET) -I$(IDIR_MIXER) -F$(FDIR) -framework SDL2 -framework SDL2_image -framework SDL2_net -framework SDL2_mixer -framework opengl
	endif
endif

gltestapp : $(objects)
	gcc -o gltest.out $(CFLAGS) $(objects)
