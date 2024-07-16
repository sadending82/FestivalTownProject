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

    std::wstring odbc = L"PartyPeople";
    std::wstring id = L"hh";
    std::wstring pw = L"tjdwo104";
};