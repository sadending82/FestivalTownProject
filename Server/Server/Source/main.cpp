#include "Server/Server.h";
#include "TableManager/TableManager.h"
#include "PacketManager/PacketManager.h"
#include "DB/DB.h"

TableManager* gTableManager = new TableManager();
Server* gServer = new Server();
DB* gDB = new DB();

int main()
{
	std::wcout.imbue(std::locale("korean"));

	gTableManager->ReadCharacterStat();

	gServer->Init(gTableManager, gDB);
	gServer->ThreadJoin();

	delete gServer;
	delete gTableManager;
	delete gDB;

	return 0;
}