/************************************************************************
 * Name: Rohan Sharma                                                   *
 * Login: rsharma1                                                      *
 * Student Number: 639271                                               *
 * COMP30023 Computer Systems Project 2 - server/client for connect4    *
 *                                                                      *
 * client.c: The file which contains the client side code of connect4.  *
 ***********************************************************************/

/*-------------------------- Libraries --------------------------------*/
#include "client.h"
/*---------------------------------------------------------------------*/

/*-------------------------        Main           ---------------------*/
 int main(int argc, char *argv[]) {
 	/* Ensure enough arguments are passed */
 	if(argc < 3) {
 		fprintf(stderr, "Usage: %s host port.\n", argv[0]);
 		exit(EXIT_FAILURE);
 	}

 	/* Initialize a socket to communicate to the server */
	int socket_fd = initialize_client_socket(argv[1], atoi(argv[2]));

	/* Begin the connect4 game against the server. */
	/* The data structures required for this game */
	c4_t board;
	int move, n;
	init_empty(board);
	print_config(board);

    /* This loop does 2 moves each iteration. One for the
	 * human player and one for the server. 
	 */
	while ((move = get_move(board)) != EOF) {
	    /* process the person's move */
		if (do_move(board, move, YELLOW)!=1) {
			printf("Panic\n");
			break;
		}
		/* Send the move to the server */
		int converted_move = htonl(move);
		n = write(socket_fd, &converted_move, sizeof(converted_move));

		if(n < 0) {
			perror("Failed to write to socket.\n");
			break;
		}
		print_config(board);
		/* and did they win??? */
		if (winner_found(board) == YELLOW) {
			/* rats, the person beat us! */
			printf("Ok, you beat me, beginner's luck!\n");
			break;
		}
		/* was that the last possible move? */
		if (!move_possible(board)) {
			/* yes, looks like it was */
			printf("An honourable draw\n");
			break;
		}

		/* Ask the server for its move  */
		n = read(socket_fd, &move, sizeof(move));
        move = ntohl(move);
		if(n < 0) {
			perror("Failed to read from socket.\n");
			break;
		}

		/* pretend to be thinking hard */
		printf("Ok, let's see now....");
		sleep(1);
		/* then play the move */
		printf(" I play in column %d\n", move);
		if (do_move(board, move, RED)!=1) {
			printf("Panic\n");
			break;
		}
		print_config(board);
		/* and did we win??? */
		if (winner_found(board) == RED) {
			/* yes!!! */
			printf("I guess I have your measure!\n");
			break;
		}
		/* and did they win??? */
		if (winner_found(board) == YELLOW) {
			/* rats, the person beat us! */
			printf("Ok, you beat me, beginner's luck!\n");
			break;
		}
		/* Was that the last possible move? */
		if (!move_possible(board)) {
			/* yes, looks like it was */
			printf("An honourable draw\n");
			break;
		}
		/* otherwise, the game goes on */
	}
	printf("FINALMOVE=%d", move);
	printf("\n");
	close(socket_fd);
	return 0;
 }
/*---------------------------------------------------------------------*/