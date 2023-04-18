
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

    //�ָ�
    BOOL bSendIO;
};
