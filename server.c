/*///////////////////////////////////////////////////////////
*
* FILE:		server.c
* AUTHOR:	Yingqiao Zheng
* PROJECT:	CS 3251 Project 1 - Professor Ellen Zegura 
* DESCRIPTION:	Network Server Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/*Included libraries*/

#include <stdio.h>	  /* for printf() and fprintf() */
#include <stdlib.h>
#include <sys/socket.h>	  /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>	  /* supports all sorts of functionality */
#include <unistd.h>	  /* for close() */
#include <string.h>	  /* support any string ops */
#include <sys/time.h>  /* support record time */

#define RCVBUFSIZE 512		/* The receive buffer size */
#define SNDBUFSIZE 512		/* The send buffer size */
#define BUFSIZE 40		/* Your name can be as many as 40 chars*/
#define ACCOUNTNUM 5	/* Number of total account */
#define OPERATIONNUM 3

/* The main function */
int main(int argc, char *argv[]) {

    int serverSock;				/* Server Socket */
    int clientSock;				/* Client Socket */
    struct sockaddr_in serverAddr;		/* Local address */
    struct sockaddr_in clientAddr;		/* Client address */
    unsigned short serverPort;		/* Server port */
    unsigned int clntLen;			/* Length of address data struct */

    char nameBuf[BUFSIZE];			/* Buff to store account name from client */
    char toNameBuf[BUFSIZE];		/* Buff to store the to-account name in TRANSFER */
    char rcvBuf[RCVBUFSIZE];		/* Buff to receive messages from client */
    char sndBuf[SNDBUFSIZE];		/* Buff to send messages to client */
    char inputOperation[9];			/* To store operation */

    int  balance = -1;				/* Place to record account balance result */
    int  toBalance = -1;			/* Record the to-account balance in TRANSFER */
    int  amount = 0;		
    int  accountCount = 0;			/* To check if there is a match of input account name */
    int  toAccountCount = 0;
    int  index = 0;					/* Current index of account */
    int  toIndex = 0;

    int  bal[ACCOUNTNUM];			/* Default account balance */
    char names[ACCOUNTNUM][11];		/* Default account name */
    char operationName[OPERATIONNUM][9];	/* Default operation name */
    struct timeval time;			/* Record Operation time */
    long timeRecord[ACCOUNTNUM][3] = {0};	/* Store recent 3 WITHDRAW operation time for every account */
    int timeIdx[ACCOUNTNUM] = {0};		/* Index for every account to record time storage */

    /* Set account names */
    strcpy(names[0], "mySavings");
    strcpy(names[1], "myChecking");
    strcpy(names[2], "myCD");
    strcpy(names[3], "my401k");
    strcpy(names[4], "my529");

    /* Set default account balance */
    bal[0] = 50000;
    bal[1] = 15000;
    bal[2] = 8000;
    bal[3] = 900;
    bal[4] = 60;

    /* Set default operation name */
    strcpy(operationName[0], "BAL");
    strcpy(operationName[1], "WITHDRAW");
    strcpy(operationName[2], "TRANSFER");

    memset(&nameBuf, 0, BUFSIZE);
    memset(&rcvBuf, 0, RCVBUFSIZE);

    /* Create new TCP Socket for incoming requests */
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        perror("error creating server socket");
        return -1;
    }

    /* Construct local address structure */
    serverPort = atoi(argv[1]);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero(&(serverAddr.sin_zero), 8);
    
    /* Bind to local address structure */
    if (bind(serverSock, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr)) < 0) {
        perror("Bind failed");
        exit(0);
    }

    /* Listen for incoming connections */
    listen(serverSock, 40);
    memset(&rcvBuf, 0, RCVBUFSIZE);
    memset(&sndBuf, 0, SNDBUFSIZE);

    /* Loop server forever*/
    while(1) {
    /* Accept incoming connection */
    	clntLen = sizeof(struct sockaddr_in);
	    clientSock = accept(serverSock, (struct sockaddr *) &clientAddr, &clntLen);
        if (clientSock < 0) {
            perror("accept failed");

        } 
        
    	/* Receive operation name from client */
    	recv(clientSock, rcvBuf, RCVBUFSIZE, 0);
    	strcpy(inputOperation, rcvBuf);
    	puts(inputOperation);
    	memset(&rcvBuf, 0, RCVBUFSIZE);
    	send(clientSock, "OK", 3, 0);

    /*
     * Exact account balance and return it to client
     */
    	if (strcmp(inputOperation, "BAL") == 0) {
    		balance = 0;
            
            /* Receive account name */
    		recv(clientSock, rcvBuf, RCVBUFSIZE, 0);
    		strcpy(nameBuf, rcvBuf);
    		memset(&rcvBuf, 0, RCVBUFSIZE);
			printf("Account name from client: %s\n", nameBuf);
			
            /* Check if account exists */
			accountCount = 0;
        	for (int i = 0; i < ACCOUNTNUM; ++i) {
            	if (strcmp(nameBuf, names[i]) == 0) {
                	balance = bal[i];
                	accountCount++;
            	}
        	}
            /* Account doesn't exist, return error type */
        	if (accountCount == 0) { balance = -2; }
            
            /* send balance or error type to client */
        	send(clientSock, &balance, sizeof(int), 0);
        	memset(&rcvBuf, 0, RCVBUFSIZE);
        	memset(&sndBuf, 0, SNDBUFSIZE);
    	}



    /*
     * withdraw balance from account
     */
    	else if (strcmp(inputOperation, "WITHDRAW") == 0) {
    		balance = 0;
            /* Receive account name */
    		recv(clientSock, rcvBuf, RCVBUFSIZE, 0);
    		strcpy(nameBuf, rcvBuf);
    		memset(&rcvBuf, 0, RCVBUFSIZE);
			printf("Account name from client: %s\n", nameBuf);
            
            /* Send OK to prevent sending overlap */
			send(clientSock, "OK", 3, 0);
            
			/* receive amount */
			recv(clientSock, &amount, sizeof(int), 0);
            
            /* Record operation time */
			gettimeofday(&time, NULL);
            
            /* Check if account exists */
			accountCount = 0;
			for (int i = 0; i < ACCOUNTNUM; ++i) {
            	if (strcmp(nameBuf, names[i]) == 0) {
                	balance = bal[i];
                	index = i;
                	accountCount++;
            	}
        	}
        	if (accountCount == 0) { balance = -2; }
   			
            /* check if there is enough balance for withdrawal */
   			if (accountCount != 0 && amount > balance) { 
   				balance = -3; 
   			}
   			else if (amount <= balance) {
                /*  Swap when there is already three withdraw in record */
   				if (timeIdx[index] >= 3) { timeIdx[index] -=3; }
                /*  Store operation time down if there is no record */
   				if (timeRecord[index][timeIdx[index]] == 0) {
   					timeRecord[index][timeIdx[index]] = time.tv_sec;
   					timeIdx[index]++;
   					printf("time of operation: %ld\n", time.tv_sec);
                    
                    /* Substract current balance with withdraw amount */
   					bal[index] = bal[index] - amount;
   					balance = bal[index];
   				}
                /*  If there are already three withdraw in one minute, return error type -4 */
   				else if (time.tv_sec - timeRecord[index][timeIdx[index]] < 60) {
   					balance = -4;
   				}
                /* Update operation time, if this withdraw is one minute later from the first one */
   				else {
   					timeRecord[index][timeIdx[index]] = time.tv_sec;
   					timeIdx[index]++;
   					printf("time of operation: %ld\n", time.tv_sec);
                    
                    /* Substract current balance with withdraw amount */
   					bal[index] = bal[index] - amount;
   					balance = bal[index];
   				}
   			}
            /* Send balance or error type to client */
        	send(clientSock, &balance, sizeof(int), 0);
   			memset(&sndBuf, 0, SNDBUFSIZE);
   			memset(&rcvBuf, 0, RCVBUFSIZE);
    	}


    /*
     * Transfer balance from one account to another
     */
    	else if (strcmp(inputOperation, "TRANSFER") == 0) {
    		balance = 0;
    		/* Receive fromAccount name from client */
    		recv(clientSock, rcvBuf, RCVBUFSIZE, 0);
    		strcpy(nameBuf, rcvBuf);
    		memset(&rcvBuf, 0, RCVBUFSIZE);
			printf("from-Account name from client: %s\n", nameBuf);

			/* Send OK */
			send(clientSock, "OK", 3, 0);

			/* Receive amount form client */
			recv(clientSock, &amount, sizeof(int), 0);

			/* Send OK */
			send(clientSock, "OK", 3, 0);

			/* Receive fromAccount name from client */
    		recv(clientSock, rcvBuf, RCVBUFSIZE, 0);
    		strcpy(toNameBuf, rcvBuf);
    		memset(&rcvBuf, 0, RCVBUFSIZE);
			printf("to-Account name from client: %s\n", toNameBuf);

            /* Check if both account exist */
			accountCount = 0;
			toAccountCount = 0;
        	for (int i = 0; i < ACCOUNTNUM; ++i) {
            	if (strcmp(nameBuf, names[i]) == 0) {
                	balance = bal[i];
                	index = i;
                	accountCount++;
            	}

            	if (strcmp(toNameBuf, names[i]) == 0) {
                	toBalance = bal[i];
                	toIndex = i;
                	toAccountCount++;
            	}
        	}
        	if (accountCount == 0 || toAccountCount == 0) {
            	balance = -2;
            	toBalance = -2;
        	}
            /* Check if there is enough balance in from-account */
        	if (accountCount != 0 && toAccountCount != 0 && amount > balance) {
   				balance = -3;
   				toBalance = -3;
   			}
            
            /* Update balance in both account */
   			else if (amount <= balance){
   				bal[index] = bal[index] - amount;
   				bal[toIndex] = bal[toIndex] + amount;

   				balance = bal[index];
   				toBalance = bal[toIndex];
   			}
            
            /* Send balance or error type in both account to client */
   			send(clientSock, &balance, sizeof(int), 0);
   			recv(clientSock, rcvBuf, 3, 0);
            memset(&rcvBuf, 0, RCVBUFSIZE);
            send(clientSock, &toBalance, sizeof(int), 0);
    	}

        /* When operation is invalid, send error type -1 to client */
    	else { 
    		balance = -1;
    		send(clientSock, &balance, sizeof(int), 0);
    		memset(&sndBuf, 0, SNDBUFSIZE);
    	}        

    }

    close(serverSock);
    return 0;

}
