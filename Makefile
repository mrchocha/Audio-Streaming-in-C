
CC = gcc

.PHONY: all server client clean

all: server client
	make install
	make server
	make client
	
install:
	sudo apt-get install libpthread-stubs0-dev libvlc-dev vlc 

server:
	$(CC) server.c list.c -o server -lpthread

client:
	$(CC) -o client client.c -lpthread -l vlc

clean:
	rm server client
