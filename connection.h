/************************************************************************
 * Name: Rohan Sharma                                                   *
 * Login: rsharma1                                                      *
 * Student Number: 639271                                               *
 * COMP30023 Computer Systems Project 2 - server/client for connect4    *
 *                                                                      *
 * connection.h: A header file which contains the libraries, data       *
 * structures and functional prototypes used by connection.c.           *
 ***********************************************************************/

/*-------------------------- Libraries --------------------------------*/
#ifndef CONNECTION_H
#define CONNECTION_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*---------------------------------------------------------------------*/

/*---------------------------- Constants ------------------------------*/
#define MAX_CLIENTS 1000
/*---------------------------------------------------------------------*/

/*------------------------ Functional Prototypes ----------------------*/
/* A function which creates, initializes and binds a socket to an 
 * address. This function should be used by the server.
 * Input: Port number, Pointer to socket address.
 * Output: The file descriptor created socket.
 */
int initialize_server_socket(int, struct sockaddr_in*);

/* A function which creates, initializes a socket for the client.
 * Input: Hostname, port number.
 * Output: The file descriptor for the socket.
 */
int initialize_client_socket(char *, int);
/*---------------------------------------------------------------------*/
#endif