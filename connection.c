/************************************************************************
 * Name: Rohan Sharma                                                   *
 * Login: rsharma1                                                      *
 * Student Number: 639271                                               *
 * COMP30023 Computer Systems Project 2 - server/client for connect4    *
 *                                                                      *
 * connection.c: This file contains the functional definitions which    *
 * can be used by the client and the server to initialize their sockets.*
 ***********************************************************************/

/*-------------------------- Libraries --------------------------------*/
#include "connection.h"
/*---------------------------------------------------------------------*/

/*----------------------- Functional Definitions ----------------------*/
/* A function which creates, initializes and binds a socket to an 
 * address. This function should be used by the server.
 * Input: Port number, Pointer to socket address.
 * Output: The file descriptor created socket.
 */
int initialize_server_socket(int port, struct sockaddr_in *server_addr) {
	/* Variable definitions. */
	int socket_fd;

	/* Create a TCP socket. */
	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Failed to create a socket.\n");
		exit(EXIT_FAILURE);
	}

	/* Create an address the server will listen on. */
	memset(server_addr, 0, sizeof(*server_addr));
	(*server_addr).sin_family = AF_INET;
	(*server_addr).sin_addr.s_addr = INADDR_ANY;
	(*server_addr).sin_port = htons(port);

	/* Bind the TCP socket to the address created above. */
	if(bind(socket_fd, (struct sockaddr *)server_addr, 
	   sizeof(*server_addr)) < 0) {
		fprintf(stderr, "Failed to bind the socket to an address.\n");
		exit(EXIT_FAILURE);
	}
	return socket_fd;
}

/* A function which creates, initializes a socket for the client.
 * Input: Hostname, port number.
 * Output: The file descriptor for the socket.
 */
int initialize_client_socket(char *host, int port) {
	/* Variable definitions. */
	int socket_fd;
	struct sockaddr_in server_addr;
	struct hostent *server;

	/* Translate host name into peer's IP address. */
	if((server = gethostbyname(host)) == NULL) {
		fprintf(stderr, "No such host.\n");
		exit(EXIT_FAILURE);
	}

	/* Create an address for the server. */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr,
		  (char*)&server_addr.sin_addr.s_addr,
		  server->h_length
		 );
	server_addr.sin_port = htons(port);

	/* Create a TCP socket. */
	if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "Failed to create a socket.\n");
		exit(EXIT_FAILURE);
	}

	/* Connect the socket to the host server */
	if(connect(socket_fd, (struct sockaddr *)&server_addr,
	   sizeof(server_addr)) < 0) {
		fprintf(stderr, "Failed to connect to host server.\n");
		exit(EXIT_FAILURE);
	}
	return socket_fd;
}
/*---------------------------------------------------------------------*/