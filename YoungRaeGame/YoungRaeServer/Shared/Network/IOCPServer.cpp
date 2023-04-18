#include "IOCPServer.h"

#pragma comment(lib, "Ws2_32.lib")
#include <WS2tcpip.h>
#include <process.h> //_beginthreadex
#include <memory>

#include "IOCPSocket.h"
#include "NetworkHeader.h"
#include "Packet.h"

//오픈 IP / 포트 / 워커스레드 수(생성수, 러닝수) / 나글옵션 / 최대접속자 수
bool IOCPServer::Start(WCHAR* ip, WORD wPort, BYTE byWorkerThreadNum, BYTE byRunningThread, DWORD dwMaxConect, Allocator allocator)
{
    std::shared_ptr<int> a;
    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    //_LOG( L"winsock reset error\n");
        return false;

    // socket()
    m_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_ListenSocket == INVALID_SOCKET)
        //_LOG( L"listen_sock invalid socket error\n");
        return false;

    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    InetPton(AF_INET, ip, &serveraddr.sin_addr.s_addr);
    serveraddr.sin_port = htons(wPort);
    int retval = bind(m_ListenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR)
        //_LOG( L"bind error\n");
        return false;

    // listen()
    retval = listen(m_ListenSocket, SOMAXCONN_HINT(1000));
    if (retval == SOCKET_ERROR)
        //_LOG( L"Listen error\n");
        return false;

    //socket option setting (so_linger & sendBuffer=0)
    LINGER _linger;
    _linger.l_linger = 0;
    _linger.l_onoff = 1;
    int i_sendBufSize = 0;
    setsockopt(m_ListenSocket, SOL_SOCKET, SO_LINGER, (char*)&_linger, sizeof(LINGER));
    setsockopt(m_ListenSocket, SOL_SOCKET, SO_SNDBUF, (char*)&i_sendBufSize, sizeof(int));

    // 입출력 완료 포트 생성
    m_IOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, byRunningThread);//CoreCount
    
    //스레드 셋팅
    auto acceptThread = std::thread([this] { AcceptThread(); });
    m_ThreadContainer.push_back(std::move(acceptThread));
       
    for (size_t i = 0; i < byWorkerThreadNum; i++)
    {
        auto workerThread = std::thread([this] { WorkerThread(); });
        m_ThreadContainer.push_back(std::move(workerThread));
    }

    m_MaxConnection = dwMaxConect;
    m_Allocator = allocator;

    return true;
}

unsigned int WINAPI IOCPServer::AcceptThread()
{
    // 데이터 통신에 사용할 변수
    SOCKET clientSocket;
    SOCKADDR_IN clientAddress;
    int addrlen;
    addrlen = sizeof(clientAddress);

    while (1) {
        // accept()
        clientSocket = accept(m_ListenSocket, (SOCKADDR*)&clientAddress, &addrlen);
        if (clientSocket == INVALID_SOCKET) {
            //wprintf(L"accept error\n");
            break;
        }

        // 사용자 정의 소켓 할당
        std::shared_ptr<IOCPSocket> pSocket = m_Allocator(clientSocket);
        m_SocketContainer.AddSocket(pSocket);
        
        // 소켓과 입출력 완료 포트 연결
        CreateIoCompletionPort((HANDLE)clientSocket, m_IOCP, (ULONG_PTR)pSocket.get(), 0);
        //Recvpost;
    }
    return 0;
}

unsigned int WINAPI IOCPServer::WorkerThread() {
    int error;
    while (1) {

        DWORD dwTransferred = 0;
        IOCPSocket* pIOCPSocket = NULL;
        LPOVERLAPPED pOverlapped = NULL;
        error = 0;
        GetQueuedCompletionStatus(m_IOCP, &dwTransferred, (PULONG_PTR)&pIOCPSocket, &pOverlapped, INFINITE);
        if (dwTransferred == 0 && pIOCPSocket == NULL && pOverlapped == NULL) {
            return 0;
        }
        if (dwTransferred == 0) {
            //closeSocket
        }
        if (&pIOCPSocket->recvOverLapped == pOverlapped) {
            //너무 많은 양의 데이터 요청 정상적인 상황이 아님
            auto& recvBuffer = pIOCPSocket->recvBuffer;
            if (!recvBuffer.SystemPutData(dwTransferred))
            {
                //추방
            }
            while (1)
            {
                //헤더 정보 살짝 엿보기
                NetworkHeader header;
                if (!recvBuffer.PeekData((BYTE*)(&header), sizeof(NetworkHeader)))
                {
                    //헤더 크기만큼의 데이터가 안 들어옴
                    break;
                }
                if (recvBuffer.GetUseSize() < header.wPayloadSize + sizeof(NetworkHeader))
                {
                    //비지니스 로직을 위한 패킷 크기만큼의 데이터가 안 들어옴
                    break;
                }
                recvBuffer.MoveStartCursor(sizeof(NetworkHeader));

                Packet packet;
                recvBuffer.GetData(packet.GetPayloadPtr(), header.wPayloadSize);
                packet.Decoding(&header);

                OnRecv()
            }
        }
        else if (&pIOCPSocket->sendOverLapped == pOverlapped) {
            if (dwTransferred == 0) {

            }
            else {

                int MTUCount = (dwTransferred - 1) / 1460 + 1;
                InterlockedAdd((LONG*)&(myThis->_dwSendBytesCal), dwTransferred + MTUCount * (20 + 20 + 18));
            }
            int i = 0;
            while (1) {
                if (pIOCPSocket->forDelete[i] == NULL)
                    break;
                int a = pIOCPSocket->forDelete[i]->SubRef();
                if (a == 0)
                    pIOCPSocket->forDelete[i]->Free();
                i++;
            }

            pIOCPSocket->bSendIO = FALSE;


            myThis->SendPost(pIOCPSocket);
        }
        else {
            wprintf(L"WorkerThread's strange overlapped pointer\n");
            throw 1;
        }

    END:
        if (InterlockedDecrement(&pIOCPSocket->byIOCount) == 0) {
            myThis->SessionRelease(pIOCPSocket->sessionID);

        }
    }
    return 0;
}

void IOCPServer::SocketContainer::AddSocket(std::shared_ptr<IOCPSocket> socket)
{
     //emplace와 다른게 뭘까?
    m_SocketSet.insert(socket);
}

void IOCPServer::SocketContainer::RemoveSocket(std::shared_ptr<IOCPSocket> socket)
{
    m_SocketSet.erase(socket);
}

size_t IOCPServer::SocketContainer::CurrentConnection()
{
    return m_SocketSet.size();
}


