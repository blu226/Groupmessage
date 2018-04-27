/*
 * Description: This is a skeleton for server part.
 * Course Name: CS371 Project 2
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>

const int PORT = 1501;

struct arguments {

	int sock;
	char name[20];
};

void *recieve_message(void *arg){


	/* get argument */
	struct arguments *args_struct = arg;
	int socket = (*args_struct).sock;
	char* name = (*args_struct).name;


	/* continue looping to recieve messages */
	while(1){
		char buffer[1024] = {0};
		int valread;
		/* passive listen for message from server*/
		valread = read(socket, buffer, 1024);
		//print message received
		printf("\n%s", buffer);
		printf("%s: ", name);
		fflush(stdout);
	}
}

int main(int argc, char *argv[]) {

	struct sockaddr_in address;
	struct sockaddr_in serv_addr;
	int sock = 0, valread, status;
	char name[20];
	pthread_t td;


    /* Socket settings */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		perror("Socket Failure.");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	if( inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0){
		perror("Invalid Address");
		exit(EXIT_FAILURE);
	}
    /* Request for connecting to the server */
	status = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (status < 0){
		perror("Connection Failed.");
		exit(EXIT_FAILURE);
	}
    /* Send and receive messages */
	struct arguments args;

	printf("Connection Established.\n");
	printf("Enter Name: ");
        scanf("%s", name);

	strcpy(args.name,name);
	args.sock = sock;
	//start a thread to recieve messages from other clients
	pthread_create(&td, NULL, &recieve_message, (void *)&args);

	printf("Enter q to close connection\n\n\n");

	int it = 0;
	while(1){
		char *input;
		size_t size = 1004;
		char *message;
		//get input from user
		message = (char *)malloc(1024);
		input = (char *)malloc(1004);
		getline(&input, &size, stdin);
		//if q is entered then exit
		if (input[0] == 'q'){
			send(sock, input, strlen(input), 0);
			printf("Exitting...\n");
			return 0;
		}
		else if (it > 0){
		//concatenate message to name and send
			strcpy(message, name);
			strcat(message, ": ");
			strcat(message, input);
			send(sock, message, strlen(message), 0);
		}
		it++;
		printf("%s: ", args.name);
		free(message);
		free(input);
	}
}
