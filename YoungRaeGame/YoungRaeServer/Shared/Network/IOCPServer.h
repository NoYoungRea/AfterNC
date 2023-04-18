#include <winsock2.h> //wsaoverlapped
#include <Windows.h>
#include <set>
#include <vector>
#include <thread>
#include <functional>

class IOCPSocket;
class IOCPServer
{
    using Allocator = std::function<std::shared_ptr<IOCPSocket>(SOCKET)>;
	class SocketContainer
	{
	public:
		void AddSocket(std::shared_ptr<IOCPSocket> socket);
		void RemoveSocket(std::shared_ptr<IOCPSocket> socket);
		size_t CurrentConnection();
	private:
		std::set<std::shared_ptr<IOCPSocket>> m_SocketSet;
	};

public:

	bool Start(WCHAR* ip, WORD wPort, BYTE byWorkerThreadNum, BYTE byRunningThread, DWORD dwMaxConect, Allocator allocator);

private:
	unsigned int WINAPI AcceptThread();
	unsigned int WINAPI WorkerThread();

	SOCKET m_ListenSocket;
	HANDLE m_IOCP;
	DWORD m_MaxConnection;
	Allocator m_Allocator;
	SocketContainer m_SocketContainer;
	std::vector<std::thread> m_ThreadContainer;
};