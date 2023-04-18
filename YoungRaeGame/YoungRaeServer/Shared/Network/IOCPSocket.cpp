#include "IOCPServer.h"
#include <memory>

IOCPSocket::IOCPSocket(SOCKET s)
{
    clientSocket = s;
}

