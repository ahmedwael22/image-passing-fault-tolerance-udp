#include "udpsocket.h"

#ifndef UDPSERVERSOCKET_H
#include <arpa/inet.h>
#define UDPSERVERSOCKET_H
class UDPServerSocket : public UDPSocket
{
public:
    UDPServerSocket();
    bool initializeServer(char *_myAddr, int _myPort);
    ~UDPServerSocket();
};
#endif // UDPSERVERSOCKET_H

UDPServerSocket::UDPServerSocket()
{
    totaldatagrams = 0;
    errordatagrams = 0;
}

bool UDPServerSocket::initializeServer(char *_myAddr, int _myPort)
{

    memset(&peerAddr, 0, sizeof(peerAddr));

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return false;

    myAddress = _myAddr;
    myPort = _myPort;
    // cout<< "myyyyadressss " << myAddress<<endl;

    myAddr.sin_family = AF_INET;
    myAddr.sin_port = htons(myPort);
    myAddr.sin_addr.s_addr =inet_addr(myAddress);


    int bind_return;
    bind_return = bind(sock, (const struct sockaddr *)&myAddr, sizeof(myAddr));

    if (bind_return<0)
        return false;

    return true;
}

UDPServerSocket::~UDPServerSocket ()
{
    //commented out bec part of MS2 only
    
    // cout << "Error Datagrams: " << errordatagrams << endl
    //      << "Total Datagrams: " << totaldatagrams << endl
    //      << "Error/Total: " << (float)errordatagrams / (float)totaldatagrams;
}