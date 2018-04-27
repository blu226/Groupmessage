/**  Description: This is a skeleton for server part.*
Course Name: CS371 Project 2*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include<sys/ipc.h>

#define PORT 1501

const int MAX_CONNECTIONS = 5;
int sockets[5] = {-1, -1, -1, -1, -1};

struct arguments {

	int sock;

};



/* This function is to handle all communication with the client */
void *handle_client(void *arg){

	//get socket descriptor of connection
	struct arguments *args_struct = arg;
	int id = (*args_struct).sock;
	//continuously listen to client for messages
	while(1){
		char buffer[1024] = {0};
		char *quit = "q";
		//passively wait for message from client
		int valread = read(id, buffer, 1024);
		//if q then close the socket
		if (buffer[0] == 'q'){
			printf("Closing Socket: %d\n",id);
			send(id, quit, strlen(quit),0);
			close(id);

			for (int i = 0; i < MAX_CONNECTIONS; i++){
				if(sockets[i] == id){
					sockets[i] = -1;
				}
			}
			return 0;
		}
		//forward message from client to all other clients
		for (int i = 0; i < MAX_CONNECTIONS; i++){
			if (sockets[i] != id && sockets[i] != -1){
				send(sockets[i], buffer, strlen(buffer), 0);
			}
		}
		//print message for server
		printf("%s\n", buffer);

	}

}

int main(int argc, char *argv[]){

	int server_fd, new_socket, valread, status;
	struct sockaddr_in address;
	int num_clients = 0;
	int addrLen = sizeof(address);
	pthread_t td;

/* Socket settings */
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == 0){
		perror("Socket Failure.");
		exit(EXIT_FAILURE);
	}

/* Bind */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	status = bind(server_fd, (struct sockaddr *)&address, sizeof(address));
	if ( status < 0){
		perror("Bind Failure.");
		exit(EXIT_FAILURE);
	}

/* Listen */
	status = listen(server_fd, MAX_CONNECTIONS);
	if (status < 0){
		perror("Listen Failure.");
		exit(EXIT_FAILURE);
	}


/* Accept Clients */
	while (1){

		struct arguments args;

		new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrLen);
		if (new_socket < 0){
			perror("Accept Failure.");
			exit(EXIT_FAILURE);
		}
		//print the socket connected to for server
		printf("Socket Connected: %d\n", new_socket);
		//add socket to array
		for(int i = 0; i < MAX_CONNECTIONS; i++){
			if (sockets[i] == -1){
				sockets[i] = new_socket;
				break;
			}

		}

		args.sock = new_socket;
	/* Create a thread */
		pthread_create(&td, NULL, &handle_client, (void*)&args);
	}
}
