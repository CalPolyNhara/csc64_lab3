/******************************************************************************
* myClient.c
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
#include <signal.h>

#include "networks.h"
#include "safeUtil.h"
#include "send_and_receive.h"
#include "pollLib.h"

#define MAXBUF 1024
#define DEBUG_FLAG 1

void sendToServer(int socketNum);
int readFromStdin(uint8_t * buffer);
void checkArgs(int argc, char * argv[]);

// void signalHandler(int signal);

//needs to be global
int socketNum = 0;         //socket descriptor

int main(int argc, char * argv[])
{	
	checkArgs(argc, argv);

	//set up the TCP Client socket
	socketNum = tcpClientSetup(argv[1], argv[2], DEBUG_FLAG);
	
	//for ^C
    // signal (SIGQUIT, signalHandler);
	setupPollSet();
	addToPollSet(socketNum);
	addToPollSet(STDIN_FILENO);
	// whie loop to always send to server
	int socket_num_check = 0;
	while (1)
	{
		socket_num_check = pollCall(-1);
		if(socket_num_check == 0)
		{
			sendToServer(socketNum);
		}		
		else if(socket_num_check == socketNum)
		{
			printf("server has terminated \n");
			close(socketNum);
        	exit(0);
		}
	}
	
	return 0;
}

void client_blocking()
{
	pollCall(-1);
}

void sendToServer(int socketNum)
{
	uint8_t sendBuf[MAXBUF];   // Data buffer
	int sendLen = 0;           // Amount of data to send
	int sent = 0;              // Actual amount of data sent 
	
	//printing data from the input
	sendLen = readFromStdin(sendBuf);
	printf("read: %s string len: %d (including null)\n", sendBuf, sendLen);
	
	//sending PDU to client
	sent =  sendPDU(socketNum, sendBuf, sendLen);
	if (sent <= 0) //error chekcing
	{
		perror("send call");
		exit(-1);
	}

	printf("Amount of data sent is: %d\n", sent);
}

int readFromStdin(uint8_t * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("Enter data: ");
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 3)
	{
		printf("usage: %s host-name port-number \n", argv[0]);
		exit(1);
	}
}

// void signalHandler(int client_signal)
// {
//     if (client_signal == SIGQUIT) //only tests ^c
//     {
//         // Shut down socket
//         close(socketNum);
//         exit(0);
//     }
// }