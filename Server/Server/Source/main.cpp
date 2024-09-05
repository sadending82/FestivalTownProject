#include "Server/Server.h";
#include "TableManager/TableManager.h"
#include "PacketManager/PacketManager.h"
#include "DB/DB.h"

#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif

TableManager* gTableManager = new TableManager();
Server* gServer = new Server();
DB* gDB = new DB();

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	std::wcout.imbue(std::locale("korean"));

	gTableManager->ReadAllDataTable();
	gServer->Run(gTableManager, gDB);
	gServer->ThreadJoin();


	delete gServer;
	delete gTableManager;
	delete gDB;

	_CrtDumpMemoryLeaks();

	return 0;
}