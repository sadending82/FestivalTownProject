#pragma once

class WorkerThread
{
public:
	WorkerThread(class Server* pServer, class PacketManager* pPacketManager) 
		:m_pServer(pServer)
		, m_pPacketManager(pPacketManager)
	{};
	~WorkerThread() {};

	void RunWorker();

private:
	class Server* m_pServer;
	class PacketManager* m_pPacketManager;
};

