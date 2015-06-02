/************************************************************************
 * Name: Rohan Sharma                                                   *
 * Login: rsharma1                                                      *
 * Student Number: 639271                                               *
 * COMP30023 Computer Systems Project 2 - server/client for connect4    *
 *                                                                      *
 * server.h: A header file which defines the libraries, data structures *
 * and functional prototypes used by server.c                           *
 ***********************************************************************/

/*-------------------------- Libraries --------------------------------*/
#ifndef SERVER_H
#define SERVER_H
#include "connection.h"
#include "connect4.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>
/*---------------------------------------------------------------------*/


/*------------------------- Data structures ---------------------------*/
pthread_mutex_t lock;
FILE *fp;
/* Client information (Passed into thread) */
typedef struct {
	int client_fd;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
} client_info_t;
/*---------------------------------------------------------------------*/

/*---------------------------- Constants ------------------------------*/
#define N_ARGS 2
#define TRUE 1
#define OUT_FILE "log.txt"
#define STATUS_ABNORMAL   1
#define STATUS_USER_WON   5
#define STATUS_DRAW       6
#define STATUS_AI_WON     7
#define TIME_BUFFER 100
#define SERVER_MOVE 1
#define CLIENT_MOVE 2
/*---------------------------------------------------------------------*/

/*------------------------- Functional prototypes ---------------------*/
/* The work function for the thread.
 * Input: A pointer to the file descriptor used for communication.
 * Output: None, plays game with the client and logs the activity.
 */
void *play_game(void *);

/* Writes out a connection log to the file. 
 * Input: Client info struct.
 * Output: None.
 */
void connection_log(client_info_t);

/* Writes out move logs to the output file.
 * Input: Client info struct, (SERVER_MOVE|CLIENT_MOVE), move,
 * Output: None.
 */
void move_log(client_info_t, int, int);

/* Writes out the logs when the game ends.
 * Input: Client info struct, STATUS (Defined in server.h).
 * Output: None.
 */
void end_game_log(client_info_t, int);
/*---------------------------------------------------------------------*/
#endif