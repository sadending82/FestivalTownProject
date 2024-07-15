#include "Server/Server.h";

#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"MSWSock.lib")

TableManager* gTableManager = new TableManager();
PacketManager* gPacketManager = new PacketManager();
Server* gServer = new Server();
DB* gDB = new DB();

int main()
{
	std::wcout.imbue(std::locale("korean"));

	gTableManager->ReadItemTable();

	gServer->Init(gPacketManager, gTableManager, gDB);
	gServer->ThreadRun();

	delete gServer;
	delete gPacketManager;
	delete gTableManager;
	delete gDB;

	return 0;
}