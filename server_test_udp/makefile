FDIR =/Library/Frameworks
CC=gcc
ifeq ($(OS),Windows_NT)
	CFLAGS=-lpthread
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CFLAGS= -lpthread
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS=-lpthread
	endif
endif

objects = main.o sha256.o up_assets.o up_filereader.o up_game_communication.o up_hash.o up_healthbar.o up_modelRepresentation.o up_network_packet_utilities.o up_objectReader.o up_server.o up_server_account.o up_server_game_simulation.o up_server_gameplay.o up_ship.o up_star_system.o up_thread_utilities.o up_utilities.o up_vertex.o

game_server : $(objects)
	gcc -o game_server.out $(objects) $(CFLAGS)
