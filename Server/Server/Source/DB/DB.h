#pragma once
#include "../DB/Security/Security.h"
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <fstream>

class DB {
public:

    DB();
    ~DB();
    void ShowError(SQLHANDLE handle, SQLSMALLINT handleType, RETCODE retcode);
    bool ReadConfig();
	int Init();
    bool Connect();

    bool UseAccountDB(SQLHSTMT& hStmt);
    bool UseGameDB(SQLHSTMT& hStmt);

    bool InsertNewAcccount(const char* id, const char* pw);

private:
    SQLHENV hEnv = NULL;
    SQLHDBC hDbc = NULL;

    std::wstring odbc = L"";
    std::wstring id = L"";
    std::wstring pw = L"";

    //Security* mSecurity = nullptr;
};