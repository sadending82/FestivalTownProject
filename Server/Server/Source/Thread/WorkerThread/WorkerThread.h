#pragma once

class WorkerThread
{
public:
	WorkerThread(class Server* Server, class PacketManager* packetManager)
		:pServer(Server),
		pPacketManager(packetManager)
	{};
	~WorkerThread();

	void RunWorker();

private:
	bool isRun = true;

	class Server* pServer;
	class PacketManager* pPacketManager;
};

