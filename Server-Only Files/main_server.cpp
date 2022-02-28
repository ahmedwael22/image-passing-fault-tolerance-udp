//system libraries
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <netdb.h>

using namespace std;

//user-defined classes
// #include "message.h"
#include "server.h"

int main (int argc, char** argv)
{

    char * host= argv[1];
    int port = 9999;

    Server my_server(host, port);  

    cout << "Server Initiated.\n";

    bool continueserving;
    while (true)
    {
        continueserving = my_server.serveRequest();
        if (!continueserving)
            break;
        // cout<<"Back to main!\n\n";
    }
    
    return 0;
}

/*
The experiment that we conducted was calculating the ratio of successful reads in the server to its total reads. 
The Printing Statement (cout) occurs in the destructor of the server socket.


In the two runs of the server that we have demonstrated we either have error of 0 (no datagram errors/loss at all) or 1 (100%). 
In reality, the number would definitely be in between 0 and 1. This can be achieved by a small modification to our code: making our READ FROM SOCKET have some timeout, so that we would miss a datagram without the program hanging.

*/