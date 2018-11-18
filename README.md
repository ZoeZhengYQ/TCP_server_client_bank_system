# TCP_server_client_bank_system

Author          : Yingqiao Zheng
Created         : September 15, 2018
Last Modified   : September 21, 2018

Affiliation          : Georgia Institute of Technology


Description
-------------

This project is a implementation of TCP in c with server.c and client.c.

It is a simplified bank system, information of bank account name, amount and operation like BAL, WITHDRAW, TRANSFER are obtained from user by client, and later sent to server. 

1. BAL operation is to return the balance in specified bank account;
2. WITHDRAW operation is to withdraw balance in specified bank account if there is enough 	balance, and WITHDRAW is only allowed 3 times in one minute;
3. TRANSFER operation is to transfer balance from one existing bank account to another.



Installation
------------

To install, simply run

    gcc server.c -std=c99 -o server
    gcc client.c -std=c99 -o client

Note: The -std=c99 flag is required on some C compilers
to allow variable declarations at arbitrary locations in
a function.


Execution
-----------

For server, the correct format is

    ./server serverPort
    ./client


For client, the correct format is

1. For BAL

     ./client serverIP serverPort BAL accountName 

2. For WITHDRAW

	./client serverIP serverPort WITHDRAW accountName amount

3. For TRANSFER

	./client serverIP serverPort TRANSFER from-accountName amount to-accountName



