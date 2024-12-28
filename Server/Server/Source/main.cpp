#include "Server/Server.h";
#include "TableManager/TableManager.h"
#include "PacketManager/PacketManager.h"
#include "DB/DB.h"

#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>

#pragma comment (lib,"WS2_32.lib")
#pragma comment (lib,"MSWSock.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#define DEBUG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif

Server* gServer = new Server();

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	/*std::wcout.imbue(std::locale("korean"));*/

	std::locale::global(std::locale("en_US.UTF-8"));
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	gServer->Run();
	gServer->ThreadJoin();

	delete gServer;
	_CrtDumpMemoryLeaks();

	return 0;
}