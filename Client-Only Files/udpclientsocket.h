#include "udpsocket.h"

#ifndef UDPCLIENTSOCKET_H
#include <arpa/inet.h>
#define UDPCLIENTSOCKET_H
class UDPClientSocket : public UDPSocket
{
public:
    UDPClientSocket();
    bool initializeClient(char *_peerAddr, int _peerPort);
    ~UDPClientSocket();
};
#endif // UDPCLIENTSOCKET_H


UDPClientSocket::UDPClientSocket()
{
    totaldatagrams = 0;
    errordatagrams = 0;
}

bool UDPClientSocket::initializeClient(char * _peerAddr, int _peerPort)
{
    peerAddress = _peerAddr;
    peerPort = _peerPort;

    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(peerPort);
    peerAddr.sin_addr.s_addr =inet_addr(peerAddress);
    
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return false;

    return true;
}


UDPClientSocket::~UDPClientSocket ()
{
}