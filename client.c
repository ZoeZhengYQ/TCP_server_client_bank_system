/*///////////////////////////////////////////////////////////
*
* FILE:		client.c
* AUTHOR:	Yingqiao Zheng
* PROJECT:	CS 3251 Project 1 - Professor Ellen Zegura 
* DESCRIPTION:	Network Client Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/* Included libraries */

#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Constants */
#define RCVBUFSIZE 512		    /* The receive buffer size */
#define SNDBUFSIZE 512		    /* The send buffer size */
#define REPLYLEN 32

/* The main function */
int main(int argc, char *argv[])
{

    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* server address structure */
    char *accountName;		      /* Account Name  */
    char *servIP;		          /* Server IP address  */
    char *operationName;
    char *toAccount;
    unsigned short servPort;	    /* Server Port number */
    
    char sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    char rcvBuf[RCVBUFSIZE];	    /* Receive Buffer */
    int balance;            /* Account balance */
    int toBalance;
    int amount;

    servIP = argv[1];
    servPort = atoi(argv[2]);
	operationName = argv[3];

    memset(&sndBuf, 0, SNDBUFSIZE);
    memset(&rcvBuf, 0, RCVBUFSIZE);

    /* printf("\n======================================================================\n"
     *	"Correct format of command:\n"
     *	"1. serverIP serverPort BAL accountName\n"
     *	"2. serverIP serverPort WITHDRAW accountName amount\n"
     *	"3. serverIP serverPort TRANSFER from-accountName amount to-accountName\n"
     *	"======================================================================\n\n");
	 */

    /* Create a new TCP socket*/
    clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock < 0) {
        perror("error creating client socket");
        exit(1);
    }

    /* Construct the server address structure */
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(servPort);
    serv_addr.sin_addr.s_addr = inet_addr(servIP);

    /* Establish connection to the server */
    if (connect(clientSock, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) {
        perror("connection failed");
        exit(1);
    }

    /* send operation name to server */
    strcpy(sndBuf, operationName);
    if (send(clientSock, sndBuf, strlen(sndBuf), 0) < 0) {
        	perror("send operation failed");
        	exit(1);
    }
    memset(&sndBuf, 0, SNDBUFSIZE);

    recv(clientSock, rcvBuf, 3, 0);
    memset(&rcvBuf, 0, RCVBUFSIZE);

    /* 
     *  operation to request for account balance 
     */
	if (strcmp(operationName, "BAL") == 0) {
		if (argc != 5) {
			printf("Incorrect number of arguments."
				"The correct format is:\n\tserverIP serverPort BAL accountName\n");
			exit(1);
    	}
    	accountName = argv[4];
    
    /* Send accountName to the server */
    	strcpy(sndBuf, accountName);
    	send(clientSock, sndBuf, strlen(sndBuf), 0);
    	memset(&sndBuf, 0, SNDBUFSIZE);

    /* Receive and print response from the server */
    	recv(clientSock, &balance, sizeof(int), 0);
    	if (balance >= 0) { 
    		printf("\nBalance in account <%s> is: %d\n\n", accountName, balance);
		}
	}



	/* 
	 * operation to withdraw from account 
	 */
	else if (strcmp(operationName, "WITHDRAW") == 0) {
		if (argc != 6) {
			printf("\nIncorrect number of arguments."
				"The correct format is:\n\tserverIP serverPort WITHDRAW accountName amount\n\n");
			exit(1);
    	}
    	accountName = argv[4];
    	amount = atoi(argv[5]);

    	/* Send accountName to the server */
    	strcpy(sndBuf, accountName);
    	send(clientSock, sndBuf, strlen(sndBuf), 0);
    	memset(&sndBuf, 0, SNDBUFSIZE);
    	/* Ok */
    	recv(clientSock, rcvBuf, 3, 0);
    	memset(&rcvBuf, 0, RCVBUFSIZE);
    	/* Send amount */
    	send(clientSock, &amount, sizeof(int), 0);
    	recv(clientSock, &balance, sizeof(int), 0);
    	if (balance >= 0) { 
    		printf("\nWithdraw %d from account <%s>\n", amount, accountName);
    		printf("Current balance is: %d\n\n", balance);
		}
	}



	/* 
	 *  Operation to transfer balance from account to another account 
	 */
	else if (strcmp(operationName, "TRANSFER") == 0) {
		if (argc != 7) {
			printf("\nIncorrect number of arguments."
				"The correct format is:\n\tserverIP serverPort WITHDRAW accountName amount\n");
			exit(1);
    	}
    	accountName = argv[4];
    	amount = atoi(argv[5]);
    	toAccount = argv[6];

    	/* Send the to-accountName to the server */
    	strcpy(sndBuf, accountName);
    	send(clientSock, sndBuf, strlen(sndBuf), 0);
    	memset(&sndBuf, 0, SNDBUFSIZE);

    	/* Ok */
    	recv(clientSock, rcvBuf, 3, 0);
    	memset(&rcvBuf, 0, RCVBUFSIZE);

    	/* Send amount */
    	send(clientSock, &amount, sizeof(int), 0);

    	/* Ok */
    	recv(clientSock, rcvBuf, 3, 0);
    	memset(&rcvBuf, 0, RCVBUFSIZE);

    	/* Send the from-accountName to the server */
    	strcpy(sndBuf, toAccount);
    	send(clientSock, sndBuf, strlen(sndBuf), 0);
    	memset(&sndBuf, 0, SNDBUFSIZE);


    	recv(clientSock, &balance, sizeof(int), 0);
    	send(clientSock, "OK", 3,0);
    	recv(clientSock, &toBalance, sizeof(int), 0);

    	if (balance >= 0 && toBalance >= 0) { 
    		printf("\nTransfer %d from account <%s> to account <%s>\n", amount, accountName, toAccount);
    		printf("Current balance after transfering is:\n");
    		printf("<%s> : %d        <%s> : %d\n\n", accountName, balance, toAccount, toBalance);
		}
	}



	/* return error when operation is invalid */
	else {
		printf("\nNo such operation, please enter BAL, WITHDRAW or TRANSFER.\n\n");
		exit(1);
	}


	/* check and return error types */
	if (balance == -1) {
    	printf("\nInvalid operation!\n\n");
    }
    else if (balance == -2) {
    	printf("\nAccount name doesn't exist!\n\n");
    }
    else if (balance == -3) {
    	printf("\nNo enought balance in account <%s>\n\n", accountName);
    }
    else if (balance == -4) {
    	printf("\nOnly up to 3 withdraw allowed in one minute, try again later.\n\n");
    }

    close(clientSock);
    return 0;
}


