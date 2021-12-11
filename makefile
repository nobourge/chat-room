all: client server

client3: client.c common.h
	gcc -o client client.c

server3: server.c common.h
	gcc -o server server.c