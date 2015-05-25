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

objects = main.o up_network_packet_utilities.o up_server.o up_thread_utilities.o up_vertex.o up_server_gameplay.o up_server_account.o

game_server : $(objects)
	gcc -o game_server.out $(objects) $(CFLAGS)
