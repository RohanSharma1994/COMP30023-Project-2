/*****************************************************************************
 * Name: Rohan Sharma                                                        *
 * Login: rsharma1                                                           *
 * Student Number: 639271                                                    *
 * COMP30023 Computer Systems Project 2 - server/client for connect4         *
 *                                                                           *
 * server.h: This is the server of the connect4 game. Many clients can       *
 * connect to it. It uses threads to handle them concurrently.               *
 * Additionally, client interactions are stored in log.txt                   *
 ****************************************************************************/

/*-------------------------------- Libraries -------------------------------*/
#include "server.h"
/*--------------------------------------------------------------------------*/

/*-----------------------------    Main   ----------------------------------*/
int main(int argc, char *argv[]) {
	/* Ensure the port number was provided. */
	if(argc < N_ARGS) {
		fprintf(stderr, "Usage %s port_number.\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	/* Initialize mutex lock */
	if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr, "Mutex init failed.\n");
        exit(EXIT_FAILURE);
    }	
	/* Client & server address */
	struct sockaddr_in client_addr, server_addr;

	/* Create a new log file each time server restarts */
	if((fp=fopen(OUT_FILE, "w")) == NULL) {
		printf("Failed to create log file.\n");
		exit(EXIT_FAILURE);
	}
	fclose(fp);

	/* Get a socket to listen on */
	int socket_fd = initialize_server_socket(atoi(argv[1]), &server_addr);
	int client_fd = 0;
  	socklen_t client_len;

	/* Listen on this socket */
	if(listen(socket_fd, MAX_CLIENTS) < 0) {
		fprintf(stderr, "Failed to listen.\n");
		exit(EXIT_FAILURE);
	}
	printf("Listening on port %d....\n", atoi(argv[1]));

	/* Accept requests from clients */
	while(TRUE) {
		/* Get a new fd to communicate on */
		client_fd = accept(socket_fd, (struct sockaddr *)&client_addr,
			            &client_len);
		if(client_fd < 0) {
			perror("Error accepting client.\n");
			exit(EXIT_FAILURE);
		}
		/* The server will communicate to the client on a new thread */
		/* Create a structure to pass in client information to the new
		 * thread.
		 */
		client_info_t *info;
		info = (client_info_t *)malloc(sizeof(*info));
		assert(info != NULL);
		info->client_fd = client_fd;
		info->client_addr = client_addr;
		info->server_addr = server_addr;
		/* Log this connection. */
		connection_log(*info);

		/* Create this new thread*/
		pthread_t thread_id;
		if(pthread_create(&thread_id, NULL, play_game, (void*)info)) {
			fprintf(stderr, "Failed to create thread.\n");
			exit(EXIT_FAILURE);
		}
		/* Detach this thread */
		if(pthread_detach(thread_id)) {
			fprintf(stderr, "Failed to detach thead.\n");
			exit(EXIT_FAILURE);
		}
	}
	return 0;
}
/*--------------------------------------------------------------------------*/

/*---------------------------- Functional definitions ----------------------*/
/* The work function for the thread.
 * Input: A pointer to the file descriptor used for communication.
 * Output: None, plays game with the client and logs the activity.
 */
void *play_game(void *params) {
	client_info_t *client_info = (client_info_t *)params;
	/* Get the file descriptor to communicate to client */
	int fd = client_info->client_fd;
	/* Initialize data structures to play connect4 game with client. */
	int move, n;
	c4_t board;
	srand(RSEED);
	init_empty(board);

	/* Run the connect4 game until someone wins. */
	while(TRUE) {
		/* Read the move the client made. */
		if((n=read(fd,&move, sizeof(move))) < 1) {
			break;
		}

		move = ntohl(move);
		/* Make the move on the server's board. */
		if(do_move(board, move, YELLOW) != 1) {
			end_game_log(*client_info, STATUS_ABNORMAL);
			break;
		}
		/* Log this move*/
		move_log(*client_info, CLIENT_MOVE, move);
		/* Check if the client won. */
		if(winner_found(board) == YELLOW) {
			/* Client won log */
			end_game_log(*client_info, STATUS_USER_WON);
			break;
		}
		/* Check if it is a draw. */
		if(!move_possible(board)) {
			/* Draw log */
			end_game_log(*client_info, STATUS_DRAW);
			break;
		}

		/* The server will now think of a move */
		move = suggest_move(board, RED);
		int converted_move = htonl(move);

		/* Send the move to the client. */
		if((n=write(fd,&converted_move, sizeof(converted_move))) < 1) {
			break;
		}
		/* Log the move */
		move_log(*client_info, SERVER_MOVE, move);
		/* Make the move on the server's board. */
		if(do_move(board, move, RED) != 1) {
			/* Abnormal log */
			end_game_log(*client_info, STATUS_ABNORMAL);
			break;
		}

		/* Check if the server won. */
		if(winner_found(board) == RED) {
			/* Server won log */
			end_game_log(*client_info, STATUS_AI_WON);
			break;
		}

		/* Check if the client won. */
		if(winner_found(board) == YELLOW) {
			/* Client won log */
			end_game_log(*client_info, STATUS_USER_WON);
			break;
		}
		/* Check if it is a draw. */
		if(!move_possible(board)) {
			/* Draw log */
			end_game_log(*client_info, STATUS_DRAW);
			break;
		}
	}
	/* Clean up */
	close(client_info->client_fd);
	free(client_info);
    return NULL;
}

/* Writes out a connection log to the file. 
 * Input: Client info struct.
 * Output: None.
 */
void connection_log(client_info_t info) {
	/* Critical section coming up!! */
	pthread_mutex_lock(&lock);

	/* Open the file to write to */
	fp = fopen(OUT_FILE, "a");
	/* Get the current time */
	char buff[TIME_BUFFER];
    time_t now = time (0);
    strftime (buff, TIME_BUFFER, "%d %m %Y %H:%M:%S", localtime (&now));
    /* Get client's IP address */
    char ip4[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&(info.client_addr.sin_addr), ip4, INET_ADDRSTRLEN);
    /* Print to the file */
    fprintf(fp, "[%s](%s)", buff, ip4);
    fprintf(fp, "(soc_id %d) client connected\n", info.client_fd);
	/* All done */
	fclose(fp);

	pthread_mutex_unlock(&lock);
}

/* Writes out move logs to the output file.
 * Input: Client info struct, (SERVER_MOVE|CLIENT_MOVE), move,
 * Output: None.
 */
void move_log(client_info_t info, int turn, int move) {
	/* Critical section coming up!! */
	pthread_mutex_lock(&lock);

	/* Open the file to write to. */
	fp = fopen(OUT_FILE, "a");
	/* Get the current time. */
	char buff[TIME_BUFFER];
    time_t now = time (0);
    strftime (buff, TIME_BUFFER, "%d %m %Y %H:%M:%S", localtime (&now));
    fprintf(fp, "[%s]", buff);
    char ip4[INET_ADDRSTRLEN];
    /* Check who is making the move. */
    if(turn == SERVER_MOVE) {
    	/* Get server's IP address. */
    	inet_ntop(AF_INET,&(info.server_addr.sin_addr), ip4, INET_ADDRSTRLEN);
    	/* Print to the file. */
    	fprintf(fp, "(%s) server's move = %d\n", ip4, move);
    }
    else {
    	/* Get client's IP address. */
    	inet_ntop(AF_INET,&(info.client_addr.sin_addr), ip4, INET_ADDRSTRLEN);
    	/* Print to the file. */
    	fprintf(fp, "(%s)(soc_id %d) ", ip4, info.client_fd);
    	fprintf(fp, "client's move = %d\n", move);
    }
	/* All done */
	fclose(fp);

	pthread_mutex_unlock(&lock);
}

/* Writes out the logs when the game ends.
 * Input: Client info struct, STATUS (Defined in server.h).
 * Output: None.
 */
void end_game_log(client_info_t info, int status) {
	/* Critical section coming up!! */
	pthread_mutex_lock(&lock);

	/* Open the file to write to. */
	fp = fopen(OUT_FILE, "a");
	/* Get the current time. */
	char buff[TIME_BUFFER];
    time_t now = time (0);
    strftime (buff, TIME_BUFFER, "%d %m %Y %H:%M:%S", localtime (&now));
    fprintf(fp, "[%s]", buff);
    char ip4[INET_ADDRSTRLEN];
    /* Get client's IP address. */
    inet_ntop(AF_INET,&(info.client_addr.sin_addr), ip4, INET_ADDRSTRLEN);
    /* Print to the file. */
    fprintf(fp, "(%s)(soc_id %d) ", ip4, info.client_fd);
    fprintf(fp, "game over, code = %d\n", status);
  	/* All done */
	fclose(fp);

	pthread_mutex_unlock(&lock);
}
/*--------------------------------------------------------------------------*/