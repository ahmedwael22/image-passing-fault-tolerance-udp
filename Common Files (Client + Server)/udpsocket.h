#include <sys/socket.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <time.h>

#ifndef UDPSOCKET_H
#define UDPSOCKET_H
class UDPSocket
{
protected:
    int sock;
    sockaddr_in myAddr;
    sockaddr_in peerAddr;
    char *myAddress;
    char *peerAddress;
    int myPort;
    int peerPort;
    bool enabled;
    pthread_mutex_t mutex;

    int totaldatagrams;
    int errordatagrams;

public:
    UDPSocket();
    void setFilterAddress(char *_filterAddress);
    char *getFilterAddress();
    bool initializeServer(char *_myAddr, int _myPort);
    bool initializeClient(char *_peerAddr, int _peerPort);
    int writeToSocket(char *buffer, int maxBytes);
    int writeToSocketAndWait(char *buffer, int maxBytes, int microSec);
    int readFromSocketWithNoBlock(char *buffer, int maxBytes);
    int readFromSocketWithTimeout(char *buffer, int maxBytes, int timeoutSec, int timeoutMilli);
    int readFromSocketWithBlock(char *buffer, int maxBytes);
    int readSocketWithNoBlock(char *buffer, int maxBytes);
    int readSocketWithTimeout(char *buffer, int maxBytes, int timeoutSec, int timeoutMilli);
    int readSocketWithBlock(char *buffer, int maxBytes);
    int getMyPort();
    int getPeerPort();
    void enable();
    void disable();
    bool isEnabled();
    void lock();
    void unlock();
    int getSocketHandler();
    ~UDPSocket();

    //changed
    char * getMyAddress();
    char * getPeerAddress();
    void incrementExperimentTotal();
    void incrementExperimentErrors();
};
#endif // UDPSOCKET_H


UDPSocket::UDPSocket()
{
    
}

int UDPSocket::getPeerPort()
{
    return peerPort;
}

char * UDPSocket::getPeerAddress()
{
    return peerAddress;
}

int UDPSocket::writeToSocket(char *buffer, int maxBytes)
{
    int return_code = sendto(sock, buffer, maxBytes,
                    MSG_CONFIRM, (sockaddr *) &peerAddr, sizeof(peerAddr));
    
    return return_code;
}


///////////////////////////////
int UDPSocket::readSocketWithTimeout(char *buffer, int maxBytes, int timeoutSec, int timeoutMilli)
{

    socklen_t length;
    timeval timeout={timeoutSec,timeoutMilli};

    setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));

    int size = recvfrom(sock, buffer, maxBytes,
                            0, (struct sockaddr *)&peerAddr, &length);

    // cout << size << endl; 


    return size;
}
///////////////////////////////



int UDPSocket::readSocketWithNoBlock(char *buffer, int maxBytes)
{

    socklen_t length;

    
    int size = recvfrom(sock, buffer, maxBytes,
                        MSG_WAITALL, (sockaddr *) &peerAddr, &length);

  
    if (size < 0)
        return -1;
    


    // buffer[size] = '\0'; //NULL Terminate the string

    return size;
    
}

void UDPSocket::incrementExperimentErrors()
{
    errordatagrams++;
}

void UDPSocket::incrementExperimentTotal()
{
    totaldatagrams++;
}

UDPSocket::~UDPSocket ()
{
}