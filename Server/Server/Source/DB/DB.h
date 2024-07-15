#pragma once
#include "../utility.h"
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

class DB {
public:

    DB();
    ~DB();
    void ShowError(SQLHANDLE handle, SQLSMALLINT handleType, RETCODE retcode);
	int Init();
    bool Connect();

    bool InsertNewAcccount(const char* id, const char* pw);

private:
    SQLHENV hEnv = NULL;
    SQLHDBC hDbc = NULL;

    wstring odbc = L"PartyPeople";
    wstring id = L"hh";
    wstring pw = L"tjdwo104";
};