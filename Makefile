
CC = gcc

all: server client
	make clean
	make server
	make client

server:
	$(CC) server.c list.c -o server -lpthread

client:
	$(CC) -o client client.c -lpthread -l vlc

clean:
	rm server client