all: client server

client3: client.c common.h
	gcc -pthread -lpthread -o client client.c -Wall -Wpedantic

server3: server.c common.h
	gcc -pthread -o server server.c -Wall -Wpedantic