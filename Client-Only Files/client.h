#ifndef CLIENT_H

#include "udpclientsocket.h"
#include "message.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include<string>

#define MAX_BUFFER_SIZE 65000

#define CLIENT_H
class Client
{
private:
    UDPClientSocket *udpSocket;

public:
    Client(char *_hostname, int _port);
    Message *execute(Message *_message);
    ~Client();
};
#endif // CLIENT_H




Client::Client(char * _hostname, int _port)
{
    udpSocket = new UDPClientSocket;
    udpSocket->initializeClient(_hostname, _port);
}

Message * Client::execute(Message * _message)
{
    int count_timeout = 0;
    destination_label:

        int return_code = udpSocket->writeToSocket(_message->getMessage(), _message->getMessageSize());
        
        if (return_code < 0)
        {
            printf("Error writing message to socket.\n");
            exit(1);
        }


        printf("Client sent request.\n");

        string exitmsg = "q";
        if (strcmp(_message->getMessage(), exitmsg.c_str()) == 0)
        {
            return _message;
        }
        
        char * buffer;
        buffer = (char*) malloc(MAX_BUFFER_SIZE * sizeof(char));





////////
        if (_message->getMessage()[0] == '3')
        {
            int return_code2 = udpSocket->readSocketWithNoBlock(buffer, MAX_BUFFER_SIZE);


            Message * reply_message = new Message (buffer, MAX_BUFFER_SIZE);


            return reply_message;
        }


        //else
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int sec=2;
        int millisec=0;
        int return_code2 = udpSocket->readSocketWithTimeout(buffer, MAX_BUFFER_SIZE, sec, millisec);

        if (return_code2 < 0 && count_timeout < 3)
        {
            count_timeout++;
            cout << "Timeout. Reattempting to send packet.\n\n";
            goto destination_label;
        }

        if (return_code2 >= 0)
        {
            Message * reply_message = new Message (buffer, MAX_BUFFER_SIZE);

            return reply_message;
        }
        else
        {
            string timeout_msg;
            timeout_msg = "99,Timeout. Could not send.";

            strcpy(buffer, timeout_msg.c_str());

            Message * reply_message = new Message (buffer, MAX_BUFFER_SIZE);

            return reply_message;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // int return_code2 = udpSocket->readSocketWithNoBlock(buffer, MAX_BUFFER_SIZE);


        // Message * reply_message = new Message (buffer, MAX_BUFFER_SIZE);

        // return reply_message;

}

Client::~Client()
{
    delete udpSocket;
}