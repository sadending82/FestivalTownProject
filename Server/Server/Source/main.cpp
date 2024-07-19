#include "Server/Server.h";
#include "TableManager/TableManager.h"
#include "PacketManager/PacketManager.h"
#include "DB/DB.h"

TableManager* gTableManager = new TableManager();
PacketManager* gPacketManager = new PacketManager();
Server* gServer = new Server();
DB* gDB = new DB();

int main()
{
	std::wcout.imbue(std::locale("korean"));

	gTableManager->ReadItemTable();

	gServer->Init(gPacketManager, gTableManager, gDB);
	gServer->ThreadJoin();

	delete gServer;
	delete gPacketManager;
	delete gTableManager;
	delete gDB;

	return 0;
}