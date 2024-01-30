/******************************************************************************
* myServer.c
* 
* Writen by Prof. Smith, updated Jan 2023
* Use at your own risk.  
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdint.h>

#include "networks.h"
#include "safeUtil.h"
#include "send_and_receive.h"
#include "pollLib.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

void recvFromClient(int clientSocket);
int checkArgs(int argc, char *argv[]);

int add_new_socket(int mainServerSocket);
void process_client(int clientSocket);
void server_control(int mainServerSocket);

int main(int argc, char *argv[])
{
	int mainServerSocket = 0;   // Socket descriptor for the server socket
	int portNumber = 0;			// The server port number
	
	portNumber = checkArgs(argc, argv);
	
	// Create the server socket
	mainServerSocket = tcpServerSetup(portNumber);
	
	// Setup poll
	setupPollSet();

	// Add main server to poll set
	addToPollSet(mainServerSocket);

	// Handle multiple clients and have the server process them
	while (1)
	{
		server_control(mainServerSocket);
	}
	
	close(mainServerSocket);

	
	return 0;
}

int add_new_socket(int mainServerSocket)
{
	// Wait for client to connect
	int client_socket = tcpAccept(mainServerSocket, DEBUG_FLAG);

	// Add client to poll set
	addToPollSet(client_socket);

	return client_socket;
}

//needed this function real bad
void process_client(int client_socket)
{
	recvFromClient(client_socket);
}

void server_control(int mainServerSocket)
{
	int socket_number = 0;

	//checking for new file descriptors
	//waits for socket to read/write
	while ((socket_number = pollCall(-1)) == -1) {} 

	if (socket_number == mainServerSocket)
	{
		add_new_socket(mainServerSocket);
	}
	else
	{
		process_client(socket_number); //dangerously efficient coding
	}
}

void recvFromClient(int clientSocket)
{
	uint8_t dataBuffer[MAXBUF];
	int messageLen = 0;
	
	// Receive data from client socket
	if ((messageLen = recvPDU(clientSocket, dataBuffer, MAXBUF)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	if (messageLen == 0)
	{
		printf("Client closed connection on socket %d. Closing socket.\n", clientSocket);
		close(clientSocket);
		removeFromPollSet(clientSocket);
	}
	else
	{
		printf("Message received on socket %d, length: %d Data: %s\n", clientSocket, messageLen, dataBuffer);
	}
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	
	return portNumber;
}

