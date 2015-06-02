# Makefile: Project 2 Computer Systems
# Author: Rohan Sharma, rsharma1
# S.N: 639271
# Create an object for connections since it doesn't have any dependencies
all: server client connection.o
connection.o:
	gcc -Wall -c connection.c
# Compile the server
server: connection.o
	gcc -Wall -o server server.c connection.o -pthread
# Compile the client
client: connection.o
	gcc -Wall -o client client.c connection.o