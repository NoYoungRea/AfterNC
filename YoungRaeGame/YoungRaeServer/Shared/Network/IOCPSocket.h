
#include <winsock2.h> //wsaoverlapped
#include "BinaryBuffer.h"

class IOCPSocket {

friend class IOCPServer;
private:
    SOCKET clientSocket;
    SRWLOCK sessionLock;

    WSAOVERLAPPED recvOverLapped;
    WSAOVERLAPPED sendOverLapped;

    BinaryBuffer recvBuffer;

    //¾Ö¸Å
    BOOL bSendIO;
};
